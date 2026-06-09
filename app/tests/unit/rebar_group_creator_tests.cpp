#include "domain/rebar/RebarGroupCreator.h"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include <unordered_map>

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

bool hasUnresolvedField(const std::vector<tsrebar::UnresolvedLegacyField>& fields,
                        const std::string& fieldName)
{
    for (const auto& field : fields) {
        if (field.fieldName == fieldName) {
            return true;
        }
    }
    return false;
}

bool hasCreatedParameter(const tsrebar::LegacyRawBlock& block, const std::string& name)
{
    for (const auto& field : block.fields) {
        if (field.name == name) {
            return true;
        }
    }
    return false;
}

std::string createdParameterValue(const tsrebar::LegacyRawBlock& block, const std::string& name)
{
    for (const auto& field : block.fields) {
        if (field.name == name) {
            return field.value;
        }
    }
    return {};
}

tsrebar::LegacySelectionRef edgeRef(const std::string& stableId)
{
    tsrebar::LegacySelectionRef ref;
    ref.stableId = stableId;
    ref.shapeKind = tsrebar::LegacyShapeKind::Edge;
    return ref;
}

class FakeLegacyRebarGeometryReader final : public tsrebar::LegacyRebarGeometryReader
{
public:
    void addCurve(tsrebar::LegacyRebarCurveSnapshot curve)
    {
        curves_.insert_or_assign(curve.stableId, std::move(curve));
    }

    [[nodiscard]] tsrebar::LegacyGeometryQueryResult<tsrebar::LegacyRebarCurveSnapshot>
    curveSnapshot(const tsrebar::LegacySelectionRef& ref, int requestedSampleCount) const override
    {
        ++callCount_;
        lastRequestedSampleCount_ = requestedSampleCount;

        tsrebar::LegacyGeometryQueryResult<tsrebar::LegacyRebarCurveSnapshot> result;
        const auto it = curves_.find(ref.stableId);
        if (it == curves_.end()) {
            result.diagnostic = QStringLiteral("missing fake curve");
            return result;
        }
        result.ok = true;
        result.value = it->second;
        return result;
    }

    [[nodiscard]] tsrebar::LegacyGeometryQueryResult<tsrebar::LegacyRebarCurveSnapshot>
    normalizeSegmentCurve(
        const tsrebar::LegacyRebarCurveSnapshot& curve,
        const tsrebar::LegacySegmentCurveNormalizeRequest& request) const override
    {
        ++normalizeCallCount_;
        lastNormalizeRequest_ = request;

        tsrebar::LegacyGeometryQueryResult<tsrebar::LegacyRebarCurveSnapshot> result;
        result.ok = true;
        result.value = normalizedCurve_.stableId.empty() ? curve : normalizedCurve_;
        return result;
    }

    void setNormalizedCurve(tsrebar::LegacyRebarCurveSnapshot curve)
    {
        normalizedCurve_ = std::move(curve);
    }

    [[nodiscard]] int callCount() const { return callCount_; }
    [[nodiscard]] int lastRequestedSampleCount() const { return lastRequestedSampleCount_; }
    [[nodiscard]] int normalizeCallCount() const { return normalizeCallCount_; }
    [[nodiscard]] const tsrebar::LegacySegmentCurveNormalizeRequest& lastNormalizeRequest() const
    {
        return lastNormalizeRequest_;
    }

private:
    mutable int callCount_ = 0;
    mutable int lastRequestedSampleCount_ = 0;
    mutable int normalizeCallCount_ = 0;
    mutable tsrebar::LegacySegmentCurveNormalizeRequest lastNormalizeRequest_;
    std::unordered_map<std::string, tsrebar::LegacyRebarCurveSnapshot> curves_;
    tsrebar::LegacyRebarCurveSnapshot normalizedCurve_;
};

tsrebar::LegacyRebarCurveSnapshot lineCurve(const std::string& stableId, double length)
{
    tsrebar::LegacyRebarCurveSnapshot curve;
    curve.stableId = stableId;
    curve.curveKind = tsrebar::LegacyCurveKind::Line;
    curve.length = length;
    curve.startPoint = {0.0, 0.0, 0.0};
    curve.endPoint = {length, 0.0, 0.0};
    curve.samplePoints = {
        curve.startPoint,
        {length * 0.25, 0.0, 0.0},
        {length * 0.5, 0.0, 0.0},
        {length * 0.75, 0.0, 0.0},
        curve.endPoint,
    };
    return curve;
}

