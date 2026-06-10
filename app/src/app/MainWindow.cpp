#include "app/MainWindow.h"

#include "command/LegacyUiCommandMap.h"
#include "geometry/occ/import/OcctStepImportService.h"
#include "geometry/occ/legacy_adapter/OccLegacyGeometryAdapter.h"
#include "presentation/occ/OccViewerWidget.h"
#include "presentation/occ/RebarAisPresentationAdapter.h"
#include "ui/LineGroupParameterDialog.h"
#include "ui/MainTabs.h"
#include "ui/ModelTreePanel.h"

#include <QAction>
#include <QDockWidget>
#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QLabel>
#include <QStatusBar>
#include <QTabWidget>
#include <QToolBar>
#include <QVBoxLayout>

#include <algorithm>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace {

class ViewerLegacyRebarGeometryReader final : public tsrebar::LegacyRebarGeometryReader
{
public:
    explicit ViewerLegacyRebarGeometryReader(const tsrebar::OccViewerWidget& viewer)
        : m_viewer(viewer)
    {
    }

    [[nodiscard]] tsrebar::LegacyGeometryQueryResult<tsrebar::LegacyRebarCurveSnapshot>
    curveSnapshot(const tsrebar::LegacySelectionRef& ref, int requestedSampleCount) const override
    {
        tsrebar::OccLegacyGeometryAdapter adapter(m_viewer.selectionIndex());
        const auto edge = adapter.edgeGeometry(ref);
        if (!edge.ok) {
            return {false, {}, edge.diagnostic};
        }

        tsrebar::LegacyRebarCurveSnapshot snapshot;
        snapshot.stableId = edge.value.stableId;
        snapshot.curveKind = edge.value.curveKind;
        snapshot.length = edge.value.length;
        snapshot.startPoint = edge.value.startPoint;
        snapshot.endPoint = edge.value.endPoint;

        const auto samples =
            adapter.edgeSamplePoints(ref, std::max(2, requestedSampleCount));
        if (samples.ok) {
            snapshot.samplePoints = samples.value;
        } else {
            snapshot.samplePoints = {snapshot.startPoint, snapshot.endPoint};
        }

        return {true, snapshot, {}};
    }

    [[nodiscard]] tsrebar::LegacyGeometryQueryResult<tsrebar::LegacySegmentCurveNormalizeResult>
    normalizeSegmentCurve(
        const tsrebar::LegacyRebarCurveSnapshot& curve,
        const tsrebar::LegacySegmentCurveNormalizeRequest& request) const override
    {
        tsrebar::LegacyGeometryQueryResult<tsrebar::LegacySegmentCurveNormalizeResult> result;
        if (curve.stableId.empty()) {
            result.diagnostic = QStringLiteral("SegmentCurveNormalizer requires a legacy edge stable id.");
            return result;
        }

        tsrebar::LegacySelectionRef ref;
        ref.stableId = curve.stableId;
        ref.shapeKind = tsrebar::LegacyShapeKind::Edge;

        tsrebar::LegacyPoint3d probePoint;
        if (!curve.samplePoints.empty()) {
            probePoint = curve.samplePoints[curve.samplePoints.size() / 2];
        } else {
            probePoint = {(curve.startPoint.x + curve.endPoint.x) * 0.5,
                          (curve.startPoint.y + curve.endPoint.y) * 0.5,
                          (curve.startPoint.z + curve.endPoint.z) * 0.5};
        }

        tsrebar::OccLegacyGeometryAdapter adapter(m_viewer.selectionIndex());
        tsrebar::LegacySegmentCurveNormalizeTrace trace;
        trace.capabilityLevel = "p0-split-spline-trim-summary";
        const int effectiveSplineSampleCount = std::max(
            request.minimumSplineSamples,
            static_cast<int>(curve.length * request.splineSamplesPerUnitLength));
        trace.effectiveSplineSampleCount = effectiveSplineSampleCount;

        trace.entityDistanceAttempted = true;
        const auto groupDistance =
            adapter.pointToEdgeGroupDistance(probePoint,
                                             std::vector<tsrebar::LegacySelectionRef>{ref},
                                             request.unresolvedEndpointDistanceThreshold);
        trace.entityDistanceApplied = groupDistance.ok && groupDistance.value.hasNearest;

        trace.splitCurveAttempted = true;
        const auto split =
            adapter.edgeSplitAtPoint(ref, probePoint, request.minimumSplineSamples);
        trace.splitCurveApplied =
            split.ok &&
            split.value.firstInterval.length >= request.minimumSplitLength &&
            split.value.secondInterval.length >= request.minimumSplitLength;

        trace.curveSplineAttempted = curve.samplePoints.size() >= 3;
        if (trace.curveSplineAttempted) {
            const auto spline =
                adapter.buildSplineFromPoints(curve.samplePoints, effectiveSplineSampleCount);
            if (spline.value.effectiveSampleCount > 0) {
                trace.effectiveSplineSampleCount = spline.value.effectiveSampleCount;
            }
            trace.curveSplineApplied = spline.ok && spline.value.buildable;
        }

        trace.startTrimAttempted = true;
        const auto startTrim =
            adapter.edgeTrimEndpoint(ref, request.trimDelta, 0, request.minimumSplineSamples);
        trace.startTrimApplied = startTrim.ok;

        trace.endTrimAttempted = true;
        const auto endTrim =
            adapter.edgeTrimEndpoint(ref, request.trimDelta, 1, request.minimumSplineSamples);
        trace.endTrimApplied = endTrim.ok;

        result.ok = true;
        result.value.curve = curve;
        result.value.trace = trace;
        return result;
    }

private:
    const tsrebar::OccViewerWidget& m_viewer;
};

