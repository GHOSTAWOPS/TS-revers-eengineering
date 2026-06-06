#include "domain/rebar/SteelData.h"

#include <cmath>
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

void testDefaultDomainState()
{
    const tsrebar::SteelData steelData;
    const tsrebar::SteelBarGroup group;
    const tsrebar::SteelBar bar;
    const tsrebar::SteelBarSegment segment;

    expect(steelData.steelDataId.empty(), "SteelData id must default empty");
    expect(steelData.binding.state == tsrebar::BindingState::PendingLegacyEvidence,
           "SteelData binding must default to pending legacy evidence");
    expect(group.active, "SteelBarGroup must default active");
    expect(group.visible, "SteelBarGroup must default visible");
    expect(group.binding.state == tsrebar::BindingState::PendingLegacyEvidence,
           "SteelBarGroup binding must default to pending legacy evidence");
    expect(bar.active, "SteelBar must default active");
    expect(bar.visible, "SteelBar must default visible");
    expect(segment.shapeType == tsrebar::SteelBarSegmentShape::Unknown,
           "SteelBarSegment shape must default unknown");
    expect(segment.detailShapeTypeCode().empty(),
           "unknown segment shape must not emit a Detail shape code");
}

void testGroupBarSegmentReferencesAndDetailFields()
{
    tsrebar::SteelData steelData;
    steelData.steelDataId = "steel-data-hrb400";
    steelData.level = "HRB400";
    steelData.gradeName = "HRB400E";
    steelData.diameterSet = {25.0, 28.0};
    steelData.evidence.push_back({"E-IDA-018", "Detail StbGroup field chain"});

    tsrebar::SteelBarSegment segment;
    segment.segmentId = "seg-001";
    segment.barId = "bar-001";
    segment.sequenceNo = 1;
    segment.shapeType = tsrebar::SteelBarSegmentShape::Line;
    segment.startPoint = {0.0, 0.0, 0.0};
    segment.endPoint = {1000.0, 0.0, 0.0};
    segment.middlePoint = {500.0, 0.0, 0.0};
    segment.offset = {0.0, 0.0, 25.0};
    segment.length = 1000.0;
    segment.legacyRaw.writeOrder = {
        "write_int(+68)",
        "write_string(+72)",
        "write_ptr(+80)",
        "write_ptr(+96)",
        "write_ptr(+88)",
        "write_int(+64)",
    };
    segment.legacyRaw.fields.push_back({"string72DisplayName", "segment display name", "E-IDA-017"});
    segment.unresolvedLegacyFields.push_back({"ptr80", "unknown saved pointer role", "GAP-IDA-007"});
    segment.geometryRef.curveStableIds.push_back("selection-v1:edge:17");
    segment.binding.state = tsrebar::BindingState::Resolved;
    segment.binding.items.push_back({"legacyObject.raw.string72DisplayName",
                                     "geometryRef.curveStableIds[0]",
                                     "E-IDA-017"});

    tsrebar::SteelBar bar;
    bar.barId = "bar-001";
    bar.groupId = "group-001";
    bar.sequenceNo = 1;
    bar.displayNumber = "1";
    bar.diameter = 25.0;
    bar.steelLevel = "HRB400";
    bar.length = segment.length;
    bar.shapeType = "lineStb";
    bar.segmentIds.push_back(segment.segmentId);

    tsrebar::SteelBarGroup group;
    group.groupId = "group-001";
    group.rsdId = "Y12";
    group.displayNumber = "12'";
    group.actualNumber = "12";
    group.componentName = "pier";
    group.projectSteelName = "main bar";
    group.createCommand = "Rebar.Create.LineGroup";
    group.steelDataId = steelData.steelDataId;
    group.diameter = 25.0;
    group.secondaryDiameter = 28.0;
    group.interval = 200.0;
    group.barCount = 1;
    group.segmentCount = 1;
    group.steelLevel = "HRB400";
    group.layer = "inside";
    group.profile = "default-profile";
    group.use = "main";
    group.rangeLess180 = true;
    group.steelWay = "LA";
    group.rebarType = "lineStb";
    group.offsetInOut = 0.0;
    group.barIds.push_back(bar.barId);
    group.sourceCurveIds.push_back("selection-v1:edge:17");
    group.evidence.push_back({"E-IDA-018", "StbGroup write chain"});

    steelData.segments.push_back(segment);
    steelData.bars.push_back(bar);
    steelData.groups.push_back(group);

    expect(steelData.groups.front().barIds.front() == steelData.bars.front().barId,
           "group must reference bar id");
    expect(steelData.bars.front().segmentIds.front() == steelData.segments.front().segmentId,
           "bar must reference segment id");
    expect(steelData.segments.front().detailShapeTypeCode() == "L",
           "line segment must map to Detail shapeType L");
    expect(steelData.groups.front().rsdId == "Y12",
           "rsdId must preserve non-integer Detail forms");
    expect(near(steelData.groups.front().diameter, 25.0), "group diameter mismatch");
    expect(near(steelData.groups.front().interval, 200.0), "group interval mismatch");
    expect(steelData.groups.front().barCount == 1, "group barCount mismatch");
    expect(steelData.groups.front().segmentCount == 1, "group segmentCount mismatch");
    expect(steelData.groups.front().steelWay == "LA", "group SteelWay mismatch");
    expect(steelData.groups.front().rebarType == "lineStb", "group rebarType mismatch");
    expect(!steelData.segments.front().unresolvedLegacyFields.empty(),
           "low-confidence legacy fields must be carried explicitly");
    expect(steelData.segments.front().binding.state == tsrebar::BindingState::Resolved,
           "segment binding state mismatch");
}

} // namespace

int main()
{
    testDefaultDomainState();
    testGroupBarSegmentReferencesAndDetailFields();
    return 0;
}
