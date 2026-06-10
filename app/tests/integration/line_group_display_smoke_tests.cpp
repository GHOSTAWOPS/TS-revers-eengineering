#include "app/MainWindow.h"

#include "command/LegacyUiCommandMap.h"
#include "geometry/occ/import/OcctStepImportService.h"
#include "presentation/occ/OccViewerWidget.h"

#include <QAction>
#include <QApplication>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QPushButton>
#include <QSpinBox>
#include <QTemporaryDir>
#include <QTimer>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

namespace {

void expect(bool condition, const char* message)
{
    if (!condition) {
        std::cerr << message << '\n';
        std::cerr.flush();
        std::exit(1);
    }
}

bool nearlyEqual(double left, double right)
{
    return std::abs(left - right) < 1.0e-9;
}

bool containsId(const std::vector<std::string>& ids, const char* expected)
{
    return std::find(ids.begin(), ids.end(), std::string(expected)) != ids.end();
}

std::string createdParameterValue(const tsrebar::LegacyRawBlock& block,
                                  const std::string& name)
{
    for (const auto& field : block.fields) {
        if (field.name == name) {
            return field.value;
        }
    }
    return {};
}

QString lineGroupActionObjectName()
{
    const auto commands = tsrebar::legacyUiCommands();
    for (const auto& command : commands) {
        if (command.id == tsrebar::CommandId::RebarLineCreate) {
            return command.objectName;
        }
    }
    return {};
}

QString projectSaveActionObjectName()
{
    const auto commands = tsrebar::legacyUiCommands();
    for (const auto& command : commands) {
        if (command.id == tsrebar::CommandId::ProjectSave) {
            return command.objectName;
        }
    }
    return {};
}

void expectCleanDirtyState(const MainWindow& window, const char* message)
{
    const MainWindow::DirtyState& dirty = window.dirtyStateForInspection();
    expect(!dirty.projectDirty, message);
    expect(!dirty.geometryDirty, message);
    expect(!dirty.rebarDirty, message);
    expect(!dirty.drawingDirty, message);
    expect(!dirty.selectionDirty, message);
    expect(!dirty.viewDirty, message);
    expect(dirty.committedTransactionCount == 0, message);
    expect(dirty.lastDirtyCommand.isEmpty(), message);
}

void expectLineGroupDirtyState(const MainWindow& window,
                               int expectedTransactionCount,
                               const char* message)
{
    const MainWindow::DirtyState& dirty = window.dirtyStateForInspection();
    expect(dirty.projectDirty && dirty.rebarDirty && dirty.drawingDirty, message);
    expect(!dirty.geometryDirty && !dirty.selectionDirty && !dirty.viewDirty, message);
    expect(dirty.committedTransactionCount == expectedTransactionCount, message);
    expect(dirty.lastDirtyCommand == QStringLiteral("Rebar.Create.LineGroup"), message);
    expect(dirty.legacyDirtyEvidenceId == QStringLiteral("E-IDA-049"), message);
    expect(dirty.unresolvedDirtyParityGap == QStringLiteral("GAP-REB-C-002"), message);
}

void expectSaveClearedLineGroupDirtyState(const MainWindow& window,
                                          int expectedTransactionCount,
                                          const char* message)
{
    const MainWindow::DirtyState& dirty = window.dirtyStateForInspection();
    expect(!dirty.projectDirty && !dirty.rebarDirty && !dirty.drawingDirty, message);
    expect(!dirty.geometryDirty && !dirty.selectionDirty && !dirty.viewDirty, message);
    expect(dirty.committedTransactionCount == expectedTransactionCount, message);
    expect(dirty.lastDirtyCommand == QStringLiteral("Rebar.Create.LineGroup"), message);
    expect(dirty.legacyDirtyEvidenceId == QStringLiteral("E-IDA-049"), message);
    expect(dirty.unresolvedDirtyParityGap == QStringLiteral("GAP-REB-C-002"), message);
}

template <typename T>
T* requireChild(QWidget& parent, const char* objectName)
{
    T* child = parent.findChild<T*>(QString::fromLatin1(objectName));
    expect(child != nullptr, objectName);
    return child;
}

void acceptLineGroupDialog()
{
    QTimer::singleShot(0, []() {
        QWidget* modal = QApplication::activeModalWidget();
        expect(modal != nullptr, "line group action must open parameter dialog");
        expect(modal->objectName() == QStringLiteral("line_group_parameter_dialog"),
               "active modal must be the line group parameter dialog");
        auto* buttons = requireChild<QDialogButtonBox>(*modal, "line_group_button_box");
        buttons->button(QDialogButtonBox::Ok)->click();
    });
}

void failIfLineGroupDialogOpens(bool& dialogOpened)
{
    QTimer::singleShot(0, [&dialogOpened]() {
        QWidget* modal = QApplication::activeModalWidget();
        if (modal == nullptr) {
            return;
        }
        if (modal->objectName() != QStringLiteral("line_group_parameter_dialog")) {
            return;
        }
        dialogOpened = true;
        auto* buttons = requireChild<QDialogButtonBox>(*modal, "line_group_button_box");
        buttons->button(QDialogButtonBox::Cancel)->click();
    });
}

void rejectLineGroupDialog()
{
    QTimer::singleShot(0, []() {
        QWidget* modal = QApplication::activeModalWidget();
        expect(modal != nullptr, "line group cancel path must open parameter dialog");
        expect(modal->objectName() == QStringLiteral("line_group_parameter_dialog"),
               "active modal must be the line group parameter dialog");
        auto* buttons = requireChild<QDialogButtonBox>(*modal, "line_group_button_box");
        buttons->button(QDialogButtonBox::Cancel)->click();
    });
}

void acceptLineGroupDialogWithEditedParameters()
{
    QTimer::singleShot(0, []() {
        QWidget* modal = QApplication::activeModalWidget();
        expect(modal != nullptr, "line group edit path must open parameter dialog");
        expect(modal->objectName() == QStringLiteral("line_group_parameter_dialog"),
               "active modal must be the line group parameter dialog");

        requireChild<QDoubleSpinBox>(*modal, "line_group_diameter_spin")->setValue(32.0);
        requireChild<QDoubleSpinBox>(*modal, "line_group_interval_spin")->setValue(150.0);
        requireChild<QSpinBox>(*modal, "line_group_bar_count_spin")->setValue(5);
        requireChild<QDoubleSpinBox>(*modal, "line_group_distance_a_spin")->setValue(0.4);
        requireChild<QDoubleSpinBox>(*modal, "line_group_distance_b_spin")->setValue(1.8);
        requireChild<QComboBox>(*modal, "line_group_steel_level_combo")
            ->setCurrentText(QStringLiteral("HRB500"));

        auto* buttons = requireChild<QDialogButtonBox>(*modal, "line_group_button_box");
        buttons->button(QDialogButtonBox::Ok)->click();
    });
}

} // namespace

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    if (argc != 2) {
        std::cerr << "usage: line_group_display_smoke_tests <123.stp>\n";
        return 2;
    }

    MainWindow window;
    window.setAttribute(Qt::WA_DontShowOnScreen, true);
    window.resize(1280, 820);
    window.show();
    app.processEvents();

    auto* viewer = window.findChild<tsrebar::OccViewerWidget*>();
    expect(viewer != nullptr, "MainWindow must expose one OccViewerWidget child");

    const tsrebar::OcctImportedDocument document =
        tsrebar::OcctStepImportService().importDocument(QString::fromLocal8Bit(argv[1]));
    expect(document.isSuccess(), document.summary.error.c_str());

    QString error;
    expect(viewer->displayDocument(document, &error), error.toUtf8().constData());

    const QString actionObjectName = lineGroupActionObjectName();
    expect(!actionObjectName.isEmpty(), "RebarLineCreate action metadata must exist");
    QAction* action = window.findChild<QAction*>(actionObjectName);
    expect(action != nullptr, "MainWindow must render RebarLineCreate action");
    const QString saveActionObjectName = projectSaveActionObjectName();
    expect(!saveActionObjectName.isEmpty(), "ProjectSave action metadata must exist");
    QAction* saveAction = window.findChild<QAction*>(saveActionObjectName);
    expect(saveAction != nullptr, "MainWindow must render ProjectSave action");

    const int beforeFailure = viewer->displayedRebarShapeCount();
    const std::size_t groupsBeforeNoSelection = window.steelDataForInspection().groups.size();
    expectCleanDirtyState(window, "initial app dirty state must be clean");
    bool noSelectionDialogOpened = false;
    failIfLineGroupDialogOpens(noSelectionDialogOpened);
    action->trigger();
    app.processEvents();
    expect(!noSelectionDialogOpened,
           "line group command must not open parameter dialog without selection");
    expect(viewer->displayedRebarShapeCount() == beforeFailure,
           "failed line group command must not refresh rebar AIS display");
    expect(window.steelDataForInspection().groups.size() == groupsBeforeNoSelection,
           "no-selection preflight must not mutate SteelData");
    expectCleanDirtyState(window, "no-selection preflight must not dirty the app");

    const auto faceRefs = viewer->selectionIndex().refs(tsrebar::LegacyShapeKind::Face);
    expect(!faceRefs.isEmpty(), "test STEP must have face selection refs");
    expect(viewer->selectByStableId(QString::fromStdString(faceRefs.front().stableId), &error),
           error.toUtf8().constData());

    const int beforeWrongType = viewer->displayedRebarShapeCount();
    const std::size_t groupsBeforeWrongType = window.steelDataForInspection().groups.size();
    bool wrongTypeDialogOpened = false;
    failIfLineGroupDialogOpens(wrongTypeDialogOpened);
    action->trigger();
    app.processEvents();
    expect(!wrongTypeDialogOpened,
           "line group command must not open parameter dialog for non-edge selection");
    expect(viewer->displayedRebarShapeCount() == beforeWrongType,
           "wrong-type preflight must not refresh rebar AIS display");
    expect(window.steelDataForInspection().groups.size() == groupsBeforeWrongType,
           "wrong-type preflight must not mutate SteelData");
    expectCleanDirtyState(window, "wrong-type preflight must not dirty the app");

    const auto edgeRefs = viewer->selectionIndex().refs(tsrebar::LegacyShapeKind::Edge);
    expect(!edgeRefs.isEmpty(), "test STEP must have edge selection refs");
    expect(viewer->selectByStableId(QString::fromStdString(edgeRefs.front().stableId), &error),
           error.toUtf8().constData());

    const int beforeCancel = viewer->displayedRebarShapeCount();
    const std::size_t groupsBeforeCancel = window.steelDataForInspection().groups.size();
    rejectLineGroupDialog();
    action->trigger();
    app.processEvents();
    expect(viewer->displayedRebarShapeCount() == beforeCancel,
           "cancelled parameter dialog must not refresh rebar AIS display");
    expect(window.steelDataForInspection().groups.size() == groupsBeforeCancel,
           "cancelled parameter dialog must not mutate SteelData");
    expectCleanDirtyState(window, "cancelled parameter dialog must not dirty the app");

    const int beforeSuccess = viewer->displayedRebarShapeCount();
    acceptLineGroupDialog();
    action->trigger();
    app.processEvents();

    expect(viewer->displayedRebarShapeCount() > beforeSuccess,
           "successful line group command must display created SteelBarGroup in AIS");
    const MainWindow::DirtyState& dirtyAfterSuccess = window.dirtyStateForInspection();
    expect(dirtyAfterSuccess.projectDirty && dirtyAfterSuccess.rebarDirty &&
               dirtyAfterSuccess.drawingDirty,
           "successful line group command must mark project/rebar/drawing dirty");
    expect(!dirtyAfterSuccess.geometryDirty && !dirtyAfterSuccess.selectionDirty &&
               !dirtyAfterSuccess.viewDirty,
           "line group dirty P0 must not mark unrelated dirty flags");
    expect(dirtyAfterSuccess.committedTransactionCount == 1,
           "first successful line group command must commit one dirty transaction");
    expect(dirtyAfterSuccess.lastDirtyCommand == QStringLiteral("Rebar.Create.LineGroup"),
           "dirty transaction must keep the line group command key");
    expect(dirtyAfterSuccess.legacyDirtyEvidenceId == QStringLiteral("E-IDA-049"),
           "dirty transaction must trace the old dirty call evidence");
    expect(dirtyAfterSuccess.unresolvedDirtyParityGap == QStringLiteral("GAP-REB-C-002"),
           "dirty transaction must keep old dirty parity unresolved");
    const QString firstDisplayedGroup = viewer->lastDisplayedRebarGroupId();
    expect(!firstDisplayedGroup.isEmpty(),
           "successful line group command must record displayed group id");

    QTemporaryDir saveTemp;
    expect(saveTemp.isValid(), "temporary save dir must be valid");
    const QString packagePath = QDir(saveTemp.path()).filePath("line_group_save.tsrebar");
    window.setProjectPackagePathForInspection(packagePath);
    saveAction->trigger();
    app.processEvents();
    const auto& saveResult = window.lastSaveResultForInspection();
    expect(saveResult.has_value(), "Project.Save must record a save result");
    expect(saveResult->ok, "Project.Save must succeed for current line group SteelData");
    expect(saveResult->dirtyBefore, "Project.Save must see dirty before saving");
    expect(!saveResult->dirtyAfter, "Project.Save success must clear runtime dirty");
    expect(QFileInfo::exists(QDir(packagePath).filePath("manifest.json")),
           "Project.Save must write the runtime package manifest");
    expectSaveClearedLineGroupDirtyState(
        window,
        1,
        "Project.Save success must clear project/rebar/drawing dirty and keep transaction evidence");

    const tsrebar::TsRebarProjectRuntime runtime;
    const auto open = runtime.open(packagePath);
    expect(open.ok, "saved line group package must open through runtime");
    expect(open.finalState == tsrebar::ProjectDocumentState::OpenedWarning,
           "line group package open must preserve pending legacy warning state");
    expect(open.bindingDecision != QStringLiteral("blocked"),
           "line group package open binding decision must not be blocked");
    expect(open.snapshot.steelData.groups.size() == 1,
           "line group package open must restore one SteelBarGroup");
    expect(open.snapshot.steelData.bars.size() == 1,
           "line group package open must restore one SteelBar");
    expect(open.snapshot.steelData.segments.size() == 1,
           "line group package open must restore one SteelBarSegment");
    expect(!open.snapshot.steelData.groups.front().binding.items.empty(),
           "line group package open must restore group binding items");
    expect(open.snapshot.steelData.groups.front().binding.items.front().geometryPath.rfind(
               "geometry/topology_refs.json#/topologyRefs/",
               0) == 0,
           "line group package open must restore runtime topology binding path");
    expect(containsId(open.snapshot.evidenceIds, "E-DEV-108"),
           "line group package open must restore save-clear evidence id");
    expect(containsId(open.snapshot.evidenceIds, "E-IDA-049"),
           "line group package open must restore dirty/write IDA evidence id");
    expect(containsId(open.snapshot.evidenceIds, "GAP-REB-C-002"),
           "line group package open must restore unresolved dirty parity gap id");

    const int beforeSecondSuccess = viewer->displayedRebarShapeCount();
    acceptLineGroupDialogWithEditedParameters();
    action->trigger();
    app.processEvents();

    expect(viewer->displayedRebarShapeCount() > beforeSecondSuccess,
           "second successful line group command must add AIS display items");
    expect(viewer->lastDisplayedRebarGroupId() != firstDisplayedGroup,
           "second successful line group command must display the newly created group");
    expectLineGroupDirtyState(
        window,
        2,
        "second successful line group command must dirty project/rebar/drawing again");

    const QString blockedPackagePath =
        QDir(saveTemp.path()).filePath("blocked_save_target.tsrebar");
    QFile blocker(blockedPackagePath);
    expect(blocker.open(QIODevice::WriteOnly | QIODevice::Text),
           "blocking file for failed save must open");
    blocker.write("keep this file");
    blocker.close();
    window.setProjectPackagePathForInspection(blockedPackagePath);
    saveAction->trigger();
    app.processEvents();
    const auto& failedSaveResult = window.lastSaveResultForInspection();
    expect(failedSaveResult.has_value(), "failed Project.Save must record a save result");
    expect(!failedSaveResult->ok, "Project.Save install failure must fail");
    expect(failedSaveResult->dirtyBefore,
           "Project.Save install failure must see dirty before saving");
    expect(failedSaveResult->dirtyAfter,
           "Project.Save install failure must keep runtime dirty");
    expect(QFileInfo(blockedPackagePath).isFile(),
           "Project.Save install failure must leave the blocking file in place");
    expectLineGroupDirtyState(
        window,
        2,
        "Project.Save failure must keep project/rebar/drawing dirty and transaction evidence");

    const auto& groups = window.steelDataForInspection().groups;
    expect(!groups.empty(), "successful line group command must leave groups inspectable");
    const tsrebar::SteelBarGroup& latest = groups.back();
    const auto& segments = window.steelDataForInspection().segments;
    expect(!segments.empty(), "successful line group command must leave segments inspectable");
    const tsrebar::SteelBarSegment& latestSegment = segments.back();
    expect(nearlyEqual(latest.diameter, 32.0), "edited dialog diameter must reach handler");
    expect(nearlyEqual(latest.interval, 150.0), "edited dialog interval must reach handler");
    expect(latest.barCount == 5, "edited dialog bar count must reach handler");
    expect(latest.steelLevel == "HRB500", "edited dialog steel level must reach handler");
    const std::string splineSampleCount =
        createdParameterValue(latest.createdFromParameters,
                              "sub_1405D5670.api_curve_spline.effectiveSampleCount");
    expect(!splineSampleCount.empty(),
           "line group raw evidence must record effective spline sample count");
    const int expectedSplineSampleCount =
        std::max(5, static_cast<int>(latestSegment.length * 50.0));
    expect(std::stoi(splineSampleCount) == expectedSplineSampleCount,
           "viewer reader spline trace must match the adapter requested sample count");
    expect(createdParameterValue(latest.createdFromParameters,
                                 "sub_14059B980.groupMinimumDistanceTrimLoop.observed") == "true",
           "viewer reader raw evidence must record TODO-084 group min-distance gap");
    expect(createdParameterValue(latest.createdFromParameters,
                                 "sub_1405BD0C0.entitySlot72Write") == "observed-deferred",
           "viewer reader raw evidence must record TODO-084 backup/write edge gap");
    expect(createdParameterValue(latest.createdFromParameters,
                                 "sub_1405E49D0.dirtyWrite") ==
               "deferred-application-state",
           "viewer reader raw evidence must record TODO-084 dirty-write gap");
    return 0;
}