tsrebar::RebarLineGroupCommandParameters lineGroupParametersForSequence(int sequence)
{
    const std::string suffix = "p0-" + std::to_string(sequence);
    tsrebar::RebarLineGroupCommandParameters parameters;
    parameters.groupId = "ui-line-group-" + suffix;
    parameters.barId = "ui-line-bar-" + suffix;
    parameters.segmentId = "ui-line-segment-" + suffix;
    parameters.steelDataId = "ui-steel-data-p0";
    parameters.distanceA = 0.25;
    parameters.distanceB = 1.2;
    parameters.legacyFlag = 0;
    parameters.diameter = 25.0;
    parameters.interval = 200.0;
    parameters.requestedBarCount = 1;
    parameters.steelLevel = "HRB400";
    parameters.rsdId = "P0";
    parameters.componentName = "pending-ui";
    parameters.projectSteelName = "line-group-" + suffix;
    return parameters;
}

QString joinIds(const QVector<QString>& values)
{
    QStringList result;
    result.reserve(values.size());
    for (const QString& value : values) {
        result.push_back(value);
    }
    return result.join(QLatin1Char(';'));
}

QString uiSurfaceName(tsrebar::LegacyCommandUiSurface surface)
{
    switch (surface) {
    case tsrebar::LegacyCommandUiSurface::Ribbon:
        return QStringLiteral("Ribbon");
    case tsrebar::LegacyCommandUiSurface::ContextMenu:
        return QStringLiteral("ContextMenu");
    }
    return QStringLiteral("Unknown");
}

QString implementationStateName(tsrebar::LegacyCommandImplementationState state)
{
    switch (state) {
    case tsrebar::LegacyCommandImplementationState::Implemented:
        return QStringLiteral("Implemented");
    case tsrebar::LegacyCommandImplementationState::NotImplemented:
        return QStringLiteral("NotImplemented");
    }
    return QStringLiteral("Unknown");
}

QString commandStatusText(const tsrebar::CommandResult& result)
{
    switch (result.status) {
    case tsrebar::CommandStatus::Completed:
        return QStringLiteral("完成：%1").arg(result.message);
    case tsrebar::CommandStatus::NotImplemented:
        return QStringLiteral("未实现：%1").arg(result.message);
    case tsrebar::CommandStatus::Failed:
        return QStringLiteral("失败：%1").arg(result.message);
    }
    return QStringLiteral("未知命令状态");
}

void appendTopologyRefForIds(const std::vector<std::string>& ids,
                             const std::string& shapeType,
                             const std::string& sourceStepId,
                             std::vector<tsrebar::TsRebarTopologyRef>& refs,
                             std::map<std::string, int>& indexByStableId)
{
    for (const std::string& id : ids) {
        if (id.empty() || indexByStableId.find(id) != indexByStableId.end()) {
            continue;
        }
        const int index = static_cast<int>(refs.size());
        indexByStableId[id] = index;
        refs.push_back({id, shapeType, sourceStepId, id, {"E-DEV-108"}});
    }
}

