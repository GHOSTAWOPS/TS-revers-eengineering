#include "command/RebarLineGroupCommandHandler.h"
#include "ui/LineGroupParameterDialog.h"

#include <QApplication>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSpinBox>

#include <cmath>
#include <cstdlib>
#include <iostream>

namespace {

void expect(bool condition, const char* message)
{
    if (!condition) {
        std::cerr << message << '\n';
        std::exit(1);
    }
}

bool near(double left, double right)
{
    return std::abs(left - right) < 1.0e-9;
}

tsrebar::RebarLineGroupCommandParameters parameters()
{
    tsrebar::RebarLineGroupCommandParameters item;
    item.groupId = "dlg-line-group-001";
    item.barId = "dlg-line-bar-001";
    item.segmentId = "dlg-line-segment-001";
    item.steelDataId = "dlg-steel-data-001";
    item.distanceA = 0.25;
    item.distanceB = 1.2;
    item.legacyFlag = 0;
    item.diameter = 25.0;
    item.interval = 200.0;
    item.requestedBarCount = 3;
    item.steelLevel = "HRB400";
    item.rsdId = "P0";
    item.componentName = "pending-ui";
    item.projectSteelName = "line-group-p0";
    return item;
}

template <typename T>
T* requireChild(QWidget& parent, const char* objectName)
{
    T* child = parent.findChild<T*>(QString::fromLatin1(objectName));
    expect(child != nullptr, objectName);
    return child;
}

void testDialogExposesP0Defaults()
{
    tsrebar::LineGroupParameterDialog dialog(parameters());

    const auto current = dialog.parameters();
    expect(current.groupId == "dlg-line-group-001", "dialog must preserve generated group id");
    expect(near(current.diameter, 25.0), "dialog must expose default diameter");
    expect(near(current.interval, 200.0), "dialog must expose default interval");
    expect(current.requestedBarCount == 3, "dialog must expose default bar count");
    expect(current.steelLevel == "HRB400", "dialog must expose default steel level");
    expect(near(current.distanceA, 0.25), "dialog must expose default distanceA");
    expect(near(current.distanceB, 1.2), "dialog must expose default distanceB");

    expect(requireChild<QDoubleSpinBox>(dialog, "line_group_diameter_spin")->suffix() ==
               QStringLiteral(" mm"),
           "diameter field must show mm unit");
    expect(requireChild<QDoubleSpinBox>(dialog, "line_group_interval_spin")->suffix() ==
               QStringLiteral(" mm"),
           "interval field must show mm unit");
}

void testDialogMapsEditedControlsToParameters()
{
    tsrebar::LineGroupParameterDialog dialog(parameters());

    requireChild<QDoubleSpinBox>(dialog, "line_group_diameter_spin")->setValue(32.0);
    requireChild<QDoubleSpinBox>(dialog, "line_group_interval_spin")->setValue(150.0);
    requireChild<QSpinBox>(dialog, "line_group_bar_count_spin")->setValue(5);
    requireChild<QDoubleSpinBox>(dialog, "line_group_distance_a_spin")->setValue(0.4);
    requireChild<QDoubleSpinBox>(dialog, "line_group_distance_b_spin")->setValue(1.8);
    auto* level = requireChild<QComboBox>(dialog, "line_group_steel_level_combo");
    level->setCurrentText(QStringLiteral("HRB500"));

    const auto current = dialog.parameters();
    expect(current.groupId == "dlg-line-group-001", "edited dialog must keep generated group id");
    expect(near(current.diameter, 32.0), "edited diameter must map to parameters");
    expect(near(current.interval, 150.0), "edited interval must map to parameters");
    expect(current.requestedBarCount == 5, "edited bar count must map to parameters");
    expect(current.steelLevel == "HRB500", "edited level must map to parameters");
    expect(near(current.distanceA, 0.4), "edited distanceA must map to parameters");
    expect(near(current.distanceB, 1.8), "edited distanceB must map to parameters");
}

} // namespace

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    testDialogExposesP0Defaults();
    testDialogMapsEditedControlsToParameters();
    return 0;
}
