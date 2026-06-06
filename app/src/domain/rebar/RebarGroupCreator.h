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

class LegacyRebarGeometryReader
{
public:
    virtual ~LegacyRebarGeometryReader() = default;

    [[nodiscard]] virtual LegacyGeometryQueryResult<LegacyRebarCurveSnapshot>
    curveSnapshot(const LegacySelectionRef& ref, int requestedSampleCount) const = 0;

    [[nodiscard]] virtual LegacyGeometryQueryResult<LegacyRebarCurveSnapshot>
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