void appendTopologyRefsFromGeometry(const tsrebar::GeometryReference& geometry,
                                    const std::string& sourceStepId,
                                    std::vector<tsrebar::TsRebarTopologyRef>& refs,
                                    std::map<std::string, int>& indexByStableId)
{
    appendTopologyRefForIds(geometry.faceStableIds, "face", sourceStepId,
                            refs, indexByStableId);
    appendTopologyRefForIds(geometry.edgeStableIds, "edge", sourceStepId,
                            refs, indexByStableId);
    appendTopologyRefForIds(geometry.curveStableIds, "edge", sourceStepId,
                            refs, indexByStableId);
}

std::optional<std::string> firstGeometryStableId(const tsrebar::GeometryReference& geometry)
{
    if (!geometry.curveStableIds.empty()) {
        return geometry.curveStableIds.front();
    }
    if (!geometry.edgeStableIds.empty()) {
        return geometry.edgeStableIds.front();
    }
    if (!geometry.faceStableIds.empty()) {
        return geometry.faceStableIds.front();
    }
    return std::nullopt;
}

std::string topologyGeometryPathFor(const std::map<std::string, int>& indexByStableId,
                                    const tsrebar::GeometryReference& geometry)
{
    const std::optional<std::string> stableId = firstGeometryStableId(geometry);
    if (!stableId.has_value()) {
        return {};
    }
    const auto found = indexByStableId.find(*stableId);
    if (found == indexByStableId.end()) {
        return {};
    }
    return "geometry/topology_refs.json#/topologyRefs/" + std::to_string(found->second);
}

void rewriteBindingForRuntimeSave(tsrebar::BindingAnchor& binding,
                                  const tsrebar::GeometryReference& geometry,
                                  const std::map<std::string, int>& indexByStableId,
                                  const char* fallbackLegacyPath)
{
    if (binding.state != tsrebar::BindingState::Resolved) {
        return;
    }

    const std::string geometryPath = topologyGeometryPathFor(indexByStableId, geometry);
    if (geometryPath.empty()) {
        return;
    }

    std::string legacyPath = fallbackLegacyPath;
    std::string evidenceId = "E-DEV-108";
    if (!binding.items.empty()) {
        if (!binding.items.front().legacyPath.empty()) {
            legacyPath = binding.items.front().legacyPath;
        }
        if (!binding.items.front().evidenceId.empty()) {
            evidenceId = binding.items.front().evidenceId;
        }
    }

    binding.items = {tsrebar::BindingItem{legacyPath, geometryPath, evidenceId}};
}

} // namespace

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    buildUi();
    registerCommandHandlers();
    buildCommandTabs();
    statusBar()->showMessage(QStringLiteral("就绪"));
}

