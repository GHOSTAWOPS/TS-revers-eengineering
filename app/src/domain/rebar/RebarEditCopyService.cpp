#include "domain/rebar/RebarEditCopyService.h"

#include <algorithm>
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

namespace tsrebar {

namespace {

bool sameId(const std::string& requestedId, const std::string& id, const std::string& alias)
{
    return requestedId == id || (!alias.empty() && requestedId == alias);
}

std::string stableId(const SteelBarGroup& group)
{
    return group.groupId.empty() ? group.id : group.groupId;
}

std::string stableId(const SteelBar& bar)
{
    return bar.barId.empty() ? bar.id : bar.barId;
}

std::string stableId(const SteelBarSegment& segment)
{
    return segment.segmentId.empty() ? segment.id : segment.segmentId;
}

void translatePoint(DomainPoint3d& point, const DomainPoint3d& vector, int multiplier)
{
    point.x += vector.x * multiplier;
    point.y += vector.y * multiplier;
    point.z += vector.z * multiplier;
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

void markCopiedBinding(BindingAnchor& binding)
{
    binding.state = BindingState::UnresolvedGeometry;
    binding.items.clear();
}

void clearGeometryReference(GeometryReference& geometryRef)
{
    geometryRef.faceStableIds.clear();
    geometryRef.edgeStableIds.clear();
    geometryRef.curveStableIds.clear();
}

void resetCopiedGroupUnconfirmedFields(SteelBarGroup& group)
{
    group.rsdId.clear();
    group.displayNumber.clear();
    group.actualNumber.clear();
    group.sourceFaceId.clear();
    group.sourceLineId.clear();
    group.sourceCurveIds.clear();
    group.referenceIds.clear();
    group.createdFromParameters = {};
    group.legacyRaw = {};
    group.unresolvedLegacyFields.push_back({
        "copy.numbering",
        "scopy P0 clears copied numbering until old numbering rules are confirmed",
        "GAP-REB-E-002",
    });
    group.unresolvedLegacyFields.push_back({
        "copy.legacyRaw",
        "scopy P0 does not reuse source legacy raw/write-order fields",
        "GAP-REB-E-002",
    });
}

void resetCopiedBarUnconfirmedFields(SteelBar& bar)
{
    bar.sequenceNo = 0;
    bar.displayNumber.clear();
    bar.legacyRaw = {};
    bar.unresolvedLegacyFields.push_back({
        "copy.barNumbering",
        "scopy P0 clears copied bar numbering until old numbering rules are confirmed",
        "GAP-REB-E-002",
    });
    bar.unresolvedLegacyFields.push_back({
        "copy.legacyRaw",
        "scopy P0 does not reuse source steelbar legacy raw fields",
        "GAP-REB-E-002",
    });
}

void resetCopiedSegmentUnconfirmedFields(SteelBarSegment& segment)
{
    segment.sequenceNo = 0;
    segment.legacyRaw = {};
    segment.unresolvedLegacyFields.push_back({
        "copy.segmentSequence",
        "scopy P0 clears copied segment sequence until old segment numbering rules are confirmed",
        "GAP-REB-E-002",
    });
    segment.unresolvedLegacyFields.push_back({
        "copy.legacyRaw",
        "scopy P0 does not reuse source segment legacy raw fields",
        "GAP-REB-E-002",
    });
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

const RebarCopyIdSet* findIdSet(const RebarCopyRequest& request,
                                int copyIndex,
                                const std::string& sourceGroupId)
{
    for (const auto& idSet : request.idSets) {
        if (idSet.copyIndex == copyIndex && idSet.sourceGroupId == sourceGroupId) {
            return &idSet;
        }
    }
    return nullptr;
}

std::optional<std::string> mappedId(const std::vector<RebarCopyIdMap>& maps,
                                    const std::string& sourceId)
{
    for (const auto& map : maps) {
        if (map.sourceId == sourceId) {
            return map.newId;
        }
    }
    return std::nullopt;
}

bool hasExistingGroupId(const SteelData& steelData, const std::string& groupId)
{
    return std::any_of(steelData.groups.begin(), steelData.groups.end(), [&](const auto& group) {
        return sameId(groupId, group.groupId, group.id);
    });
}

bool hasExistingBarId(const SteelData& steelData, const std::string& barId)
{
    return std::any_of(steelData.bars.begin(), steelData.bars.end(), [&](const auto& bar) {
        return sameId(barId, bar.barId, bar.id);
    });
}

bool hasExistingSegmentId(const SteelData& steelData, const std::string& segmentId)
{
    return std::any_of(steelData.segments.begin(), steelData.segments.end(), [&](const auto& segment) {
        return sameId(segmentId, segment.segmentId, segment.id);
    });
}

struct SourceSegmentRef
{
    SteelBarSegment* segment = nullptr;
};

struct SourceBarRef
{
    SteelBar* bar = nullptr;
    std::vector<SourceSegmentRef> segments;
};

struct SourceGroupRef
{
    SteelBarGroup* group = nullptr;
    std::vector<SourceBarRef> bars;
};

struct PlannedCopyGroup
{
    SteelBarGroup group;
    std::vector<SteelBar> bars;
    std::vector<SteelBarSegment> segments;
};

struct Prevalidation
{
    bool ok = false;
    std::string diagnostic;
    std::vector<SourceGroupRef> sources;
};

Prevalidation collectSources(SteelData& steelData, const RebarCopyRequest& request)
{
    Prevalidation result;
    if (request.groupIds.empty()) {
        result.diagnostic = "Rebar.Edit.Copy requires at least one group id";
        return result;
    }
    if (request.copyCount < 1 || request.copyCount > 1000) {
        result.diagnostic = "Rebar.Edit.Copy copyCount must be 1..1000";
        return result;
    }

    for (const auto& groupId : request.groupIds) {
        SteelBarGroup* group = findGroup(steelData, groupId);
        if (!group) {
            result.diagnostic = "Rebar.Edit.Copy missing group: " + groupId;
            return result;
        }
        if (!group->active) {
            result.diagnostic = "Rebar.Edit.Copy inactive group: " + groupId;
            return result;
        }

        SourceGroupRef sourceGroup;
        sourceGroup.group = group;
        const std::string sourceGroupId = stableId(*group);

        for (const auto& barId : group->barIds) {
            SteelBar* bar = findBar(steelData, barId);
            if (!bar) {
                result.diagnostic = "Rebar.Edit.Copy missing bar: " + barId;
                return result;
            }
            if (!sameId(sourceGroupId, bar->groupId, {})) {
                result.diagnostic = "Rebar.Edit.Copy bar group mismatch: " + barId;
                return result;
            }
            if (!bar->active) {
                result.diagnostic = "Rebar.Edit.Copy inactive bar: " + barId;
                return result;
            }

            SourceBarRef sourceBar;
            sourceBar.bar = bar;
            const std::string sourceBarId = stableId(*bar);

            for (const auto& segmentId : bar->segmentIds) {
                SteelBarSegment* segment = findSegment(steelData, segmentId);
                if (!segment) {
                    result.diagnostic = "Rebar.Edit.Copy missing segment: " + segmentId;
                    return result;
                }
                if (!sameId(sourceBarId, segment->barId, {})) {
                    result.diagnostic = "Rebar.Edit.Copy segment bar mismatch: " + segmentId;
                    return result;
                }
                sourceBar.segments.push_back({segment});
            }
            sourceGroup.bars.push_back(sourceBar);
        }

        result.sources.push_back(sourceGroup);
    }

    result.ok = true;
    return result;
}

bool stageGroupCopy(const SteelData& steelData,
                    const RebarCopyRequest& request,
                    const SourceGroupRef& source,
                    const RebarCopyIdSet& idSet,
                    int copyIndex,
                    std::unordered_set<std::string>& newGroupIds,
                    std::unordered_set<std::string>& newBarIds,
                    std::unordered_set<std::string>& newSegmentIds,
                    PlannedCopyGroup& planned,
                    std::string& diagnostic)
{
    if (idSet.newGroupId.empty()) {
        diagnostic = "Rebar.Edit.Copy missing new group id";
        return false;
    }
    if (hasExistingGroupId(steelData, idSet.newGroupId)
        || !newGroupIds.insert(idSet.newGroupId).second) {
        diagnostic = "Rebar.Edit.Copy duplicate new group id: " + idSet.newGroupId;
        return false;
    }

    planned.group = *source.group;
    planned.group.id = idSet.newGroupId;
    planned.group.groupId = idSet.newGroupId;
    planned.group.barIds.clear();
    resetCopiedGroupUnconfirmedFields(planned.group);
    markCopiedBinding(planned.group.binding);
    clearGeometryReference(planned.group.geometryRef);
    appendMissingEvidence(planned.group.evidence, request.evidence);

    for (const auto& sourceBar : source.bars) {
        const std::string sourceBarId = stableId(*sourceBar.bar);
        const auto newBarId = mappedId(idSet.barIds, sourceBarId);
        if (!newBarId || newBarId->empty()) {
            diagnostic = "Rebar.Edit.Copy missing new bar id for: " + sourceBarId;
            return false;
        }
        if (hasExistingBarId(steelData, *newBarId) || !newBarIds.insert(*newBarId).second) {
            diagnostic = "Rebar.Edit.Copy duplicate new bar id: " + *newBarId;
            return false;
        }

        SteelBar copiedBar = *sourceBar.bar;
        copiedBar.id = *newBarId;
        copiedBar.barId = *newBarId;
        copiedBar.groupId = idSet.newGroupId;
        copiedBar.segmentIds.clear();
        resetCopiedBarUnconfirmedFields(copiedBar);
        markCopiedBinding(copiedBar.binding);
        clearGeometryReference(copiedBar.geometryRef);
        appendMissingEvidence(copiedBar.evidence, request.evidence);

        for (const auto& sourceSegment : sourceBar.segments) {
            const std::string sourceSegmentId = stableId(*sourceSegment.segment);
            const auto newSegmentId = mappedId(idSet.segmentIds, sourceSegmentId);
            if (!newSegmentId || newSegmentId->empty()) {
                diagnostic = "Rebar.Edit.Copy missing new segment id for: " + sourceSegmentId;
                return false;
            }
            if (hasExistingSegmentId(steelData, *newSegmentId)
                || !newSegmentIds.insert(*newSegmentId).second) {
                diagnostic = "Rebar.Edit.Copy duplicate new segment id: " + *newSegmentId;
                return false;
            }

            SteelBarSegment copiedSegment = *sourceSegment.segment;
            copiedSegment.id = *newSegmentId;
            copiedSegment.segmentId = *newSegmentId;
            copiedSegment.barId = *newBarId;
            translatePoint(copiedSegment.startPoint, request.translation, copyIndex);
            translatePoint(copiedSegment.middlePoint, request.translation, copyIndex);
            translatePoint(copiedSegment.endPoint, request.translation, copyIndex);
            resetCopiedSegmentUnconfirmedFields(copiedSegment);
            markCopiedBinding(copiedSegment.binding);
            clearGeometryReference(copiedSegment.geometryRef);
            appendMissingEvidence(copiedSegment.evidence, request.evidence);

            copiedBar.segmentIds.push_back(*newSegmentId);
            planned.segments.push_back(copiedSegment);
        }

        planned.group.barIds.push_back(*newBarId);
        planned.bars.push_back(copiedBar);
    }

    return true;
}

} // namespace

RebarCopyResult RebarEditCopyService::copyGroups(SteelData& steelData,
                                                 const RebarCopyRequest& request) const
{
    RebarCopyResult result;
    Prevalidation prevalidation = collectSources(steelData, request);
    if (!prevalidation.ok) {
        result.diagnostic = prevalidation.diagnostic;
        return result;
    }

    std::vector<PlannedCopyGroup> plannedCopies;
    std::unordered_set<std::string> newGroupIds;
    std::unordered_set<std::string> newBarIds;
    std::unordered_set<std::string> newSegmentIds;

    for (int copyIndex = 1; copyIndex <= request.copyCount; ++copyIndex) {
        for (const auto& source : prevalidation.sources) {
            const std::string sourceGroupId = stableId(*source.group);
            const RebarCopyIdSet* idSet = findIdSet(request, copyIndex, sourceGroupId);
            if (!idSet) {
                result.diagnostic = "Rebar.Edit.Copy missing id set for copy "
                    + std::to_string(copyIndex) + " group: " + sourceGroupId;
                return result;
            }

            PlannedCopyGroup planned;
            if (!stageGroupCopy(steelData,
                                request,
                                source,
                                *idSet,
                                copyIndex,
                                newGroupIds,
                                newBarIds,
                                newSegmentIds,
                                planned,
                                result.diagnostic)) {
                return result;
            }
            plannedCopies.push_back(planned);
        }
    }

    for (const auto& planned : plannedCopies) {
        steelData.groups.push_back(planned.group);
        result.newGroupIds.push_back(planned.group.groupId);
        ++result.copiedGroupCount;

        for (const auto& bar : planned.bars) {
            steelData.bars.push_back(bar);
            ++result.copiedBarCount;
        }

        for (const auto& segment : planned.segments) {
            steelData.segments.push_back(segment);
            ++result.copiedSegmentCount;
        }
    }

    result.ok = true;
    return result;
}

} // namespace tsrebar
