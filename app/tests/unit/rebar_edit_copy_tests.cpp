#include "domain/rebar/RebarEditCopyService.h"

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

tsrebar::SteelBarGroup* findGroup(tsrebar::SteelData& data, const std::string& groupId)
{
    for (auto& group : data.groups) {
        if (group.groupId == groupId) {
            return &group;
        }
    }
    return nullptr;
}

tsrebar::SteelBar* findBar(tsrebar::SteelData& data, const std::string& barId)
{
    for (auto& bar : data.bars) {
        if (bar.barId == barId) {
            return &bar;
        }
    }
    return nullptr;
}

tsrebar::SteelBarSegment* findSegment(tsrebar::SteelData& data, const std::string& segmentId)
{
    for (auto& segment : data.segments) {
        if (segment.segmentId == segmentId) {
            return &segment;
        }
    }
    return nullptr;
}

tsrebar::SteelData steelDataWithGroup()
{
    tsrebar::SteelBarSegment line;
    line.id = "seg-line";
    line.segmentId = "seg-line";
    line.barId = "bar-001";
    line.shapeType = tsrebar::SteelBarSegmentShape::Line;
    line.startPoint = {0.0, 1.0, 2.0};
    line.middlePoint = {5.0, 1.0, 2.0};
    line.endPoint = {10.0, 1.0, 2.0};
    line.geometryRef.curveStableIds.push_back("curve-source-line");
    line.binding.state = tsrebar::BindingState::Resolved;
    line.binding.items.push_back({"legacyObject.raw.ptr80", "geometryRef.curveStableIds[0]", "E-IDA-017"});
    line.evidence.push_back({"E-DEV-042", "domain/rebar P1 field freeze"});

    tsrebar::SteelBarSegment arc;
    arc.id = "seg-arc";
    arc.segmentId = "seg-arc";
    arc.barId = "bar-001";
    arc.shapeType = tsrebar::SteelBarSegmentShape::Arc;
    arc.startPoint = {1.0, 2.0, 3.0};
    arc.middlePoint = {2.0, 4.0, 3.0};
    arc.endPoint = {3.0, 2.0, 3.0};
    arc.geometryRef.curveStableIds.push_back("curve-source-arc");
    arc.binding.state = tsrebar::BindingState::Resolved;

    tsrebar::SteelBar bar;
    bar.id = "bar-001";
    bar.barId = "bar-001";
    bar.groupId = "group-001";
    bar.segmentIds = {line.segmentId, arc.segmentId};
    bar.geometryRef.curveStableIds.push_back("bar-source-curve");
    bar.binding.state = tsrebar::BindingState::Resolved;
    bar.evidence.push_back({"E-DEV-042", "domain/rebar P1 field freeze"});

    tsrebar::SteelBarGroup group;
    group.id = "group-001";
    group.groupId = "group-001";
    group.rsdId = "RSD-SOURCE";
    group.displayNumber = "G1";
    group.actualNumber = "G1-ACT";
    group.barIds.push_back(bar.barId);
    group.sourceFaceId = "face-source";
    group.sourceLineId = "line-source";
    group.sourceCurveIds.push_back("curve-source");
    group.referenceIds.push_back("ref-source");
    group.createdFromParameters.legacyType = "source-created-params";
    group.legacyRaw.legacyType = "steelbargroup";
    group.geometryRef.curveStableIds.push_back("group-source-curve");
    group.binding.state = tsrebar::BindingState::Resolved;
    group.evidence.push_back({"E-DEV-042", "domain/rebar P1 field freeze"});

    tsrebar::SteelData data;
    data.segments = {line, arc};
    data.bars.push_back(bar);
    data.groups.push_back(group);
    return data;
}

tsrebar::RebarCopyIdSet idSetFor(int copyIndex,
                                 const std::string& groupId,
                                 const std::string& barId,
                                 const std::string& lineId,
                                 const std::string& arcId)
{
    tsrebar::RebarCopyIdSet ids;
    ids.copyIndex = copyIndex;
    ids.sourceGroupId = "group-001";
    ids.newGroupId = groupId;
    ids.barIds.push_back({"bar-001", barId});
    ids.segmentIds.push_back({"seg-line", lineId});
    ids.segmentIds.push_back({"seg-arc", arcId});
    return ids;
}

