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

#include <cmath>
#include <cstdlib>
#include <iostream>

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
    acceptLineGroupDialog();
    action->trigger();
    app.processEvents();
    expect(viewer->displayedRebarShapeCount() == beforeFailure,
           "failed line group command must not refresh rebar AIS display");

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

    const int beforeSuccess = viewer->displayedRebarShapeCount();
    acceptLineGroupDialog();
    action->trigger();
    app.processEvents();

    expect(viewer->displayedRebarShapeCount() > beforeSuccess,
           "successful line group command must display created SteelBarGroup in AIS");
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
    const auto& groups = window.steelDataForInspection().groups;
    expect(!groups.empty(), "successful line group command must leave groups inspectable");
    const tsrebar::SteelBarGroup& latest = groups.back();
    expect(nearlyEqual(latest.diameter, 32.0), "edited dialog diameter must reach handler");
    expect(nearlyEqual(latest.interval, 150.0), "edited dialog interval must reach handler");
    expect(latest.barCount == 5, "edited dialog bar count must reach handler");
    expect(latest.steelLevel == "HRB500", "edited dialog steel level must reach handler");
    return 0;
}
