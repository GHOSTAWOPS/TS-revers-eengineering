#include "domain/rebar/RebarEditMoveService.h"

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

bool hasEvidence(const std::vector<tsrebar::RebarEvidenceRef>& evidence,
                 const std::string& evidenceId)
{
    for (const auto& item : evidence) {
        if (item.evidenceId == evidenceId) {
            return true;
        }
    }
    return false;
}

tsrebar::SteelData steelDataWithGroup()
{
    tsrebar::SteelBarSegment line;
    line.segmentId = "seg-line";
    line.barId = "bar-001";
    line.shapeType = tsrebar::SteelBarSegmentShape::Line;
    line.startPoint = {0.0, 1.0, 2.0};
    line.middlePoint = {5.0, 1.0, 2.0};
    line.endPoint = {10.0, 1.0, 2.0};
    line.binding.state = tsrebar::BindingState::Resolved;
    line.binding.items.push_back({"legacyObject.raw.ptr80", "geometryRef.curveStableIds[0]", "E-IDA-017"});
    line.evidence.push_back({"E-DEV-042", "domain/rebar P1 field freeze"});

    tsrebar::SteelBarSegment arc;
    arc.segmentId = "seg-arc";
    arc.barId = "bar-001";
    arc.shapeType = tsrebar::SteelBarSegmentShape::Arc;
    arc.startPoint = {1.0, 2.0, 3.0};
    arc.middlePoint = {2.0, 4.0, 3.0};
    arc.endPoint = {3.0, 2.0, 3.0};

    tsrebar::SteelBar bar;
    bar.barId = "bar-001";
    bar.groupId = "group-001";
    bar.segmentIds = {line.segmentId, arc.segmentId};
    bar.binding.state = tsrebar::BindingState::Resolved;
    bar.evidence.push_back({"E-DEV-042", "domain/rebar P1 field freeze"});

    tsrebar::SteelBarGroup group;
    group.groupId = "group-001";
    group.barIds.push_back(bar.barId);
    group.binding.state = tsrebar::BindingState::Resolved;
    group.evidence.push_back({"E-DEV-042", "domain/rebar P1 field freeze"});

    tsrebar::SteelData data;
    data.segments = {line, arc};
    data.bars.push_back(bar);
    data.groups.push_back(group);
    return data;
}

void testMoveTranslatesAllSegmentPointsAndKeepsIdentity()
{
    tsrebar::SteelData data = steelDataWithGroup();
    const auto originalBinding = data.segments.front().binding;

    tsrebar::RebarMoveRequest request;
    request.groupIds.push_back("group-001");
    request.translation = {100.0, -20.0, 7.5};
    request.evidence.push_back({"E-IDA-023", "barmove Input_Choice copyFlag=0 translate_transf path"});

    const tsrebar::RebarEditMoveService service;
    const tsrebar::RebarMoveResult result = service.moveGroups(data, request);

    expect(result.ok, "move result must be ok");
    expect(result.movedGroupCount == 1, "move must count one group");
    expect(result.movedBarCount == 1, "move must count one bar");
    expect(result.movedSegmentCount == 2, "move must count both segments");
    expect(data.groups.front().groupId == "group-001", "move must keep group id");
    expect(data.bars.front().barId == "bar-001", "move must keep bar id");
    expect(data.segments.front().segmentId == "seg-line", "move must keep segment id");
    expect(data.segments.front().binding.state == originalBinding.state,
           "move must keep segment binding state");
    expect(data.segments.front().binding.items.front().legacyPath
               == originalBinding.items.front().legacyPath,
           "move must keep binding items");

    const auto& line = data.segments[0];
    expect(near(line.startPoint.x, 100.0), "line start x mismatch");
    expect(near(line.startPoint.y, -19.0), "line start y mismatch");
    expect(near(line.startPoint.z, 9.5), "line start z mismatch");
    expect(near(line.middlePoint.x, 105.0), "line middle x mismatch");
    expect(near(line.endPoint.x, 110.0), "line end x mismatch");

    const auto& arc = data.segments[1];
    expect(near(arc.startPoint.x, 101.0), "arc start x mismatch");
    expect(near(arc.middlePoint.y, -16.0), "arc middle y mismatch");
    expect(near(arc.endPoint.z, 10.5), "arc end z mismatch");
    expect(hasEvidence(data.groups.front().evidence, "E-IDA-023"),
           "group must retain move evidence");
    expect(hasEvidence(data.bars.front().evidence, "E-IDA-023"),
           "bar must retain move evidence");
    expect(hasEvidence(data.segments.front().evidence, "E-IDA-023"),
           "segment must retain move evidence");
}