tsrebar::LegacyRebarCurveSnapshot arcCurve(const std::string& stableId, double length)
{
    tsrebar::LegacyRebarCurveSnapshot curve;
    curve.stableId = stableId;
    curve.curveKind = tsrebar::LegacyCurveKind::Circle;
    curve.length = length;
    curve.startPoint = {10.0, 0.0, 0.0};
    curve.endPoint = {0.0, 10.0, 0.0};
    curve.samplePoints = {
        curve.startPoint,
        {9.2, 3.8, 0.0},
        {7.1, 7.1, 0.0},
        {3.8, 9.2, 0.0},
        curve.endPoint,
    };
    return curve;
}

tsrebar::RebarGroupCreationRequest baseRequest(const std::string& stableId)
{
    tsrebar::RebarGroupCreationRequest request;
    request.groupId = "group-001";
    request.barId = "bar-001";
    request.segmentId = "segment-001";
    request.steelDataId = "steel-data-hrb400";
    request.sourceCurve = edgeRef(stableId);
    request.distanceA = 0.25;
    request.distanceB = 1.2;
    request.legacyFlag = 0;
    request.diameter = 25.0;
    request.interval = 200.0;
    request.requestedBarCount = 3;
    request.steelLevel = "HRB400";
    request.rsdId = "Y12";
    request.componentName = "pier";
    request.projectSteelName = "main bar";
    return request;
}

void testLineGroupCreatorBuildsDomainGroupFromLegacyEdge()
{
    FakeLegacyRebarGeometryReader reader;
    reader.addCurve(lineCurve("selection-v1:edge:17", 12.5));
    auto normalized = lineCurve("selection-v1:edge:17", 11.5);
    normalized.startPoint = {0.5, 0.0, 0.0};
    normalized.endPoint = {12.0, 0.0, 0.0};
    normalized.samplePoints = {
        normalized.startPoint,
        {3.375, 0.0, 0.0},
        {6.25, 0.0, 0.0},
        {9.125, 0.0, 0.0},
        normalized.endPoint,
    };
    reader.setNormalizedCurve(normalized);

    const auto result = tsrebar::RebarGroupCreator{}.createLineGroup(
        baseRequest("selection-v1:edge:17"), reader);

    expect(result.ok, "line group creation must succeed");
    expect(result.steelData.groups.size() == 1, "line creation must emit one group");
    expect(result.steelData.bars.size() == 1, "line creation must emit one bar");
    expect(result.steelData.segments.size() == 1, "line creation must emit one segment");
    expect(reader.lastRequestedSampleCount() == 5,
           "creator must request the old minimum spline sample count");
    expect(reader.normalizeCallCount() == 1,
           "creator must call SegmentCurveNormalizer P0 before building domain segment");
    expect(near(reader.lastNormalizeRequest().minimumSplitLength, 0.01),
           "normalizer request must carry VisualTS split length threshold");
    expect(near(reader.lastNormalizeRequest().trimDelta, -0.03),
           "normalizer request must carry VisualTS trim delta");

    const auto& group = result.steelData.groups.front();
    const auto& bar = result.steelData.bars.front();
    const auto& segment = result.steelData.segments.front();

    expect(group.createCommand == "Rebar.Create.LineGroup", "line group command mismatch");
    expect(group.legacyCommand == "sgroupbarline", "line legacy command mismatch");
    expect(group.sourceCurveIds.front() == "selection-v1:edge:17",
           "line source curve id mismatch");
    expect(group.geometryRef.curveStableIds.front() == "selection-v1:edge:17",
           "line geometry binding mismatch");
    expect(group.binding.state == tsrebar::BindingState::Resolved,
           "line group binding must resolve to legacy geometry");
    expect(group.barIds.front() == bar.barId, "line group must reference bar");
    expect(bar.segmentIds.front() == segment.segmentId, "line bar must reference segment");
    expect(segment.shapeType == tsrebar::SteelBarSegmentShape::Line,
           "line segment shape mismatch");
    expect(near(segment.length, 11.5), "line segment length must use normalized curve");
    expect(near(segment.startPoint.x, 0.5), "line segment start must use normalized curve");
    expect(group.barCount == 3, "line group bar count mismatch");
    expect(group.segmentCount == 1, "line group segment count mismatch");
    expect(hasEvidence(group.evidence, "E-IDA-022"), "line group must carry IDA evidence");
    expect(hasEvidence(group.evidence, "E-IDA-045"),
           "line group must carry TODO-071 sgroupbarline entry evidence");
    expect(hasUnresolvedField(group.unresolvedLegacyFields, "sub_1405D5670.fullEquivalence"),
           "line group must keep unresolved full equivalence field");
    expect(hasCreatedParameter(group.createdFromParameters, "minimumCreationDistance"),
           "line group must record legacy creation threshold");
    expect(createdParameterValue(group.createdFromParameters,
                                 "sub_1404D10C0.objA.present") == "true",
           "line group must record old objA presence gate");
    expect(createdParameterValue(group.createdFromParameters,
                                 "sub_1404D10C0.objB.present") == "true",
           "line group must record old objB presence gate");
    expect(createdParameterValue(group.createdFromParameters,
                                 "sub_1404D10C0.entityList.count") == "1",
           "line group must record old ENTITY_LIST count gate");
    expect(createdParameterValue(group.createdFromParameters,
                                 "sub_1404D10C0.objA.sub_1405F25F0.count") == "3",
           "line group must record old sub_1405F25F0(objA) gate");
    expect(createdParameterValue(group.createdFromParameters,
                                 "sub_1404D10C0.objA.sub_1405F25F0.minimum") == "3",
           "line group must record old sub_1405F25F0(objA) minimum");
    expect(createdParameterValue(group.createdFromParameters, "sgroupbarline.selectionCount") == "1",
           "line group must record old single-selection entry contract");
    expect(createdParameterValue(group.createdFromParameters,
                                 "sgroupbarline.selectionGate.structureCandidate") == "sub_1405C6820",
           "line group must record old structure selection gate");
    expect(createdParameterValue(group.createdFromParameters,
                                 "sgroupbarline.selectionGate.payloadCandidate") == "sub_1405DA020",
           "line group must record old payload selection gate");
    expect(createdParameterValue(group.createdFromParameters,
                                 "sgroupbarline.endpointDistanceCandidateCount") == "4",
           "line group must record old four endpoint distance candidates");
    expect(createdParameterValue(group.createdFromParameters,
                                 "sgroupbarline.initialMinimumDistanceCandidate") == "10.0",
           "line group must preserve old initial minimum distance literal");
    expect(hasCreatedParameter(group.createdFromParameters,
                               "sgroupbarline.oddIndexedEntityExtraction"),
           "line group must record odd-indexed entity extraction");
}

