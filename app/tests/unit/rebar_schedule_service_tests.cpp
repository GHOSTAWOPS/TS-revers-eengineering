#include "domain/rebar/RebarScheduleService.h"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace {

void expect(bool condition, const char* message)
{
    if (!condition) {
        std::cerr << message << '\n';
        std::exit(1);
    }
}

bool near(double lhs, double rhs)
{
    return std::abs(lhs - rhs) < 1.0e-9;
}

tsrebar::SteelBarSegment lineSegment(std::string id,
                                     std::string barId,
                                     int sequenceNo,
                                     double length)
{
    tsrebar::SteelBarSegment segment;
    segment.segmentId = std::move(id);
    segment.barId = std::move(barId);
    segment.sequenceNo = sequenceNo;
    segment.shapeType = tsrebar::SteelBarSegmentShape::Line;
    segment.length = length;
    segment.startPoint = {0.0, 0.0, 0.0};
    segment.endPoint = {length, 0.0, 0.0};
    return segment;
}

tsrebar::SteelBar bar(std::string id,
                      std::string groupId,
                      std::vector<std::string> segmentIds,
                      double length)
{
    tsrebar::SteelBar item;
    item.barId = std::move(id);
    item.groupId = std::move(groupId);
    item.sequenceNo = 1;
    item.diameter = 25.0;
    item.steelLevel = "HRB400";
    item.length = length;
    item.segmentIds = std::move(segmentIds);
    return item;
}

tsrebar::SteelBarGroup group(std::string id,
                             std::string rsdId,
                             double diameter,
                             std::string steelLevel,
                             int barCount,
                             std::vector<std::string> barIds)
{
    tsrebar::SteelBarGroup item;
    item.groupId = std::move(id);
    item.rsdId = std::move(rsdId);
    item.componentName = "pier";
    item.steelWay = "LA";
    item.diameter = diameter;
    item.barCount = barCount;
    item.steelLevel = std::move(steelLevel);
    item.layer = "inside";
    item.profile = "default";
    item.use = "main";
    item.segmentCount = 2;
    item.barIds = std::move(barIds);
    return item;
}

void addSingleMass(tsrebar::SteelBarGroup& item, double value)
{
    item.legacyRaw.fields.push_back(
        {"singleMass", std::to_string(value), "E-IDA-TODO-030"});
}

tsrebar::SteelData steelDataWithTwoGroups()
{
    tsrebar::SteelData data;
    data.segments.push_back(lineSegment("seg-001", "bar-001", 1, 10.0));
    data.segments.push_back(lineSegment("seg-002", "bar-001", 2, 5.0));
    data.segments.push_back(lineSegment("seg-003", "bar-002", 1, 20.0));
    data.bars.push_back(bar("bar-001", "group-001", {"seg-001", "seg-002"}, 15.0));
    data.bars.push_back(bar("bar-002", "group-002", {"seg-003"}, 20.0));

    tsrebar::SteelBarGroup first =
        group("group-001", "Y12", 25.0, "HRB400", 3, {"bar-001"});
    tsrebar::SteelBarGroup second =
        group("group-002", "Z8", 25.0, "HRB400", 2, {"bar-002"});
    data.groups.push_back(first);
    data.groups.push_back(second);
    return data;
}

void testScheduleRowsShareOldStbRowFieldsWithGroups()
{
    const tsrebar::SteelData data = steelDataWithTwoGroups();

    const tsrebar::RebarSchedule schedule =
        tsrebar::RebarScheduleService{}.buildSchedule(data);

    expect(schedule.scheduleRows.size() == 2, "schedule row count mismatch");
    const tsrebar::RebarScheduleRow& first = schedule.scheduleRows.at(0);
    expect(first.rsdId == "Y12", "StbRow.rsdID must come from group.rsdId");
    expect(first.componentName == "pier", "StbRow.ComponentName mismatch");
    expect(first.steelWay == "LA", "StbRow.SteelWay mismatch");
    expect(near(first.diameter, 25.0), "StbRow.diameter mismatch");
    expect(near(first.length, 15.0), "StbRow.length must be one bar length");
    expect(first.segmentCount == 2, "StbRow.segNum mismatch");
    expect(first.sameGroupCount == 1, "P0 sameGrpNum must stay conservative");
    expect(first.barNumberSum == 3, "StbRow.stbNumSum mismatch");
    expect(near(first.lengthSum, 45.0), "StbRow.lenSum mismatch");
    expect(first.segments.size() == 2, "StbRow segment refs mismatch");
}

