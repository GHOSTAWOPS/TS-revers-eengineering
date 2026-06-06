#include "domain/rebar/RebarEditMoveService.h"

#include <algorithm>
#include <unordered_set>
#include <vector>

namespace tsrebar {

namespace {

bool sameId(const std::string& requestedId, const std::string& id, const std::string& alias)
{
    return requestedId == id || (!alias.empty() && requestedId == alias);
}

void translatePoint(DomainPoint3d& point, const DomainPoint3d& vector)
{
    point.x += vector.x;
    point.y += vector.y;
    point.z += vector.z;
}

bool hasEvidence(const std::vector<RebarEvidenceRef>& evidence, const std::string& evidenceId)
{
    return std::any_of(evidence.begin(), evidence.end(), [&](const RebarEvidenceRef& item) {
        return item.evidenceId == evidenceId;
    });
}

void appendMissingEvidence(std::vector<RebarEvidenceRef>& target,
                           const std::vector<RebarEvidenceRef>& evidence)
{
    for (const auto& item : evidence) {
        if (!item.evidenceId.empty() && !hasEvidence(target, item.evidenceId)) {
            target.push_back(item);
        }
    }
}

SteelBarGroup* findGroup(SteelData& steelData, const std::string& groupId)
{
    for (auto& group : steelData.groups) {
        if (sameId(groupId, group.groupId, group.id)) {
            return &group;
        }
    }
    return nullptr;
}

SteelBar* findBar(SteelData& steelData, const std::string& barId)
{
    for (auto& bar : steelData.bars) {
        if (sameId(barId, bar.barId, bar.id)) {
            return &bar;
        }
    }
    return nullptr;
}

SteelBarSegment* findSegment(SteelData& steelData, const std::string& segmentId)
{
    for (auto& segment : steelData.segments) {
        if (sameId(segmentId, segment.segmentId, segment.id)) {
            return &segment;
        }
    }
    return nullptr;
}

std::string stableId(const SteelBar& bar)
{
    return bar.barId.empty() ? bar.id : bar.barId;
}

std::string stableId(const SteelBarSegment& segment)
{
    return segment.segmentId.empty() ? segment.id : segment.segmentId;
}

} // namespace

RebarMoveResult RebarEditMoveService::moveGroups(SteelData& steelData,
                                                 const RebarMoveRequest& request) const
{
    RebarMoveResult result;
    if (request.groupIds.empty()) {
        result.diagnostic = "Rebar.Edit.Move requires at least one group id";
        return result;
    }

    std::vector<SteelBarGroup*> groupsToMove;
    std::vector<SteelBar*> barsToMove;
    std::vector<SteelBarSegment*> segmentsToMove;
    std::unordered_set<std::string> stagedBarIds;
    std::unordered_set<std::string> stagedSegmentIds;

    for (const auto& groupId : request.groupIds) {
        SteelBarGroup* group = findGroup(steelData, groupId);
        if (!group) {
            result.diagnostic = "Rebar.Edit.Move missing group: " + groupId;
            return result;
        }
        if (!group->active) {
            result.diagnostic = "Rebar.Edit.Move inactive group: " + groupId;
            return result;
        }
        groupsToMove.push_back(group);

        for (const auto& barId : group->barIds) {
            SteelBar* bar = findBar(steelData, barId);
            if (!bar) {
                result.diagnostic = "Rebar.Edit.Move missing bar: " + barId;
                return result;
            }
            if (!sameId(group->groupId.empty() ? group->id : group->groupId, bar->groupId, {})) {
                result.diagnostic = "Rebar.Edit.Move bar group mismatch: " + barId;
                return result;
            }
            if (!bar->active) {
                result.diagnostic = "Rebar.Edit.Move inactive bar: " + barId;
                return result;
            }

            if (stagedBarIds.insert(stableId(*bar)).second) {
                barsToMove.push_back(bar);
            }

            for (const auto& segmentId : bar->segmentIds) {
                SteelBarSegment* segment = findSegment(steelData, segmentId);
                if (!segment) {
                    result.diagnostic = "Rebar.Edit.Move missing segment: " + segmentId;
                    return result;
                }
                if (!sameId(stableId(*bar), segment->barId, {})) {
                    result.diagnostic = "Rebar.Edit.Move segment bar mismatch: " + segmentId;
                    return result;
                }

                if (stagedSegmentIds.insert(stableId(*segment)).second) {
                    segmentsToMove.push_back(segment);
                }
            }
        }
    }

    for (auto* group : groupsToMove) {
        appendMissingEvidence(group->evidence, request.evidence);
    }
    for (auto* bar : barsToMove) {
        appendMissingEvidence(bar->evidence, request.evidence);
    }
    for (auto* segment : segmentsToMove) {
        translatePoint(segment->startPoint, request.translation);
        translatePoint(segment->endPoint, request.translation);
        translatePoint(segment->middlePoint, request.translation);
        appendMissingEvidence(segment->evidence, request.evidence);
    }

    result.movedGroupCount = static_cast<int>(groupsToMove.size());
    result.movedBarCount = static_cast<int>(barsToMove.size());
    result.movedSegmentCount = static_cast<int>(segmentsToMove.size());
    result.ok = true;
    return result;
}

} // namespace tsrebar