void testLineGroupCreatorRecordsPublicCreateRolesRawEvidence()
{
    FakeLegacyRebarGeometryReader reader;
    reader.addCurve(lineCurve("selection-v1:edge:roles", 18.0));

    auto request = baseRequest("selection-v1:edge:roles");
    request.distanceA = 0.12349;
    request.publicCreateRoles.createdPayloadRef = "createdObject+104";
    request.publicCreateRoles.linkedModelRef = "createdObject+112";
    request.publicCreateRoles.linkedModelRefConfidence = "low";
    request.publicCreateRoles.objARole = "createContextOwner/sourceContextObject";
    request.publicCreateRoles.objBRole = "refreshTargetObject/counterpartObject";
    request.publicCreateRoles.objAObjBMaySwapByEntryPoint = true;

    const auto result = tsrebar::RebarGroupCreator{}.createLineGroup(request, reader);

    expect(result.ok, "line group role evidence creation must succeed");
    const auto& group = result.steelData.groups.front();

    expect(createdParameterValue(group.createdFromParameters,
                                 "sub_1405D5670.distanceA4Digit") == "0.1234",
           "line group must record TODO-081 distanceA_4digit truncation");
    expect(near(reader.lastNormalizeRequest().unresolvedEndpointDistanceThreshold, 0.1234),
           "normalizer request must use distanceA_4digit, matching sub_1405D5670 arg4");
    expect(createdParameterValue(group.createdFromParameters,
                                 "sub_140451730.createdPayloadRef") == "createdObject+104",
           "line group must record createdPayloadRef raw evidence");
    expect(createdParameterValue(group.createdFromParameters,
                                 "sub_140451730.linkedModelRef") == "createdObject+112",
           "line group must record linkedModelRef raw evidence");
    expect(createdParameterValue(group.createdFromParameters,
                                 "sub_140451730.linkedModelRef.confidence") == "low",
           "linkedModelRef must remain low confidence");
    expect(createdParameterValue(group.createdFromParameters,
                                 "sub_1404D10C0.objA.roleCandidate") ==
               "createContextOwner/sourceContextObject",
           "objA role must remain a candidate role, not a fixed business name");
    expect(createdParameterValue(group.createdFromParameters,
                                 "sub_1404D10C0.objB.roleCandidate") ==
               "refreshTargetObject/counterpartObject",
           "objB role must remain a candidate role, not a fixed business name");
    expect(createdParameterValue(group.createdFromParameters,
                                 "sub_1404D10C0.objAObjB.rolesMaySwapByEntryPoint") == "true",
           "line group must record objA/objB role swap risk");
    expect(createdParameterValue(group.createdFromParameters,
                                 "sub_1404D10C0.objA.roleCandidate") != "sourceSteelGroup",
           "objA must not be hard-coded as sourceSteelGroup");
    expect(createdParameterValue(group.createdFromParameters,
                                 "sub_1404D10C0.objB.roleCandidate") != "targetSteelGroup",
           "objB must not be hard-coded as targetSteelGroup");
    expect(!hasUnresolvedField(group.unresolvedLegacyFields, "sub_1405D5670.arg4"),
           "distanceA_4digit source is now closed by TODO-081 and should not remain unresolved");
    expect(hasUnresolvedField(group.unresolvedLegacyFields, "createdObject+112.businessMeaning"),
           "linkedModelRef business meaning must remain explicitly unresolved");
    expect(hasEvidence(group.evidence, "E-IDA-048"),
           "line group roles DTO must carry TODO-081 IDA evidence");
}