void testCopyCreatesTranslatedGroupsAndKeepsSourceUnchanged()
{
    tsrebar::SteelData data = steelDataWithGroup();

    tsrebar::RebarCopyRequest request;
    request.groupIds.push_back("group-001");
    request.translation = {10.0, -2.0, 3.0};
    request.copyCount = 2;
    request.idSets.push_back(idSetFor(1, "group-copy-1", "bar-copy-1", "seg-line-copy-1", "seg-arc-copy-1"));
    request.idSets.push_back(idSetFor(2, "group-copy-2", "bar-copy-2", "seg-line-copy-2", "seg-arc-copy-2"));
    request.evidence.push_back({"E-IDA-024", "scopy Input_Choice copyFlag=1 clone then transform path"});

    const tsrebar::RebarEditCopyService service;
    const tsrebar::RebarCopyResult result = service.copyGroups(data, request);

    expect(result.ok, "copy result must be ok");
    expect(result.copiedGroupCount == 2, "copy must count two groups");
    expect(result.copiedBarCount == 2, "copy must count two bars");
    expect(result.copiedSegmentCount == 4, "copy must count four segments");
    expect(result.newGroupIds.size() == 2, "copy must return new group ids");
    expect(data.groups.size() == 3, "copy must append groups");
    expect(data.bars.size() == 3, "copy must append bars");
    expect(data.segments.size() == 6, "copy must append segments");

    const auto* sourceLine = findSegment(data, "seg-line");
    expect(sourceLine, "source line must remain");
    expect(near(sourceLine->startPoint.x, 0.0), "source line start x must not change");
    expect(sourceLine->binding.state == tsrebar::BindingState::Resolved,
           "source binding must remain resolved");
    expect(!sourceLine->geometryRef.curveStableIds.empty(),
           "source geometry ref must remain");

    const auto* copyGroup1 = findGroup(data, "group-copy-1");
    const auto* copyBar1 = findBar(data, "bar-copy-1");
    const auto* copyLine1 = findSegment(data, "seg-line-copy-1");
    const auto* copyLine2 = findSegment(data, "seg-line-copy-2");
    expect(copyGroup1 && copyBar1 && copyLine1 && copyLine2, "copied objects must exist");
    expect(copyGroup1->barIds.size() == 1 && copyGroup1->barIds.front() == "bar-copy-1",
           "copied group must point to copied bar");
    expect(copyBar1->groupId == "group-copy-1", "copied bar must point to copied group");
    expect(copyBar1->segmentIds.size() == 2
               && copyBar1->segmentIds.front() == "seg-line-copy-1",
           "copied bar must point to copied segments");
    expect(copyLine1->barId == "bar-copy-1", "copied segment must point to copied bar");

    expect(near(copyLine1->startPoint.x, 10.0), "copy 1 line start x mismatch");
    expect(near(copyLine1->startPoint.y, -1.0), "copy 1 line start y mismatch");
    expect(near(copyLine1->endPoint.z, 5.0), "copy 1 line end z mismatch");
    expect(near(copyLine2->startPoint.x, 20.0), "copy 2 line start x mismatch");
    expect(near(copyLine2->startPoint.y, -3.0), "copy 2 line start y mismatch");
    expect(near(copyLine2->endPoint.z, 8.0), "copy 2 line end z mismatch");

    expect(copyGroup1->binding.state == tsrebar::BindingState::UnresolvedGeometry,
           "copied group binding must require geometry rebinding");
    expect(copyBar1->binding.state == tsrebar::BindingState::UnresolvedGeometry,
           "copied bar binding must require geometry rebinding");
    expect(copyLine1->binding.state == tsrebar::BindingState::UnresolvedGeometry,
           "copied segment binding must require geometry rebinding");
    expect(copyLine1->geometryRef.curveStableIds.empty(),
           "copied segment must not alias source geometry ref");
    expect(copyGroup1->rsdId.empty(), "copied group must clear source rsd id");
    expect(copyGroup1->displayNumber.empty(), "copied group must clear display number");
    expect(copyGroup1->actualNumber.empty(), "copied group must clear actual number");
    expect(copyGroup1->sourceFaceId.empty(), "copied group must clear source face");
    expect(copyGroup1->sourceLineId.empty(), "copied group must clear source line");
    expect(copyGroup1->sourceCurveIds.empty(), "copied group must clear source curves");
    expect(copyGroup1->referenceIds.empty(), "copied group must clear reference ids");
    expect(copyGroup1->createdFromParameters.legacyType.empty(),
           "copied group must clear source parameters raw block");
    expect(copyGroup1->legacyRaw.legacyType.empty(), "copied group must clear legacy raw");
    expect(!copyGroup1->unresolvedLegacyFields.empty(),
           "copied group must record unresolved numbering/raw fields");
    expect(copyBar1->sequenceNo == 0, "copied bar must clear source sequence");
    expect(copyBar1->displayNumber.empty(), "copied bar must clear source display number");
    expect(copyBar1->legacyRaw.legacyType.empty(), "copied bar must clear legacy raw");
    expect(!copyBar1->unresolvedLegacyFields.empty(),
           "copied bar must record unresolved numbering/raw fields");
    expect(copyLine1->sequenceNo == 0, "copied segment must clear source sequence");
    expect(copyLine1->legacyRaw.legacyType.empty(), "copied segment must clear legacy raw");
    expect(!copyLine1->unresolvedLegacyFields.empty(),
           "copied segment must record unresolved numbering/raw fields");
    expect(hasEvidence(copyGroup1->evidence, "E-IDA-024"),
           "copied group must include copy evidence");
    expect(hasEvidence(copyLine1->evidence, "E-IDA-024"),
           "copied segment must include copy evidence");
}

