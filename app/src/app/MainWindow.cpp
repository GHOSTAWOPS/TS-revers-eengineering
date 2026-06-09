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

    [[nodiscard]] tsrebar::LegacyGeometryQueryResult<tsrebar::LegacyRebarCurveSnapshot>
    normalizeSegmentCurve(
        const tsrebar::LegacyRebarCurveSnapshot& curve,
        const tsrebar::LegacySegmentCurveNormalizeRequest& request) const override
    {
        Q_UNUSED(request)
        return {true, curve, {}};
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
        if (!configureLineGroupParametersForCommand()) {
            statusBar()->showMessage(QStringLiteral("取消：线配筋参数"), 5000);
            return;
        }
    }

    const tsrebar::CommandResult result = m_commands.execute(id);
    if (id == tsrebar::CommandId::RebarLineCreate &&
        result.status == tsrebar::CommandStatus::Completed) {
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