void MainWindow::registerCommandHandlers()
{
    m_commands.registerHandler(tsrebar::CommandId::ProjectSave, [this]() {
        if (m_projectPackagePath.isEmpty()) {
            return tsrebar::CommandResult{
                tsrebar::CommandStatus::Failed,
                QStringLiteral("Project.Save 需要工程包路径；TODO-086 仅提供保存清 dirty P0 入口。")};
        }

        m_lastSaveResult = m_projectRuntime.saveSnapshot(
            m_projectPackagePath,
            currentProjectSnapshotForSave(),
            hasPersistentDirtyState());
        applyDirtyStateFromSaveResult(*m_lastSaveResult);
        if (!m_lastSaveResult->ok) {
            return tsrebar::CommandResult{
                tsrebar::CommandStatus::Failed,
                QStringLiteral("保存失败：dirty 保持，validation=%1")
                    .arg(m_lastSaveResult->validation.decision)};
        }

        return tsrebar::CommandResult{
            tsrebar::CommandStatus::Completed,
            QStringLiteral("保存完成：Project/Rebar/Drawing dirty 已清除")};
    });

    m_commands.registerHandler(tsrebar::CommandId::ImportStep, [this]() {
        const QString path = QFileDialog::getOpenFileName(
            this,
            QStringLiteral("导入 STEP"),
            QString(),
            QStringLiteral("STEP 文件 (*.stp *.step)"));

        if (path.isEmpty()) {
            return tsrebar::CommandResult{tsrebar::CommandStatus::Failed,
                                          QStringLiteral("导入已取消")};
        }

        const tsrebar::OcctImportedDocument document =
            tsrebar::OcctStepImportService().importDocument(path);
        if (!document.summary.ok) {
            return tsrebar::CommandResult{
                tsrebar::CommandStatus::Failed,
                QString::fromStdString(document.summary.error)};
        }

        QString displayError;
        if (!m_viewer->displayDocument(document, &displayError)) {
            return tsrebar::CommandResult{
                tsrebar::CommandStatus::Failed,
                displayError};
        }

        m_modelTree->setImportSummary(QFileInfo(path).fileName(),
                                      QString::fromStdString(document.summary.lengthUnit),
                                      document.summary.solids,
                                      document.summary.faces,
                                      document.summary.edges,
                                      document.summary.vertices);

        return tsrebar::CommandResult{
            tsrebar::CommandStatus::Completed,
            QStringLiteral("%1: solids=%2, faces=%3, unit=%4")
                .arg(QFileInfo(path).fileName())
                .arg(document.summary.solids)
                .arg(document.summary.faces)
                .arg(QString::fromStdString(document.summary.lengthUnit))};
    });

    m_commands.registerHandler(tsrebar::CommandId::ViewFitAll, [this]() {
        QString error;
        if (!m_viewer->fitAll(&error)) {
            return tsrebar::CommandResult{tsrebar::CommandStatus::Failed, error};
        }
        return tsrebar::CommandResult{tsrebar::CommandStatus::Completed,
                                      QStringLiteral("全显")};
    });

    m_commands.registerHandler(tsrebar::CommandId::ViewSelectFace, [this]() {
        m_viewer->setSelectionMode(TopAbs_FACE);
        return tsrebar::CommandResult{tsrebar::CommandStatus::Completed,
                                      QStringLiteral("选择模式：面")};
    });

    m_commands.registerHandler(tsrebar::CommandId::ViewSelectEdge, [this]() {
        m_viewer->setSelectionMode(TopAbs_EDGE);
        return tsrebar::CommandResult{tsrebar::CommandStatus::Completed,
                                      QStringLiteral("选择模式：边")};
    });

    m_commands.registerHandler(tsrebar::CommandId::ViewSelectVertex, [this]() {
        m_viewer->setSelectionMode(TopAbs_VERTEX);
        return tsrebar::CommandResult{tsrebar::CommandStatus::Completed,
                                      QStringLiteral("选择模式：点")};
    });

    m_lineGroupParameters = lineGroupParametersForSequence(m_lineGroupCreateSequence);
    m_lineGroupGeometryReader =
        std::make_shared<ViewerLegacyRebarGeometryReader>(*m_viewer);
    m_lineGroupHandler = std::make_shared<tsrebar::RebarLineGroupCommandHandler>(
        [this]() {
            std::vector<tsrebar::LegacySelectionRef> selection;
            const std::optional<tsrebar::LegacySelectionRef> current =
                m_viewer->currentSelectionRef();
            if (current.has_value()) {
                selection.push_back(*current);
            }
            return selection;
        },
        *m_lineGroupGeometryReader,
        m_steelData,
        m_lineGroupParameters);
    tsrebar::registerRebarLineGroupCommandHandler(m_commands, m_lineGroupHandler);

    tsrebar::registerLegacyUiCommandPlaceholders(m_commands);
}