void testCopyRejectsMissingIdMappingWithoutPartialMutation()
{
    tsrebar::SteelData data = steelDataWithGroup();
    tsrebar::RebarCopyRequest request;
    request.groupIds.push_back("group-001");
    request.translation = {1.0, 0.0, 0.0};
    request.copyCount = 1;

    const tsrebar::RebarEditCopyService service;
    const tsrebar::RebarCopyResult result = service.copyGroups(data, request);

    expect(!result.ok, "copy without id mapping must fail");
    expect(result.diagnostic.find("missing id set") != std::string::npos,
           "missing id mapping diagnostic mismatch");
    expect(data.groups.size() == 1, "failed copy must not append groups");
    expect(data.bars.size() == 1, "failed copy must not append bars");
    expect(data.segments.size() == 2, "failed copy must not append segments");
}

void testCopyRejectsDuplicateNewIdsWithoutPartialMutation()
{
    tsrebar::SteelData data = steelDataWithGroup();
    tsrebar::RebarCopyRequest request;
    request.groupIds.push_back("group-001");
    request.translation = {1.0, 0.0, 0.0};
    request.copyCount = 1;
    request.idSets.push_back(idSetFor(1, "group-001", "bar-copy", "seg-line-copy", "seg-arc-copy"));

    const tsrebar::RebarEditCopyService service;
    const tsrebar::RebarCopyResult result = service.copyGroups(data, request);

    expect(!result.ok, "copy with duplicate group id must fail");
    expect(result.diagnostic.find("duplicate new group id") != std::string::npos,
           "duplicate id diagnostic mismatch");
    expect(data.groups.size() == 1, "duplicate id failure must not append groups");
}

void testCopyRejectsMissingNewBarIdWithoutPartialMutation()
{
    tsrebar::SteelData data = steelDataWithGroup();
    tsrebar::RebarCopyRequest request;
    request.groupIds.push_back("group-001");
    request.translation = {1.0, 0.0, 0.0};
    request.copyCount = 1;

    tsrebar::RebarCopyIdSet ids;
    ids.copyIndex = 1;
    ids.sourceGroupId = "group-001";
    ids.newGroupId = "group-copy";
    ids.segmentIds.push_back({"seg-line", "seg-line-copy"});
    ids.segmentIds.push_back({"seg-arc", "seg-arc-copy"});
    request.idSets.push_back(ids);

    const tsrebar::RebarEditCopyService service;
    const tsrebar::RebarCopyResult result = service.copyGroups(data, request);

    expect(!result.ok, "copy with missing new bar id must fail");
    expect(result.diagnostic.find("missing new bar id") != std::string::npos,
           "missing new bar id diagnostic mismatch");
    expect(data.groups.size() == 1, "missing bar id failure must not append groups");
    expect(data.bars.size() == 1, "missing bar id failure must not append bars");
    expect(data.segments.size() == 2, "missing bar id failure must not append segments");
}

void testCopyRejectsMissingNewSegmentIdWithoutPartialMutation()
{
    tsrebar::SteelData data = steelDataWithGroup();
    tsrebar::RebarCopyRequest request;
    request.groupIds.push_back("group-001");
    request.translation = {1.0, 0.0, 0.0};
    request.copyCount = 1;

    tsrebar::RebarCopyIdSet ids;
    ids.copyIndex = 1;
    ids.sourceGroupId = "group-001";
    ids.newGroupId = "group-copy";
    ids.barIds.push_back({"bar-001", "bar-copy"});
    ids.segmentIds.push_back({"seg-line", "seg-line-copy"});
    request.idSets.push_back(ids);

    const tsrebar::RebarEditCopyService service;
    const tsrebar::RebarCopyResult result = service.copyGroups(data, request);

    expect(!result.ok, "copy with missing new segment id must fail");
    expect(result.diagnostic.find("missing new segment id") != std::string::npos,
           "missing new segment id diagnostic mismatch");
    expect(data.groups.size() == 1, "missing segment id failure must not append groups");
    expect(data.bars.size() == 1, "missing segment id failure must not append bars");
    expect(data.segments.size() == 2, "missing segment id failure must not append segments");
}