void testArcGroupCreatorBuildsArcSegmentAndKeepsUnresolvedUiGap()
{
    FakeLegacyRebarGeometryReader reader;
    reader.addCurve(arcCurve("selection-v1:edge:42", 15.7));

    auto request = baseRequest("selection-v1:edge:42");
    request.groupId = "group-arc-001";
    request.barId = "bar-arc-001";
    request.segmentId = "segment-arc-001";
    request.distanceA = 0.1;
    request.distanceB = 2.5;
    request.legacyFlag = 0;

    const auto result = tsrebar::RebarGroupCreator{}.createArcGroup(request, reader);

    expect(result.ok, "arc group creation must succeed");
    const auto& group = result.steelData.groups.front();
    const auto& segment = result.steelData.segments.front();

    expect(group.createCommand == "Rebar.Create.ArcGroup", "arc group command mismatch");
    expect(group.legacyCommand == "sgroupbararc", "arc legacy command mismatch");
    expect(group.rebarType == "arcStb", "arc group rebar type mismatch");
    expect(segment.shapeType == tsrebar::SteelBarSegmentShape::Arc,
           "arc segment shape mismatch");
    expect(near(segment.middlePoint.x, 7.1) && near(segment.middlePoint.y, 7.1),
           "arc segment must use the legacy curve midpoint sample");
    expect(hasCreatedParameter(group.createdFromParameters, "arcSecondaryDistance"),
           "arc group must record old 0.8 secondary distance");
    expect(createdParameterValue(group.createdFromParameters, "distanceB") == "0.8",
           "arc group must force old distanceB=0.8 even if caller passed another value");
    expect(hasCreatedParameter(group.createdFromParameters, "segmentCurveNormalizerP0"),
           "arc group must record SegmentCurveNormalizer P0 application");
    expect(hasCreatedParameter(group.createdFromParameters, "legacyFlagForcedByCommand"),
           "arc group must record that sgroupbararc forces legacy flag 1");
    expect(hasUnresolvedField(group.unresolvedLegacyFields, "sgroupbararc.uiMapping"),
           "arc group must keep unresolved UI mapping gap");
    expect(hasEvidence(segment.evidence, "E-IDA-022"), "arc segment must carry IDA evidence");
}

void testCreationRejectsDistanceBelowVisualTsThresholdBeforeGeometryQuery()
{
    FakeLegacyRebarGeometryReader reader;
    reader.addCurve(lineCurve("selection-v1:edge:17", 12.5));

    auto request = baseRequest("selection-v1:edge:17");
    request.distanceA = 0.0019;

    const auto result = tsrebar::RebarGroupCreator{}.createLineGroup(request, reader);

    expect(!result.ok, "distance below 0.002 must be rejected");
    expect(result.diagnostic.contains(QStringLiteral("sub_1404D10C0")),
           "distance diagnostic must mention old common creation gate");
    expect(result.diagnostic.contains(QStringLiteral("0.002")),
           "threshold diagnostic must mention 0.002");
    expect(reader.callCount() == 0, "legacy distance gate must run before geometry query");
}

void testCreationRejectsMissingLegacyObjBBeforeGeometryQuery()
{
    FakeLegacyRebarGeometryReader reader;
    reader.addCurve(lineCurve("selection-v1:edge:17", 12.5));

    auto request = baseRequest("selection-v1:edge:17");
    request.publicCreateGate.objBResolved = false;

    const auto result = tsrebar::RebarGroupCreator{}.createLineGroup(request, reader);

    expect(!result.ok, "missing objB surrogate must be rejected");
    expect(result.diagnostic.contains(QStringLiteral("sub_1404D10C0")),
           "objB diagnostic must mention old common creation gate");
    expect(result.diagnostic.contains(QStringLiteral("objB")),
           "objB diagnostic must mention objB");
    expect(reader.callCount() == 0, "objB gate must run before geometry query");
}

