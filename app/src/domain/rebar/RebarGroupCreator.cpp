#include "domain/rebar/RebarGroupCreator.h"

#include <algorithm>
#include <sstream>
#include <utility>

namespace tsrebar {
namespace {

constexpr double kMinimumCreationDistance = 0.002;
constexpr double kMinimumSplitSegmentLength = 0.01;
constexpr double kDefaultArcSecondaryDistance = 0.8;
constexpr int kMinimumSplineSamples = 5;
constexpr int kMinimumPublicCreateObjACount = 3;
constexpr int kMinimumPublicCreateEntityListCount = 1;

std::string numberText(double value)
{
    std::ostringstream stream;
    stream << value;
    return stream.str();
}

std::string boolText(bool value)
{
    return value ? "true" : "false";
}

DomainPoint3d toDomainPoint(const LegacyPoint3d& point)
{
    return {point.x, point.y, point.z};
}

LegacyRawField rawField(std::string name, std::string value, std::string evidenceId)
{
    return {std::move(name), std::move(value), std::move(evidenceId)};
}

RebarEvidenceRef evidenceRef(std::string evidenceId, std::string note)
{
    return {std::move(evidenceId), std::move(note)};
}

RebarEvidenceRef evidenceRef(const std::string& note)
{
    return evidenceRef("E-IDA-022", note);
}

UnresolvedLegacyField unresolvedField(std::string name,
                                      std::string reason,
                                      std::string gapId = "GAP-IDA-007")
{
    return {std::move(name), std::move(reason), std::move(gapId)};
}

LegacyRawBlock creationParameters(const RebarGroupCreationRequest& request,
                                  const char* legacyCommand,
                                  double normalizedDistanceB,
                                  char effectiveFlag,
                                  bool normalizedCurve)
{
    LegacyRawBlock block;
    block.legacyType = legacyCommand;
    block.writeOrder = {
        "sub_1404D10C0.objB.present",
        "sub_1404D10C0.objA.present",
        "sub_1404D10C0.objA.sub_1405F25F0.count",
        "sub_1404D10C0.objA.sub_1405F25F0.minimum",
        "sub_1404D10C0.entityList.count",
        "sub_1404D10C0.entityList.minimum",
        "sub_1404D10C0.distanceA",
        "sub_1404D10C0.distanceB",
        "sub_1404D10C0.flag",
    };
    block.fields.push_back(rawField("minimumCreationDistance",
                                    numberText(kMinimumCreationDistance),
                                    "E-IDA-022"));
    block.fields.push_back(rawField("splitMinimumLength",
                                    numberText(kMinimumSplitSegmentLength),
                                    "E-IDA-022"));
    block.fields.push_back(rawField("sub_1404D10C0.objB.present",
                                    boolText(request.publicCreateGate.objBResolved),
                                    "E-IDA-047"));
    block.fields.push_back(rawField("sub_1404D10C0.objA.present",
                                    boolText(request.publicCreateGate.objAResolved),
                                    "E-IDA-047"));
    block.fields.push_back(rawField(
        "sub_1404D10C0.objA.sub_1405F25F0.count",
        numberText(request.publicCreateGate.objASub1405F25F0Count),
        "E-IDA-047"));
    block.fields.push_back(rawField(
        "sub_1404D10C0.objA.sub_1405F25F0.minimum",
        numberText(kMinimumPublicCreateObjACount),
        "E-IDA-047"));
    block.fields.push_back(rawField("sub_1404D10C0.entityList.count",
                                    numberText(request.publicCreateGate.entityListCount),
                                    "E-IDA-047"));
    block.fields.push_back(rawField("sub_1404D10C0.entityList.minimum",
                                    numberText(kMinimumPublicCreateEntityListCount),
                                    "E-IDA-047"));
    block.fields.push_back(rawField("distanceA", numberText(request.distanceA), "E-IDA-022"));
    block.fields.push_back(rawField("distanceB", numberText(normalizedDistanceB), "E-IDA-022"));
    block.fields.push_back(rawField("legacyFlag",
                                    numberText(static_cast<int>(effectiveFlag)),
                                    "E-IDA-022"));
    block.fields.push_back(rawField("segmentCurveNormalizerP0",
                                    normalizedCurve ? "applied" : "not-applied",
                                    "E-IDA-022"));
    if (std::string(legacyCommand) == "sgroupbarline") {
        block.fields.push_back(rawField("sgroupbarline.selectionCount", "1", "E-IDA-045"));
        block.fields.push_back(rawField("sgroupbarline.selectionGate.structureCandidate",
                                        "sub_1405C6820",
                                        "E-IDA-045"));
        block.fields.push_back(rawField("sgroupbarline.selectionGate.payloadCandidate",
                                        "sub_1405DA020",
                                        "E-IDA-045"));
        block.fields.push_back(rawField("sgroupbarline.minimumInternalItemCount",
                                        "2",
                                        "E-IDA-045"));
        block.fields.push_back(rawField("sgroupbarline.oddIndexedEntityExtraction",
                                        "child-index-1-3-5-...",
                                        "E-IDA-045"));
        block.fields.push_back(rawField("sgroupbarline.endpointDistanceCandidateCount",
                                        "4",
                                        "E-IDA-045"));
        block.fields.push_back(rawField("sgroupbarline.initialMinimumDistanceCandidate",
                                        "10.0",
                                        "E-IDA-045"));
        block.fields.push_back(rawField(
            "sgroupbarline.publicCreateCall",
            "sub_1404D10C0(entityList,objA,objB,minDistance,selectedEndpointDistance,flag)",
            "E-IDA-045"));
    }
    if (std::string(legacyCommand) == "sgroupbararc") {
        block.fields.push_back(rawField("arcSecondaryDistance",
                                        numberText(kDefaultArcSecondaryDistance),
                                        "E-IDA-022"));
        block.fields.push_back(rawField("legacyFlagForcedByCommand", "1", "E-IDA-022"));
    }
    return block;
}

RebarGroupCreationResult reject(const QString& diagnostic)
{
    RebarGroupCreationResult result;
    result.diagnostic = diagnostic;
    return result;
}

RebarGroupCreationResult validateAndReadCurve(const RebarGroupCreationRequest& request,
                                              const LegacyRebarGeometryReader& geometry,
                                              LegacyRebarCurveSnapshot* curve)
{
    if (!request.publicCreateGate.objBResolved) {
        return reject(QStringLiteral(
            "sub_1404D10C0 gate rejected: objB is missing (E-IDA-047)."));
    }
    if (!request.publicCreateGate.objAResolved) {
        return reject(QStringLiteral(
            "sub_1404D10C0 gate rejected: objA is missing (E-IDA-047)."));
    }
    if (request.publicCreateGate.objASub1405F25F0Count < kMinimumPublicCreateObjACount) {
        return reject(QStringLiteral(
            "sub_1404D10C0 gate rejected: sub_1405F25F0(objA) must be at least 3 (E-IDA-047)."));
    }
    if (request.publicCreateGate.entityListCount < kMinimumPublicCreateEntityListCount) {
        return reject(QStringLiteral(
            "sub_1404D10C0 gate rejected: ENTITY_LIST count must be at least 1 (E-IDA-047)."));
    }
    if (request.sourceCurve.shapeKind != LegacyShapeKind::Edge ||
        request.sourceCurve.stableId.empty()) {
        return reject(QStringLiteral("RebarGroupCreator expected a legacy edge ref."));
    }
    if (request.distanceA < kMinimumCreationDistance) {
        return reject(QStringLiteral(
            "sub_1404D10C0 gate rejected: distanceA below VisualTS 0.002 threshold (E-IDA-047)."));
    }

    const auto curveResult = geometry.curveSnapshot(request.sourceCurve, kMinimumSplineSamples);
    if (!curveResult.ok) {
        return reject(curveResult.diagnostic.isEmpty()
                          ? QStringLiteral("RebarGroupCreator failed to read legacy curve.")
                          : curveResult.diagnostic);
    }
    if (curveResult.value.length < kMinimumSplitSegmentLength) {
        return reject(QStringLiteral(
            "RebarGroupCreator rejected legacy segment shorter than VisualTS 0.01 split length."));
    }

    *curve = curveResult.value;
    RebarGroupCreationResult ok;
    ok.ok = true;
    return ok;
}

LegacySegmentCurveNormalizeRequest normalizerRequest(double unresolvedThreshold)
{
    LegacySegmentCurveNormalizeRequest request;
    request.minimumSplitLength = kMinimumSplitSegmentLength;
    request.nearEndpointDistance = 0.1;
    request.trimDelta = -0.03;
    request.endpointSplitStartRatio = 0.001;
    request.endpointSplitEndRatio = 0.999;
    request.trimIterationStep = 0.02;
    request.minimumSplineSamples = kMinimumSplineSamples;
    request.splineSamplesPerUnitLength = 50.0;
    request.unresolvedEndpointDistanceThreshold = unresolvedThreshold;
    return request;
}

SteelBarSegmentShape segmentShapeForCommand(bool arcGroup)
{
    return arcGroup ? SteelBarSegmentShape::Arc : SteelBarSegmentShape::Line;
}

std::string rebarTypeForCommand(bool arcGroup)
{
    return arcGroup ? "arcStb" : "lineStb";
}

std::string steelWayForCommand(bool arcGroup)
{
    return arcGroup ? "AA" : "LA";
}

DomainPoint3d middlePointForCurve(const LegacyRebarCurveSnapshot& curve)
{
    if (!curve.samplePoints.empty()) {
        return toDomainPoint(curve.samplePoints[curve.samplePoints.size() / 2]);
    }
    return {(curve.startPoint.x + curve.endPoint.x) * 0.5,
            (curve.startPoint.y + curve.endPoint.y) * 0.5,
            (curve.startPoint.z + curve.endPoint.z) * 0.5};
}

RebarGroupCreationResult createGroup(const RebarGroupCreationRequest& request,
                                     const LegacyRebarGeometryReader& geometry,
                                     bool arcGroup)
{
    LegacyRebarCurveSnapshot curve;
    auto validation = validateAndReadCurve(request, geometry, &curve);
    if (!validation.ok) {
        return validation;
    }

    const char* commandKey = arcGroup ? "Rebar.Create.ArcGroup" : "Rebar.Create.LineGroup";
    const char* legacyCommand = arcGroup ? "sgroupbararc" : "sgroupbarline";
    const double normalizedDistanceB =
        arcGroup ? kDefaultArcSecondaryDistance : request.distanceB;
    const char effectiveFlag = arcGroup ? 1 : request.legacyFlag;
    const auto normalizeRequest = normalizerRequest(request.distanceA);
    const auto normalizedCurveResult = geometry.normalizeSegmentCurve(curve, normalizeRequest);
    if (!normalizedCurveResult.ok) {
        return reject(normalizedCurveResult.diagnostic.isEmpty()
                          ? QStringLiteral("RebarGroupCreator failed to normalize segment curve.")
                          : normalizedCurveResult.diagnostic);
    }
    const LegacyRebarCurveSnapshot segmentCurve = normalizedCurveResult.value;
    if (segmentCurve.length < kMinimumSplitSegmentLength) {
        return reject(QStringLiteral(
            "RebarGroupCreator rejected normalized segment shorter than VisualTS 0.01 split length."));
    }

    SteelBarSegment segment;
    segment.segmentId = request.segmentId;
    segment.id = segment.segmentId;
    segment.barId = request.barId;
    segment.sequenceNo = 1;
    segment.shapeType = segmentShapeForCommand(arcGroup);
    segment.segmentType = rebarTypeForCommand(arcGroup);
    segment.startPoint = toDomainPoint(segmentCurve.startPoint);
    segment.endPoint = toDomainPoint(segmentCurve.endPoint);
    segment.middlePoint = middlePointForCurve(segmentCurve);
    segment.length = segmentCurve.length;
    segment.lengthMm = segment.length;
    segment.geometryRef.curveStableIds.push_back(segmentCurve.stableId);
    segment.binding.state = BindingState::Resolved;
    segment.binding.items.push_back({"legacyCurve.edgeStableId",
                                     "geometryRef.curveStableIds[0]",
                                     "E-IDA-022"});
    segment.legacyRaw = creationParameters(request, legacyCommand, normalizedDistanceB,
                                           effectiveFlag, true);
    segment.unresolvedLegacyFields.push_back(unresolvedField(
        "sub_1405D5670.arg4",
        "Hex-Rays shows a fourth double used as an endpoint distance threshold, but TODO-020 did not close its call-site source."));
    segment.evidence.push_back(evidenceRef("sub_1405D5670 split / spline / trim chain"));

    SteelBar bar;
    bar.barId = request.barId;
    bar.id = bar.barId;
    bar.groupId = request.groupId;
    bar.sequenceNo = 1;
    bar.displayNumber = "1";
    bar.diameter = request.diameter;
    bar.steelLevel = request.steelLevel;
    bar.segmentIds.push_back(segment.segmentId);
    bar.length = segment.length;
    bar.shapeType = rebarTypeForCommand(arcGroup);
    bar.geometryRef.curveStableIds.push_back(segmentCurve.stableId);
    bar.binding.state = BindingState::Resolved;
    bar.binding.items.push_back({"legacyBar.segment",
                                 "segmentIds[0]",
                                 "E-IDA-022"});
    bar.legacyRaw = creationParameters(request, legacyCommand, normalizedDistanceB,
                                       effectiveFlag, true);
    bar.unresolvedLegacyFields = segment.unresolvedLegacyFields;
    bar.evidence.push_back(evidenceRef("created from VisualTS line/arc group chain"));

    SteelBarGroup group;
    group.groupId = request.groupId;
    group.id = group.groupId;
    group.rsdId = request.rsdId;
    group.displayNumber = request.rsdId;
    group.actualNumber = request.rsdId;
    group.componentName = request.componentName;
    group.projectSteelName = request.projectSteelName;
    group.legacyCommand = legacyCommand;
    group.createCommand = commandKey;
    group.steelDataId = request.steelDataId;
    group.diameter = request.diameter;
    group.interval = request.interval;
    group.barCount = std::max(1, request.requestedBarCount);
    group.segmentCount = 1;
    group.steelLevel = request.steelLevel;
    group.steelWay = steelWayForCommand(arcGroup);
    group.rebarType = rebarTypeForCommand(arcGroup);
    group.sourceLineId = arcGroup ? std::string{} : segmentCurve.stableId;
    group.sourceCurveIds.push_back(segmentCurve.stableId);
    group.referenceIds.push_back(segmentCurve.stableId);
    group.barIds.push_back(bar.barId);
    group.createdFromParameters = creationParameters(request, legacyCommand, normalizedDistanceB,
                                                     effectiveFlag, true);
    group.legacyRaw = group.createdFromParameters;
    group.geometryRef.curveStableIds.push_back(segmentCurve.stableId);
    group.binding.state = BindingState::Resolved;
    group.binding.items.push_back({"legacyEntityList.edge",
                                   "geometryRef.curveStableIds[0]",
                                   "E-IDA-022"});
    group.unresolvedLegacyFields.push_back(unresolvedField(
        "sub_1405D5670.arg4",
        "TODO-020 confirmed the value is used, but did not close its true business source."));
    group.unresolvedLegacyFields.push_back(unresolvedField(
        "objA/objB/createdPayload",
        "TODO-020 kept old object business names unresolved."));
    if (!arcGroup) {
        group.unresolvedLegacyFields.push_back(unresolvedField(
            "sgroupbarline.selectionObjectType",
            "TODO-071 confirmed VisualTS selection gates, but did not close the business name of the selected object type.",
            "GAP-IDA-002"));
    }
    if (arcGroup) {
        group.unresolvedLegacyFields.push_back(unresolvedField(
            "sgroupbararc.uiMapping",
            "TODO-020 did not close whether sgroupbararc maps to fan bars, concentric bars, or shared UI entry.",
            "GAP-IDA-001"));
    }
    group.evidence.push_back(evidenceRef("sgroupbarline / sgroupbararc common creation chain"));
    group.evidence.push_back(evidenceRef(
        "E-IDA-047",
        "TODO-079 confirmed sub_1404D10C0 public creation gate diagnostics"));
    if (!arcGroup) {
        group.evidence.push_back(evidenceRef(
            "E-IDA-045",
            "TODO-071 confirmed sgroupbarline table entry and line-group entry contract"));
    }

    SteelData steelData;
    steelData.steelDataId = request.steelDataId;
    steelData.level = request.steelLevel;
    steelData.diameterSet.push_back(request.diameter);
    steelData.evidence.push_back(evidenceRef("TODO-021 P0 creator output"));
    steelData.segments.push_back(segment);
    steelData.bars.push_back(bar);
    steelData.groups.push_back(group);

    RebarGroupCreationResult result;
    result.ok = true;
    result.steelData = std::move(steelData);
    return result;
}

} // namespace

RebarGroupCreationResult RebarGroupCreator::createLineGroup(
    const RebarGroupCreationRequest& request,
    const LegacyRebarGeometryReader& geometry) const
{
    return createGroup(request, geometry, false);
}

RebarGroupCreationResult RebarGroupCreator::createArcGroup(
    const RebarGroupCreationRequest& request,
    const LegacyRebarGeometryReader& geometry) const
{
    return createGroup(request, geometry, true);
}

} // namespace tsrebar