void testMaterialRowsGroupByDiameterAndLevel()
{
    tsrebar::SteelData data = steelDataWithTwoGroups();
    addSingleMass(data.groups.at(0), 3.85);
    addSingleMass(data.groups.at(1), 3.85);

    const tsrebar::RebarSchedule schedule =
        tsrebar::RebarScheduleService{}.buildSchedule(data);

    expect(schedule.materialRows.size() == 1, "same diameter/level material rows must merge");
    const tsrebar::RebarMaterialRow& row = schedule.materialRows.front();
    expect(near(row.diameter, 25.0), "MatRow.diameter mismatch");
    expect(row.steelLevel == "HRB400", "MatRow.stbLevel mismatch");
    expect(near(row.lengthSum, 85.0), "MatRow.lenSum mismatch");
    expect(row.countSum == 5, "MatRow.countSum mismatch");
    expect(near(row.singleMass, 3.85), "MatRow.singleMass mismatch");
    expect(near(row.massSum, 327.25), "MatRow.massSum mismatch");
    expect(near(schedule.totalMass, 0.327), "MaterialTable.Mass mismatch");
    expect(!schedule.massFormulaDeferred, "mass must not be deferred with explicit singleMass");
}

void testMaterialMassIsDeferredWithoutConfirmedSingleMass()
{
    const tsrebar::SteelData data = steelDataWithTwoGroups();

    const tsrebar::RebarSchedule schedule =
        tsrebar::RebarScheduleService{}.buildSchedule(data);

    expect(schedule.materialRows.size() == 1, "material row count mismatch");
    expect(schedule.materialRows.front().massDeferred, "mass must be deferred");
    expect(schedule.massFormulaDeferred, "schedule must expose deferred mass formula");
    expect(near(schedule.totalMass, 0.0), "deferred total mass must be zero");
    expect(!schedule.diagnostics.empty(), "deferred mass must emit diagnostic");
}

void testMixedSingleMassInOneMaterialRowDefersWholeMass()
{
    tsrebar::SteelData data = steelDataWithTwoGroups();
    addSingleMass(data.groups.at(0), 3.85);

    const tsrebar::RebarSchedule schedule =
        tsrebar::RebarScheduleService{}.buildSchedule(data);

    expect(schedule.materialRows.size() == 1, "material row count mismatch");
    expect(schedule.materialRows.front().massDeferred,
           "mixed confirmed/unconfirmed singleMass must defer row mass");
    expect(near(schedule.materialRows.front().massSum, 0.0),
           "deferred mixed material massSum must be zero");
    expect(schedule.massFormulaDeferred, "mixed singleMass must defer schedule mass");
    expect(near(schedule.totalMass, 0.0), "mixed singleMass total mass must be zero");
}

void testConflictingSingleMassInOneMaterialRowDefersWholeMass()
{
    tsrebar::SteelData data = steelDataWithTwoGroups();
    addSingleMass(data.groups.at(0), 3.85);
    addSingleMass(data.groups.at(1), 4.20);

    const tsrebar::RebarSchedule schedule =
        tsrebar::RebarScheduleService{}.buildSchedule(data);

    expect(schedule.materialRows.size() == 1, "material row count mismatch");
    expect(schedule.materialRows.front().massDeferred,
           "conflicting singleMass must defer row mass");
    expect(near(schedule.materialRows.front().massSum, 0.0),
           "conflicting material massSum must be zero");
    expect(schedule.massFormulaDeferred, "conflicting singleMass must defer schedule mass");
}

void testDeferredMassClearsAllMaterialRows()
{
    tsrebar::SteelData data = steelDataWithTwoGroups();
    data.groups.at(1).diameter = 32.0;
    data.bars.at(1).diameter = 32.0;
    addSingleMass(data.groups.at(0), 3.85);

    const tsrebar::RebarSchedule schedule =
        tsrebar::RebarScheduleService{}.buildSchedule(data);

    expect(schedule.materialRows.size() == 2, "different diameter material rows must split");
    expect(schedule.massFormulaDeferred, "any deferred row must defer table Mass");
    expect(near(schedule.totalMass, 0.0), "deferred table Mass must be zero");
    for (const tsrebar::RebarMaterialRow& row : schedule.materialRows) {
        expect(row.massDeferred, "global deferred table must mark every MatRow deferred");
        expect(near(row.massSum, 0.0), "global deferred table must clear every MatRow.massSum");
    }
}

void testBrokenReferencesEmitDiagnostics()
{
    tsrebar::SteelData data = steelDataWithTwoGroups();
    data.groups.front().barIds = {"missing-bar"};

    const tsrebar::RebarSchedule schedule =
        tsrebar::RebarScheduleService{}.buildSchedule(data);

    expect(schedule.scheduleRows.size() == 1, "valid groups should still produce rows");
    expect(!schedule.diagnostics.empty(), "missing bar must emit diagnostic");
    expect(schedule.diagnostics.front().code == "RS001_MISSING_REFERENCE",
           "missing bar diagnostic code mismatch");
}

} // namespace

int main()
{
    testScheduleRowsShareOldStbRowFieldsWithGroups();
    testMaterialRowsGroupByDiameterAndLevel();
    testMaterialMassIsDeferredWithoutConfirmedSingleMass();
    testMixedSingleMassInOneMaterialRowDefersWholeMass();
    testConflictingSingleMassInOneMaterialRowDefersWholeMass();
    testDeferredMassClearsAllMaterialRows();
    testBrokenReferencesEmitDiagnostics();
    return 0;
}