void testCopyRejectsDuplicateNewBarIdWithoutPartialMutation()
{
    tsrebar::SteelData data = steelDataWithGroup();
    tsrebar::RebarCopyRequest request;
    request.groupIds.push_back("group-001");
    request.translation = {1.0, 0.0, 0.0};
    request.copyCount = 2;
    request.idSets.push_back(idSetFor(1, "group-copy-1", "bar-copy", "seg-line-copy-1", "seg-arc-copy-1"));
    request.idSets.push_back(idSetFor(2, "group-copy-2", "bar-copy", "seg-line-copy-2", "seg-arc-copy-2"));

    const tsrebar::RebarEditCopyService service;
    const tsrebar::RebarCopyResult result = service.copyGroups(data, request);

    expect(!result.ok, "copy with duplicate new bar id must fail");
    expect(result.diagnostic.find("duplicate new bar id") != std::string::npos,
           "duplicate new bar id diagnostic mismatch");
    expect(data.groups.size() == 1, "duplicate bar id failure must not append groups");
    expect(data.bars.size() == 1, "duplicate bar id failure must not append bars");
    expect(data.segments.size() == 2, "duplicate bar id failure must not append segments");
}

void testCopyRejectsDuplicateNewSegmentIdWithoutPartialMutation()
{
    tsrebar::SteelData data = steelDataWithGroup();
    tsrebar::RebarCopyRequest request;
    request.groupIds.push_back("group-001");
    request.translation = {1.0, 0.0, 0.0};
    request.copyCount = 2;
    request.idSets.push_back(idSetFor(1, "group-copy-1", "bar-copy-1", "seg-line-copy", "seg-arc-copy-1"));
    request.idSets.push_back(idSetFor(2, "group-copy-2", "bar-copy-2", "seg-line-copy", "seg-arc-copy-2"));

    const tsrebar::RebarEditCopyService service;
    const tsrebar::RebarCopyResult result = service.copyGroups(data, request);

    expect(!result.ok, "copy with duplicate new segment id must fail");
    expect(result.diagnostic.find("duplicate new segment id") != std::string::npos,
           "duplicate new segment id diagnostic mismatch");
    expect(data.groups.size() == 1, "duplicate segment id failure must not append groups");
    expect(data.bars.size() == 1, "duplicate segment id failure must not append bars");
    expect(data.segments.size() == 2, "duplicate segment id failure must not append segments");
}

void testCopyRejectsBrokenReferencesWithoutPartialMutation()
{
    tsrebar::SteelData data = steelDataWithGroup();
    data.segments.front().barId = "other-bar";

    tsrebar::RebarCopyRequest request;
    request.groupIds.push_back("group-001");
    request.translation = {1.0, 0.0, 0.0};
    request.copyCount = 1;
    request.idSets.push_back(idSetFor(1, "group-copy", "bar-copy", "seg-line-copy", "seg-arc-copy"));

    const tsrebar::RebarEditCopyService service;
    const tsrebar::RebarCopyResult result = service.copyGroups(data, request);

    expect(!result.ok, "copy with broken segment ref must fail");
    expect(result.diagnostic.find("segment bar mismatch") != std::string::npos,
           "broken segment diagnostic mismatch");
    expect(data.groups.size() == 1, "broken ref failure must not append groups");
    expect(data.bars.size() == 1, "broken ref failure must not append bars");
    expect(data.segments.size() == 2, "broken ref failure must not append segments");
}

void testCopyRejectsInvalidCopyCount()
{
    tsrebar::SteelData data = steelDataWithGroup();
    tsrebar::RebarCopyRequest request;
    request.groupIds.push_back("group-001");
    request.copyCount = 0;

    const tsrebar::RebarEditCopyService service;
    const tsrebar::RebarCopyResult result = service.copyGroups(data, request);

    expect(!result.ok, "zero copy count must fail");
    expect(result.diagnostic.find("copyCount must be 1..1000") != std::string::npos,
           "copy count diagnostic mismatch");
}

} // namespace

int main()
{
    testCopyCreatesTranslatedGroupsAndKeepsSourceUnchanged();
    testCopyRejectsMissingIdMappingWithoutPartialMutation();
    testCopyRejectsDuplicateNewIdsWithoutPartialMutation();
    testCopyRejectsMissingNewBarIdWithoutPartialMutation();
    testCopyRejectsMissingNewSegmentIdWithoutPartialMutation();
    testCopyRejectsDuplicateNewBarIdWithoutPartialMutation();
    testCopyRejectsDuplicateNewSegmentIdWithoutPartialMutation();
    testCopyRejectsBrokenReferencesWithoutPartialMutation();
    testCopyRejectsInvalidCopyCount();
    return 0;
}