bool MainWindow::verifyLegacyUiActionMetadata(QString* errorMessage) const
{
    const auto commands = tsrebar::legacyUiCommands();
    for (const auto& command : commands) {
        if (command.uiSurface != tsrebar::LegacyCommandUiSurface::Ribbon) {
            continue;
        }

        const auto actions = findChildren<QAction*>(command.objectName);
        if (actions.size() != 1) {
            if (errorMessage) {
                *errorMessage = QStringLiteral("%1 action count=%2")
                                    .arg(command.commandKey)
                                    .arg(actions.size());
            }
            return false;
        }

        const QAction* action = actions.front();
        if (action->text() != command.caption ||
            action->property("commandKey").toString() != command.commandKey ||
            action->property("legacyUiPath").toString() != command.legacyUiPath ||
            action->property("legacyUiSurface").toString() != uiSurfaceName(command.uiSurface) ||
            action->property("implementationState").toString() !=
                implementationStateName(command.implementationState) ||
            action->property("legacyCommand").toString() != command.legacyCommand ||
            action->property("legacyContextCommandId").toString() !=
                command.legacyContextCommandId ||
            action->property("evidenceIds").toString() != joinIds(command.evidenceIds) ||
            action->property("sourceRefs").toString() != joinIds(command.sourceRefs) ||
            action->property("gapIds").toString() != joinIds(command.gapIds) ||
            action->property("inputSelectionTypes").toString() !=
                joinIds(command.inputSelectionTypes)) {
            if (errorMessage) {
                *errorMessage =
                    QStringLiteral("%1 action traceability metadata mismatch")
                        .arg(command.commandKey);
            }
            return false;
        }
    }

    for (const auto& command : commands) {
        if (command.uiSurface != tsrebar::LegacyCommandUiSurface::ContextMenu) {
            continue;
        }
        if (!findChildren<QAction*>(command.objectName).isEmpty()) {
            if (errorMessage) {
                *errorMessage =
                    QStringLiteral("%1 context menu action unexpectedly rendered in ribbon")
                        .arg(command.commandKey);
            }
            return false;
        }
    }

    return true;
}

const tsrebar::SteelData& MainWindow::steelDataForInspection() const
{
    return m_steelData;
}

const MainWindow::DirtyState& MainWindow::dirtyStateForInspection() const
{
    return m_dirtyState;
}

void MainWindow::setProjectPackagePathForInspection(const QString& path)
{
    m_projectPackagePath = path;
}

const std::optional<tsrebar::TsRebarProjectSaveResult>&
MainWindow::lastSaveResultForInspection() const
{
    return m_lastSaveResult;
}

void MainWindow::buildUi()
{
    setWindowTitle(QStringLiteral("图石钢筋 1:1 复刻"));

    auto* central = new QWidget(this);
    auto* layout = new QVBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);

    m_tabs = new QTabWidget(central);
    layout->addWidget(m_tabs);

    m_viewer = new tsrebar::OccViewerWidget(central);
    layout->addWidget(m_viewer, 1);

    setCentralWidget(central);

    m_modelTree = new ModelTreePanel(this);
    auto* modelDock = new QDockWidget(QStringLiteral("模型树"), this);
    modelDock->setObjectName(QStringLiteral("dock_model_tree"));
    modelDock->setWidget(m_modelTree);
    addDockWidget(Qt::LeftDockWidgetArea, modelDock);

    auto* propertyDock = new QDockWidget(QStringLiteral("属性"), this);
    propertyDock->setObjectName(QStringLiteral("dock_properties"));
    auto* propertyLabel = new QLabel(QStringLiteral("选择对象后显示属性"), propertyDock);
    propertyLabel->setAlignment(Qt::AlignCenter);
    propertyLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    propertyDock->setWidget(propertyLabel);
    addDockWidget(Qt::RightDockWidgetArea, propertyDock);

    connect(m_viewer,
            &tsrebar::OccViewerWidget::selectionChanged,
            this,
            [this, propertyLabel](const QString& summary,
                                  const QString& details,
                                  const QString& stableId) {
                if (stableId.isEmpty()) {
                    propertyLabel->setText(QStringLiteral("选择对象后显示属性"));
                    statusBar()->showMessage(QStringLiteral("未选中对象"), 2000);
                    return;
                }
                propertyLabel->setText(details);
                statusBar()->showMessage(QStringLiteral("已选择：%1").arg(summary), 3000);
            });
}

