#include "app/MainWindow.h"

#include "command/LegacyUiCommandMap.h"
#include "geometry/occ/import/OcctStepImportService.h"
#include "presentation/occ/OccViewerWidget.h"

#include <QAction>
#include <QApplication>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QSpinBox>
#include <QTimer>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

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

    const int beforeSecondSuccess = viewer->displayedRebarShapeCount();
    acceptLineGroupDialogWithEditedParameters();
    action->trigger();
    app.processEvents();

    expect(viewer->displayedRebarShapeCount() > beforeSecondSuccess,
           "second successful line group command must add AIS display items");
    expect(viewer->lastDisplayedRebarGroupId() != firstDisplayedGroup,
           "second successful line group command must display the newly created group");
    expect(window.dirtyStateForInspection().committedTransactionCount == 2,
           "second successful line group command must commit a second dirty transaction");
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