void testCreationRejectsMissingLegacyObjABeforeGeometryQuery()
{
    FakeLegacyRebarGeometryReader reader;
    reader.addCurve(lineCurve("selection-v1:edge:17", 12.5));

    auto request = baseRequest("selection-v1:edge:17");
    request.publicCreateGate.objAResolved = false;

    const auto result = tsrebar::RebarGroupCreator{}.createLineGroup(request, reader);

    expect(!result.ok, "missing objA surrogate must be rejected");
    expect(result.diagnostic.contains(QStringLiteral("sub_1404D10C0")),
           "objA diagnostic must mention old common creation gate");
    expect(result.diagnostic.contains(QStringLiteral("objA")),
           "objA diagnostic must mention objA");
    expect(reader.callCount() == 0, "objA gate must run before geometry query");
}

void testCreationRejectsLegacyObjACountBelowVisualTsGateBeforeGeometryQuery()
{
    FakeLegacyRebarGeometryReader reader;
    reader.addCurve(lineCurve("selection-v1:edge:17", 12.5));

    auto request = baseRequest("selection-v1:edge:17");
    request.publicCreateGate.objASub1405F25F0Count = 2;

    const auto result = tsrebar::RebarGroupCreator{}.createLineGroup(request, reader);

    expect(!result.ok, "sub_1405F25F0(objA) below 3 must be rejected");
    expect(result.diagnostic.contains(QStringLiteral("sub_1405F25F0")),
           "objA count diagnostic must mention sub_1405F25F0");
    expect(result.diagnostic.contains(QStringLiteral("3")),
           "objA count diagnostic must mention minimum 3");
    expect(reader.callCount() == 0, "objA count gate must run before geometry query");
}

void testCreationRejectsEmptyLegacyEntityListBeforeGeometryQuery()
{
    FakeLegacyRebarGeometryReader reader;
    reader.addCurve(lineCurve("selection-v1:edge:17", 12.5));

    auto request = baseRequest("selection-v1:edge:17");
    request.publicCreateGate.entityListCount = 0;

    const auto result = tsrebar::RebarGroupCreator{}.createLineGroup(request, reader);

    expect(!result.ok, "empty ENTITY_LIST surrogate must be rejected");
    expect(result.diagnostic.contains(QStringLiteral("ENTITY_LIST")),
           "entity list diagnostic must mention ENTITY_LIST");
    expect(result.diagnostic.contains(QStringLiteral("1")),
           "entity list diagnostic must mention minimum 1");
    expect(reader.callCount() == 0, "ENTITY_LIST gate must run before geometry query");
}

void testCreationRejectsShortLegacySegment()
{
    FakeLegacyRebarGeometryReader reader;
    reader.addCurve(lineCurve("selection-v1:edge:short", 0.009));

    const auto result = tsrebar::RebarGroupCreator{}.createLineGroup(
        baseRequest("selection-v1:edge:short"), reader);

    expect(!result.ok, "short segment must be rejected");
    expect(result.diagnostic.contains(QStringLiteral("0.01")),
           "short segment diagnostic must mention old split length threshold");
}

void testCreationRejectsInvalidLegacyRef()
{
    FakeLegacyRebarGeometryReader reader;
    auto request = baseRequest("selection-v1:vertex:3");
    request.sourceCurve.shapeKind = tsrebar::LegacyShapeKind::Vertex;

    const auto result = tsrebar::RebarGroupCreator{}.createLineGroup(request, reader);

    expect(!result.ok, "non-edge legacy ref must be rejected");
    expect(result.diagnostic.contains(QStringLiteral("edge ref")),
           "invalid ref diagnostic must mention edge ref");
}

} // namespace

int main()
{
    testLineGroupCreatorBuildsDomainGroupFromLegacyEdge();
    testLineGroupCreatorRecordsPublicCreateRolesRawEvidence();
    testArcGroupCreatorBuildsArcSegmentAndKeepsUnresolvedUiGap();
    testCreationRejectsDistanceBelowVisualTsThresholdBeforeGeometryQuery();
    testCreationRejectsMissingLegacyObjBBeforeGeometryQuery();
    testCreationRejectsMissingLegacyObjABeforeGeometryQuery();
    testCreationRejectsLegacyObjACountBelowVisualTsGateBeforeGeometryQuery();
    testCreationRejectsEmptyLegacyEntityListBeforeGeometryQuery();
    testCreationRejectsShortLegacySegment();
    testCreationRejectsInvalidLegacyRef();
    return 0;
}
