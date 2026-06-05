#pragma once

#include "geometry/legacy/LegacyGeometry.h"
#include "geometry/legacy/LegacySelectionRef.h"
#include "geometry/occ/selection/OccSelectionIndex.h"

namespace tsrebar {

class OccLegacyGeometryAdapter
{
public:
    explicit OccLegacyGeometryAdapter(OccSelectionIndex selectionIndex);

    [[nodiscard]] LegacyGeometryQueryResult<LegacyEdgeGeometry> edgeGeometry(
        const LegacySelectionRef& ref) const;
    [[nodiscard]] LegacyGeometryQueryResult<LegacyFaceGeometry> faceGeometry(
        const LegacySelectionRef& ref) const;
    [[nodiscard]] LegacyGeometryQueryResult<LegacyPoint3d> edgePointAtParameter(
        const LegacySelectionRef& ref,
        double parameter) const;
    [[nodiscard]] LegacyGeometryQueryResult<LegacyEdgeDifferential> edgeDifferentialAtParameter(
        const LegacySelectionRef& ref,
        double parameter) const;
    [[nodiscard]] LegacyGeometryQueryResult<std::vector<LegacyPoint3d>> edgeSamplePoints(
        const LegacySelectionRef& ref,
        int sampleCount) const;
    [[nodiscard]] LegacyGeometryQueryResult<LegacyDistanceMeasure> distanceBetween(
        const LegacySelectionRef& first,
        const LegacySelectionRef& second) const;
    [[nodiscard]] LegacyGeometryQueryResult<LegacyIntersectionSet> edgeFaceIntersections(
        const LegacySelectionRef& edgeRef,
        const LegacySelectionRef& faceRef) const;
    [[nodiscard]] LegacyGeometryQueryResult<LegacyCurveInterval> edgeInterval(
        const LegacySelectionRef& edgeRef,
        double firstParameter,
        double lastParameter,
        int sampleCount) const;
    [[nodiscard]] LegacyGeometryQueryResult<LegacyEdgePointProjection> edgeProjectPoint(
        const LegacySelectionRef& edgeRef,
        LegacyPoint3d point) const;
    [[nodiscard]] LegacyGeometryQueryResult<LegacyCurveSplit> edgeSplitAtParameter(
        const LegacySelectionRef& edgeRef,
        double splitParameter,
        int sampleCountPerInterval) const;
    [[nodiscard]] LegacyGeometryQueryResult<LegacyCurveSplit> edgeSplitAtPoint(
        const LegacySelectionRef& edgeRef,
        LegacyPoint3d point,
        int sampleCountPerInterval) const;
    [[nodiscard]] LegacyGeometryQueryResult<LegacyEdgeEndpointTrim> edgeTrimEndpoint(
        const LegacySelectionRef& edgeRef,
        double delta,
        int endFlag,
        int sampleCount) const;
    [[nodiscard]] LegacyGeometryQueryResult<LegacyEdgeGroupDistance> pointToEdgeGroupDistance(
        LegacyPoint3d point,
        const std::vector<LegacySelectionRef>& edgeRefs,
        double threshold) const;
    [[nodiscard]] LegacyGeometryQueryResult<LegacySplineCurveBuild> buildSplineFromPoints(
        const std::vector<LegacyPoint3d>& points,
        int sampleCount) const;
    [[nodiscard]] LegacyGeometryQueryResult<LegacyWireChain> buildWireChain(
        const std::vector<LegacySelectionRef>& edgeRefs) const;

private:
    OccSelectionIndex m_selectionIndex;
};

} // namespace tsrebar
