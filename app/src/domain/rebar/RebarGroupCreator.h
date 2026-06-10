#pragma once

#include "domain/rebar/SteelData.h"
#include "geometry/legacy/LegacyGeometry.h"
#include "geometry/legacy/LegacySelectionRef.h"

#include <QString>

#include <string>
#include <vector>

namespace tsrebar {

struct LegacyRebarCurveSnapshot
{
    std::string stableId;
    LegacyCurveKind curveKind = LegacyCurveKind::Unknown;
    double length = 0.0;
    LegacyPoint3d startPoint;
    LegacyPoint3d endPoint;
    std::vector<LegacyPoint3d> samplePoints;
};

struct LegacySegmentCurveNormalizeRequest
{
    double minimumSplitLength = 0.01;
    double nearEndpointDistance = 0.1;
    double trimDelta = -0.03;
    double endpointSplitStartRatio = 0.001;
    double endpointSplitEndRatio = 0.999;
    double trimIterationStep = 0.02;
    int minimumSplineSamples = 5;
    double splineSamplesPerUnitLength = 50.0;
    double unresolvedEndpointDistanceThreshold = 0.0;
};

struct LegacySegmentCurveNormalizeTrace
{
    std::string capabilityLevel = "p0-summary";
    bool entityDistanceAttempted = false;
    bool entityDistanceApplied = false;
    bool splitCurveAttempted = false;
    bool splitCurveApplied = false;
    bool curveSplineAttempted = false;
    bool curveSplineApplied = false;
    bool startTrimAttempted = false;
    bool startTrimApplied = false;
    bool endTrimAttempted = false;
    bool endTrimApplied = false;
    bool groupMinimumDistanceTrimLoopObserved = false;
    bool groupMinimumDistanceUsesApiEntityPointDistance = false;
    bool groupMinimumDistanceStartEndpointProbeDeferred = true;
    bool groupMinimumDistanceEndEndpointProbeDeferred = true;
    int groupMinimumDistanceStartIterationBudget = 0;
    int groupMinimumDistanceEndIterationBudget = 0;
    double groupMinimumDistanceThreshold = 0.0;
    bool groupMinimumDistanceTrimLoopDeferred = true;
    bool backupWriteEdgeObserved = false;
    bool backupWriteEdgeEntityBackupObserved = false;
    bool backupWriteEdgeEntitySlot72Observed = false;
    bool backupWriteEdgeDeferred = true;
    bool postCreateMutationOrderObserved = false;
    bool dirtyWriteDeferred = true;
    int effectiveSplineSampleCount = 0;
};

struct LegacySegmentCurveNormalizeResult
{
    LegacyRebarCurveSnapshot curve;
    LegacySegmentCurveNormalizeTrace trace;
};

struct LegacyPublicCreateGateSnapshot
{
    bool objAResolved = true;
    bool objBResolved = true;
    int objASub1405F25F0Count = 3;
    int entityListCount = 1;
};

struct LegacyPublicCreateRolesSnapshot
{
    std::string objARole = "createContextOwner/sourceContextObject";
    std::string objBRole = "refreshTargetObject/counterpartObject";
    std::string roleConfidence = "candidate";
    bool objAObjBMaySwapByEntryPoint = true;
    std::string createdPayloadRef = "createdObject+104";
    std::string linkedModelRef = "createdObject+112";
    std::string linkedModelRefConfidence = "low";
};

class LegacyRebarGeometryReader
{
public:
    virtual ~LegacyRebarGeometryReader() = default;

    [[nodiscard]] virtual LegacyGeometryQueryResult<LegacyRebarCurveSnapshot>
    curveSnapshot(const LegacySelectionRef& ref, int requestedSampleCount) const = 0;

    [[nodiscard]] virtual LegacyGeometryQueryResult<LegacySegmentCurveNormalizeResult>
    normalizeSegmentCurve(const LegacyRebarCurveSnapshot& curve,
                          const LegacySegmentCurveNormalizeRequest& request) const = 0;
};

struct RebarGroupCreationRequest
{
    std::string groupId;
    std::string barId;
    std::string segmentId;
    std::string steelDataId;
    LegacySelectionRef sourceCurve;
    LegacyPublicCreateGateSnapshot publicCreateGate;
    LegacyPublicCreateRolesSnapshot publicCreateRoles;
    double distanceA = 0.0;
    double distanceB = 0.0;
    char legacyFlag = 0;
    double diameter = 0.0;
    double interval = 0.0;
    int requestedBarCount = 1;
    std::string steelLevel;
    std::string rsdId;
    std::string componentName;
    std::string projectSteelName;
};

struct RebarGroupCreationResult
{
    bool ok = false;
    SteelData steelData;
    QString diagnostic;
};

class RebarGroupCreator
{
public:
    [[nodiscard]] RebarGroupCreationResult createLineGroup(
        const RebarGroupCreationRequest& request,
        const LegacyRebarGeometryReader& geometry) const;

    [[nodiscard]] RebarGroupCreationResult createArcGroup(
        const RebarGroupCreationRequest& request,
        const LegacyRebarGeometryReader& geometry) const;
};

} // namespace tsrebar