void MainWindow::buildCommandTabs()
{
    const auto tabs = tsrebar::mainTabDefinitions();
    const auto commands = tsrebar::legacyUiCommands();

    for (const auto& tab : tabs) {
        auto* page = new QWidget(m_tabs);
        auto* pageLayout = new QHBoxLayout(page);
        pageLayout->setContentsMargins(8, 8, 8, 8);
        pageLayout->setSpacing(8);

        for (const auto& group : tab.groups) {
            auto* toolbar = new QToolBar(group.title, page);
            toolbar->setObjectName(QStringLiteral("toolbar_%1_%2")
                                       .arg(tab.objectName, group.objectName));
            toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

            for (const auto& command : commands) {
                if (command.uiSurface != tsrebar::LegacyCommandUiSurface::Ribbon ||
                    command.tabObjectName != tab.objectName ||
                    command.groupObjectName != group.objectName) {
                    continue;
                }

                auto* action = new QAction(command.caption, toolbar);
                action->setObjectName(command.objectName);
                action->setProperty("commandKey", command.commandKey);
                action->setProperty("legacyUiPath", command.legacyUiPath);
                action->setProperty("legacyUiSurface", uiSurfaceName(command.uiSurface));
                action->setProperty("implementationState",
                                    implementationStateName(command.implementationState));
                action->setProperty("legacyCommand", command.legacyCommand);
                action->setProperty("legacyContextCommandId",
                                    command.legacyContextCommandId);
                action->setProperty("evidenceIds", joinIds(command.evidenceIds));
                action->setProperty("sourceRefs", joinIds(command.sourceRefs));
                action->setProperty("gapIds", joinIds(command.gapIds));
                action->setProperty("inputSelectionTypes",
                                    joinIds(command.inputSelectionTypes));
                connect(action, &QAction::triggered, this, [this, id = command.id]() {
                    executeCommand(id);
                });
                toolbar->addAction(action);
            }

            pageLayout->addWidget(toolbar);
        }

        pageLayout->addStretch(1);
        m_tabs->addTab(page, tab.title);
    }
}

void MainWindow::executeCommand(tsrebar::CommandId id)
{
    if (id == tsrebar::CommandId::RebarLineCreate) {
        QString preflightError;
        if (!lineGroupSelectionPreflightForCommand(&preflightError)) {
            statusBar()->showMessage(preflightError, 5000);
            return;
        }
        if (!configureLineGroupParametersForCommand()) {
            statusBar()->showMessage(QStringLiteral("取消：线配筋参数"), 5000);
            return;
        }
    }

    const tsrebar::CommandResult result = m_commands.execute(id);
    if (id == tsrebar::CommandId::RebarLineCreate &&
        result.status == tsrebar::CommandStatus::Completed) {
        applyDirtyStateFromCommandResult(result);
        QString displayError;
        if (!displayCreatedLineGroup(&displayError)) {
            statusBar()->showMessage(
                QStringLiteral("完成但显示失败：%1").arg(displayError),
                5000);
            return;
        }
    }
    statusBar()->showMessage(commandStatusText(result), 5000);
}

void MainWindow::applyDirtyStateFromCommandResult(const tsrebar::CommandResult& result)
{
    if (!result.transaction.committed) {
        return;
    }

    m_dirtyState.projectDirty = m_dirtyState.projectDirty || result.dirtyFlags.projectDirty;
    m_dirtyState.geometryDirty = m_dirtyState.geometryDirty || result.dirtyFlags.geometryDirty;
    m_dirtyState.rebarDirty = m_dirtyState.rebarDirty || result.dirtyFlags.rebarDirty;
    m_dirtyState.drawingDirty = m_dirtyState.drawingDirty || result.dirtyFlags.drawingDirty;
    m_dirtyState.selectionDirty = m_dirtyState.selectionDirty || result.dirtyFlags.selectionDirty;
    m_dirtyState.viewDirty = m_dirtyState.viewDirty || result.dirtyFlags.viewDirty;
    ++m_dirtyState.committedTransactionCount;
    m_dirtyState.lastDirtyCommand = result.transaction.commandKey;
    m_dirtyState.legacyDirtyEvidenceId = result.transaction.legacyDirtyEvidenceId;
    m_dirtyState.unresolvedDirtyParityGap = result.transaction.unresolvedDirtyParityGap;
}

void MainWindow::applyDirtyStateFromSaveResult(
    const tsrebar::TsRebarProjectSaveResult& result)
{
    if (!result.ok || result.dirtyAfter) {
        return;
    }

    m_dirtyState.projectDirty = false;
    m_dirtyState.rebarDirty = false;
    m_dirtyState.drawingDirty = false;
}