void testMoveRejectsEmptySelection()
{
    tsrebar::SteelData data = steelDataWithGroup();
    const tsrebar::RebarEditMoveService service;
    const tsrebar::RebarMoveResult result = service.moveGroups(data, {});

    expect(!result.ok, "empty move request must fail");
    expect(result.diagnostic.find("requires at least one group id") != std::string::npos,
           "empty move diagnostic mismatch");
    expect(near(data.segments.front().startPoint.x, 0.0),
           "failed move must not change segment");
}

void testMoveRejectsMissingGroupWithoutPartialMutation()
{
    tsrebar::SteelData data = steelDataWithGroup();
    tsrebar::RebarMoveRequest request;
    request.groupIds = {"missing-group"};
    request.translation = {1.0, 2.0, 3.0};

    const tsrebar::RebarEditMoveService service;
    const tsrebar::RebarMoveResult result = service.moveGroups(data, request);

    expect(!result.ok, "missing group move must fail");
    expect(result.diagnostic.find("missing group") != std::string::npos,
           "missing group diagnostic mismatch");
    expect(near(data.segments.front().startPoint.x, 0.0),
           "missing group move must not change segment");
}

void testMoveRejectsBrokenBarReferenceWithoutPartialMutation()
{
    tsrebar::SteelData data = steelDataWithGroup();
    data.groups.front().barIds.push_back("missing-bar");

    tsrebar::RebarMoveRequest request;
    request.groupIds = {"group-001"};
    request.translation = {1.0, 2.0, 3.0};

    const tsrebar::RebarEditMoveService service;
    const tsrebar::RebarMoveResult result = service.moveGroups(data, request);

    expect(!result.ok, "broken bar move must fail");
    expect(result.diagnostic.find("missing bar") != std::string::npos,
           "missing bar diagnostic mismatch");
    expect(near(data.segments.front().startPoint.x, 0.0),
           "broken bar move must not partially translate segment");
}

void testMoveRejectsBarFromAnotherGroupWithoutPartialMutation()
{
    tsrebar::SteelData data = steelDataWithGroup();
    data.bars.front().groupId = "other-group";

    tsrebar::RebarMoveRequest request;
    request.groupIds = {"group-001"};
    request.translation = {1.0, 2.0, 3.0};

    const tsrebar::RebarEditMoveService service;
    const tsrebar::RebarMoveResult result = service.moveGroups(data, request);

    expect(!result.ok, "bar group mismatch move must fail");
    expect(result.diagnostic.find("bar group mismatch") != std::string::npos,
           "bar group mismatch diagnostic mismatch");
    expect(near(data.segments.front().startPoint.x, 0.0),
           "bar group mismatch must not partially translate segment");
}

void testMoveRejectsSegmentFromAnotherBarWithoutPartialMutation()
{
    tsrebar::SteelData data = steelDataWithGroup();
    data.segments.front().barId = "other-bar";

    tsrebar::RebarMoveRequest request;
    request.groupIds = {"group-001"};
    request.translation = {1.0, 2.0, 3.0};

    const tsrebar::RebarEditMoveService service;
    const tsrebar::RebarMoveResult result = service.moveGroups(data, request);

    expect(!result.ok, "segment bar mismatch move must fail");
    expect(result.diagnostic.find("segment bar mismatch") != std::string::npos,
           "segment bar mismatch diagnostic mismatch");
    expect(near(data.segments.front().startPoint.x, 0.0),
           "segment bar mismatch must not partially translate segment");
}

} // namespace

int main()
{
    testMoveTranslatesAllSegmentPointsAndKeepsIdentity();
    testMoveRejectsEmptySelection();
    testMoveRejectsMissingGroupWithoutPartialMutation();
    testMoveRejectsBrokenBarReferenceWithoutPartialMutation();
    testMoveRejectsBarFromAnotherGroupWithoutPartialMutation();
    testMoveRejectsSegmentFromAnotherBarWithoutPartialMutation();
    return 0;
}