tsrebar::TsRebarProjectSnapshot MainWindow::currentProjectSnapshotForSave() const
{
    tsrebar::TsRebarProjectSnapshot snapshot;
    snapshot.projectId = "ui-project-p0";
    snapshot.projectName = "ui project p0";
    snapshot.sourceStep.sourceStepId = "step-main";
    snapshot.sourceStep.path = "models/source.step";
    snapshot.sourceStep.originalPath = "ui-p0";
    snapshot.sourceStep.sha256 = "sha256-ui-p0";
    snapshot.steelData = m_steelData;
    snapshot.evidenceIds = {
        "E-DEV-046",
        "E-DEV-107",
        "E-DEV-108",
        "E-IDA-049",
        "GAP-REB-C-002",
    };

    std::map<std::string, int> indexByStableId;
    for (const tsrebar::SteelBarGroup& group : snapshot.steelData.groups) {
        appendTopologyRefsFromGeometry(group.geometryRef, snapshot.sourceStep.sourceStepId,
                                       snapshot.topologyRefs, indexByStableId);
    }
    for (const tsrebar::SteelBar& bar : snapshot.steelData.bars) {
        appendTopologyRefsFromGeometry(bar.geometryRef, snapshot.sourceStep.sourceStepId,
                                       snapshot.topologyRefs, indexByStableId);
    }
    for (const tsrebar::SteelBarSegment& segment : snapshot.steelData.segments) {
        appendTopologyRefsFromGeometry(segment.geometryRef, snapshot.sourceStep.sourceStepId,
                                       snapshot.topologyRefs, indexByStableId);
    }

    for (tsrebar::SteelBarGroup& group : snapshot.steelData.groups) {
        rewriteBindingForRuntimeSave(group.binding, group.geometryRef, indexByStableId,
                                     "rebar/groups.json#/items/0/geometryRef/curveRefs/0");
    }
    for (tsrebar::SteelBar& bar : snapshot.steelData.bars) {
        rewriteBindingForRuntimeSave(bar.binding, bar.geometryRef, indexByStableId,
                                     "rebar/bars.json#/items/0/geometryRef/curveRefs/0");
    }
    for (tsrebar::SteelBarSegment& segment : snapshot.steelData.segments) {
        rewriteBindingForRuntimeSave(segment.binding, segment.geometryRef, indexByStableId,
                                     "rebar/segments.json#/items/0/geometryRef/curveRefs/0");
    }

    return snapshot;
}

bool MainWindow::hasPersistentDirtyState() const
{
    return m_dirtyState.projectDirty || m_dirtyState.geometryDirty ||
           m_dirtyState.rebarDirty || m_dirtyState.drawingDirty;
}

bool MainWindow::lineGroupSelectionPreflightForCommand(QString* errorMessage) const
{
    const std::optional<tsrebar::LegacySelectionRef> current =
        m_viewer->currentSelectionRef();
    if (!current.has_value()) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("失败：线配筋需要先选择边");
        }
        return false;
    }
    if (current->shapeKind != tsrebar::LegacyShapeKind::Edge) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("失败：线配筋当前只接受边选择");
        }
        return false;
    }
    return true;
}

bool MainWindow::configureLineGroupParametersForCommand()
{
    tsrebar::LineGroupParameterDialog dialog(nextLineGroupParameters(), this);
    if (dialog.exec() != QDialog::Accepted) {
        return false;
    }

    m_lineGroupParameters = dialog.parameters();
    if (m_lineGroupHandler) {
        m_lineGroupHandler->setParameters(m_lineGroupParameters);
    }
    return true;
}

tsrebar::RebarLineGroupCommandParameters MainWindow::nextLineGroupParameters()
{
    ++m_lineGroupCreateSequence;
    return lineGroupParametersForSequence(m_lineGroupCreateSequence);
}

bool MainWindow::displayCreatedLineGroup(QString* errorMessage)
{
    const tsrebar::RebarAisPresentationResult presentation =
        tsrebar::RebarAisPresentationAdapter{}.buildGroupPresentation(
            m_steelData,
            m_lineGroupParameters.groupId);
    return m_viewer->displayRebarPresentation(presentation, errorMessage);
}
