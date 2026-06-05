#include "geometry/occ/import/OcctStepImportService.h"
#include "geometry/occ/legacy_adapter/OccLegacyGeometryAdapter.h"
#include "geometry/occ/selection/OccSelectionIndex.h"

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

namespace {

void expect(bool condition, const char* message)
{
    if (!condition) {
        std::cerr << message << '\n';
        std::exit(1);
    }
}

double distance(const tsrebar::LegacyPoint3d& lhs, const tsrebar::LegacyPoint3d& rhs)
{
    const double dx = lhs.x - rhs.x;
    const double dy = lhs.y - rhs.y;
    const double dz = lhs.z - rhs.z;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

void expectValidBox(const tsrebar::LegacyBoundingBox& box, const char* message)
{
    expect(box.valid, message);
    expect(box.minPoint.x <= box.maxPoint.x, "bbox x range must be ordered");
    expect(box.minPoint.y <= box.maxPoint.y, "bbox y range must be ordered");
    expect(box.minPoint.z <= box.maxPoint.z, "bbox z range must be ordered");
}

} // namespace

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "usage: legacy_geometry_adapter_tests <123.stp>\n";
        return 2;
    }

    const tsrebar::OcctImportedDocument document =
        tsrebar::OcctStepImportService().importDocument(QString::fromLocal8Bit(argv[1]));
    expect(document.isSuccess(), document.summary.error.c_str());

    const tsrebar::OccSelectionIndex selectionIndex =
        tsrebar::OccSelectionIndex::fromDocument(document);
    const tsrebar::OccLegacyGeometryAdapter adapter(selectionIndex);

    const auto edgeRefs = selectionIndex.refs(tsrebar::LegacyShapeKind::Edge);
    expect(!edgeRefs.isEmpty(), "test STEP must have selectable edges");

    const auto edgeResult = adapter.edgeGeometry(edgeRefs.front());
    expect(edgeResult.ok, edgeResult.diagnostic.toUtf8().constData());
    expect(edgeResult.value.length > 1000.0, "edge length must be measured from OCCT geometry");
    expect(distance(edgeResult.value.startPoint, edgeResult.value.endPoint) > 1000.0,
           "edge endpoints must not collapse");
    expect(edgeResult.value.curveKind != tsrebar::LegacyCurveKind::Unknown,
           "edge curve kind must be classified");
    expect(edgeResult.value.stableId == edgeRefs.front().stableId,
           "edge geometry must preserve stable selection id");
    expectValidBox(edgeResult.value.bounds, "edge bbox must be available");
    expect(!edgeResult.value.fingerprint.empty(), "edge fingerprint must be available");
    expect(edgeResult.value.fingerprint.find("edge-fingerprint-v1") == 0,
           "edge fingerprint must use a stable schema prefix");

    const double middleParameter =
        edgeResult.value.firstParameter +
        (edgeResult.value.lastParameter - edgeResult.value.firstParameter) * 0.5;
    const auto middlePoint = adapter.edgePointAtParameter(edgeRefs.front(), middleParameter);
    expect(middlePoint.ok, middlePoint.diagnostic.toUtf8().constData());
    expect(distance(middlePoint.value, edgeResult.value.startPoint) > 1.0,
           "midpoint must not collapse to edge start");

    const auto middleProjection = adapter.edgeProjectPoint(edgeRefs.front(), middlePoint.value);
    expect(middleProjection.ok, middleProjection.diagnostic.toUtf8().constData());
    expect(middleProjection.value.edgeStableId == edgeRefs.front().stableId,
           "edge projection must preserve source edge stable id");
    expect(middleProjection.value.parameterValid,
           "midpoint projection parameter must be valid");
    expect(middleProjection.value.inside,
           "midpoint projection must be inside edge interval");
    expect(std::fabs(middleProjection.value.parameter - middleParameter) < 1.0e-6,
           "midpoint projection parameter must match source parameter");
    expect(middleProjection.value.distance < 1.0e-6,
           "midpoint projection distance must be near zero");
    expect(distance(middleProjection.value.projectedPoint, middlePoint.value) < 1.0e-6,
           "midpoint projection point must match input point on edge");

    const auto endpointProjection =
        adapter.edgeProjectPoint(edgeRefs.front(), edgeResult.value.startPoint);
    expect(endpointProjection.ok, endpointProjection.diagnostic.toUtf8().constData());
    expect(endpointProjection.value.parameterValid,
           "endpoint projection parameter must be valid");
    expect(!endpointProjection.value.inside,
           "endpoint projection must not be split-safe inside interval");
    expect(endpointProjection.value.distance < 1.0e-6,
           "endpoint projection distance must be near zero");

    const auto middleDifferential =
        adapter.edgeDifferentialAtParameter(edgeRefs.front(), middleParameter);
    expect(middleDifferential.ok, middleDifferential.diagnostic.toUtf8().constData());
    expect(distance(middleDifferential.value.point, middlePoint.value) < 1.0e-6,
           "edge differential point must match pointAtParameter");
    expect(middleDifferential.value.tangent.valid, "edge tangent must be available");
    const double tangentLength =
        std::sqrt(middleDifferential.value.tangent.x * middleDifferential.value.tangent.x +
                  middleDifferential.value.tangent.y * middleDifferential.value.tangent.y +
                  middleDifferential.value.tangent.z * middleDifferential.value.tangent.z);
    expect(std::fabs(tangentLength - 1.0) < 1.0e-6, "edge tangent must be unit length");

    const auto samples = adapter.edgeSamplePoints(edgeRefs.front(), 5);
    expect(samples.ok, samples.diagnostic.toUtf8().constData());
    expect(samples.value.size() == 5, "edge sample count must match request");
    expect(distance(samples.value.front(), edgeResult.value.startPoint) < 1.0e-6,
           "first sample must match start point");
    expect(distance(samples.value.back(), edgeResult.value.endPoint) < 1.0e-6,
           "last sample must match end point");

    const auto spline = adapter.buildSplineFromPoints(samples.value, 7);
    expect(spline.ok, spline.diagnostic.toUtf8().constData());
    expect(spline.value.buildable, "spline build summary must be marked buildable");
    expect(spline.value.inputPointCount == 5,
           "spline build summary must preserve input point count");
    expect(spline.value.samplePoints.size() == 7,
           "spline build summary sample count must match request");
    expect(spline.value.legacySuggestedSampleCount >= 5,
           "spline build summary must expose legacy max(5, length * 50) sample count");
    expect(spline.value.curveKind == tsrebar::LegacyCurveKind::BSpline,
           "spline build summary must classify generated curve as BSpline");
    expect(spline.value.length > 0.0, "spline build summary length must be positive");
    expectValidBox(spline.value.bounds, "spline build summary bbox must be available");
    expect(distance(spline.value.samplePoints.front(), samples.value.front()) < 1.0e-6,
           "spline build first sample must match input start point");
    expect(distance(spline.value.samplePoints.back(), samples.value.back()) < 1.0e-6,
           "spline build last sample must match input end point");

    const auto splineWithProtectedSampleCount =
        adapter.buildSplineFromPoints(samples.value, 1);
    expect(splineWithProtectedSampleCount.ok,
           splineWithProtectedSampleCount.diagnostic.toUtf8().constData());
    expect(splineWithProtectedSampleCount.value.samplePoints.size() == 5,
           "spline build must protect sample count with legacy minimum");
    expect(splineWithProtectedSampleCount.value.requestedSampleCount == 1,
           "spline build summary must preserve requested sample count");
    expect(splineWithProtectedSampleCount.value.effectiveSampleCount == 5,
           "spline build summary must expose effective legacy sample count");

    std::vector<tsrebar::LegacyPoint3d> insufficientSplinePoints{samples.value.front(),
                                                                 samples.value.back()};
    const auto insufficientSpline =
        adapter.buildSplineFromPoints(insufficientSplinePoints, 5);
    expect(!insufficientSpline.ok, "spline build must reject insufficient point count");
    expect(insufficientSpline.diagnostic.contains(QStringLiteral("at least 3")),
           "insufficient spline point diagnostic must be stable");
    expect(!insufficientSpline.value.failureReason.empty(),
           "insufficient spline failure must be carried in DTO");

    std::vector<tsrebar::LegacyPoint3d> collapsedSplinePoints(3, samples.value.front());
    const auto collapsedSpline = adapter.buildSplineFromPoints(collapsedSplinePoints, 5);
    expect(!collapsedSpline.ok, "spline build must reject collapsed point lists");
    expect(collapsedSpline.diagnostic.contains(QStringLiteral("length too short")),
           "collapsed spline diagnostic must be stable");
    expect(collapsedSpline.value.failureReason.find("length too short") != std::string::npos,
           "collapsed spline failure reason must be carried in DTO");

    const auto faceRefs = selectionIndex.refs(tsrebar::LegacyShapeKind::Face);
    expect(!faceRefs.isEmpty(), "test STEP must have selectable faces");

    const auto faceResult = adapter.faceGeometry(faceRefs.front());
    expect(faceResult.ok, faceResult.diagnostic.toUtf8().constData());
    expect(faceResult.value.area > 0.0, "face area must be measured from OCCT geometry");
    expect(faceResult.value.boundaryEdgeCount > 0,
           "face boundary edge count must be available");
    expect(faceResult.value.surfaceKind != tsrebar::LegacySurfaceKind::Unknown,
           "face surface kind must be classified");
    expect(faceResult.value.normal.valid, "face normal must be available for first test face");
    const double normalLength = std::sqrt(faceResult.value.normal.x * faceResult.value.normal.x +
                                          faceResult.value.normal.y * faceResult.value.normal.y +
                                          faceResult.value.normal.z * faceResult.value.normal.z);
    expect(std::fabs(normalLength - 1.0) < 1.0e-6, "face normal must be unit length");
    expect(faceResult.value.stableId == faceRefs.front().stableId,
           "face geometry must preserve stable selection id");
    expectValidBox(faceResult.value.bounds, "face bbox must be available");
    expect(!faceResult.value.fingerprint.empty(), "face fingerprint must be available");
    expect(faceResult.value.fingerprint.find("face-fingerprint-v1") == 0,
           "face fingerprint must use a stable schema prefix");
    expect(!faceResult.value.boundaryLoops.empty(), "face boundary loops must be available");
    expect(faceResult.value.boundaryLoops.front().outer, "first boundary loop must be outer");
    expect(faceResult.value.boundaryLoops.front().edgeCount > 0,
           "outer boundary loop edge count must be available");
    expect(!faceResult.value.boundaryLoops.front().edgeStableIds.empty(),
           "outer boundary loop edge stable ids must be available");

    std::vector<tsrebar::LegacySelectionRef> boundaryEdgeRefs;
    boundaryEdgeRefs.reserve(faceResult.value.boundaryLoops.front().edgeStableIds.size());
    for (const std::string& stableId : faceResult.value.boundaryLoops.front().edgeStableIds) {
        const auto parsedRef =
            tsrebar::parseStableSelectionString(QString::fromStdString(stableId));
        expect(parsedRef.has_value(), "boundary wire edge stable id must be parseable");
        boundaryEdgeRefs.push_back(*parsedRef);
    }
    const auto boundaryWire = adapter.buildWireChain(boundaryEdgeRefs);
    expect(boundaryWire.ok, boundaryWire.diagnostic.toUtf8().constData());
    expect(boundaryWire.value.inputEdgeCount == static_cast<int>(boundaryEdgeRefs.size()),
           "boundary wire chain must preserve input edge count");
    expect(boundaryWire.value.orderedEdgeCount == boundaryWire.value.inputEdgeCount,
           "boundary wire chain must order every input edge");
    expect(boundaryWire.value.connected, "boundary wire chain must be connected");
    expect(boundaryWire.value.closed, "face boundary wire chain must be closed");
    expect(boundaryWire.value.totalLength > 0.0,
           "boundary wire chain total length must be positive");
    expectValidBox(boundaryWire.value.bounds, "boundary wire chain bbox must be available");
    expect(!boundaryWire.value.orderedEdges.empty(),
           "boundary wire chain must expose ordered edge summaries");
    expect(distance(boundaryWire.value.startPoint, boundaryWire.value.endPoint) < 1.0e-5,
           "closed boundary wire chain endpoints must meet");

    const std::vector<tsrebar::LegacySelectionRef> singleWireEdges{edgeRefs.front()};
    const auto singleWire = adapter.buildWireChain(singleWireEdges);
    expect(singleWire.ok, singleWire.diagnostic.toUtf8().constData());
    expect(singleWire.value.inputEdgeCount == 1,
           "single edge wire chain must preserve input count");
    expect(singleWire.value.orderedEdgeCount == 1,
           "single edge wire chain must expose one ordered edge");
    expect(singleWire.value.connected, "single edge wire chain must be connected");
    expect(!singleWire.value.closed, "non-collapsed single edge wire chain must be open");
    expect(singleWire.value.totalLength == edgeResult.value.length,
           "single edge wire chain length must match edge length");
    expect(distance(singleWire.value.startPoint, edgeResult.value.startPoint) < 1.0e-6,
           "single edge wire chain start must match edge start");
    expect(distance(singleWire.value.endPoint, edgeResult.value.endPoint) < 1.0e-6,
           "single edge wire chain end must match edge end");

    std::vector<tsrebar::LegacySelectionRef> disconnectedWireEdges{edgeRefs.front()};
    for (qsizetype index = 1; index < edgeRefs.size(); ++index) {
        const auto candidate = adapter.edgeGeometry(edgeRefs.at(index));
        expect(candidate.ok, candidate.diagnostic.toUtf8().constData());
        const bool disjoint =
            distance(candidate.value.startPoint, edgeResult.value.startPoint) > 1.0 &&
            distance(candidate.value.startPoint, edgeResult.value.endPoint) > 1.0 &&
            distance(candidate.value.endPoint, edgeResult.value.startPoint) > 1.0 &&
            distance(candidate.value.endPoint, edgeResult.value.endPoint) > 1.0;
        if (disjoint) {
            disconnectedWireEdges.push_back(edgeRefs.at(index));
            break;
        }
    }
    expect(disconnectedWireEdges.size() == 2,
           "test STEP must provide a disconnected edge pair");
    const auto disconnectedWire = adapter.buildWireChain(disconnectedWireEdges);
    expect(!disconnectedWire.ok, "wire chain must reject disconnected edge refs");
    expect(disconnectedWire.diagnostic.contains(QStringLiteral("not connected")),
           "disconnected wire chain diagnostic must be stable");
    expect(!disconnectedWire.value.connected,
           "disconnected wire chain DTO must expose connected=false");
    expect(disconnectedWire.value.failureReason.find("not connected") != std::string::npos,
           "disconnected wire chain failure reason must be carried in DTO");

    const auto selfDistance = adapter.distanceBetween(edgeRefs.front(), edgeRefs.front());
    expect(selfDistance.ok, selfDistance.diagnostic.toUtf8().constData());
    expect(std::fabs(selfDistance.value.distance) < 1.0e-6,
           "edge self distance must be zero");
    expect(selfDistance.value.pointsValid, "distance result must expose closest points");
    expect(selfDistance.value.firstStableId == edgeRefs.front().stableId,
           "distance result must preserve first stable id");
    expect(selfDistance.value.secondStableId == edgeRefs.front().stableId,
           "distance result must preserve second stable id");

    const auto edgeFaceDistance = adapter.distanceBetween(edgeRefs.front(), faceRefs.front());
    expect(edgeFaceDistance.ok, edgeFaceDistance.diagnostic.toUtf8().constData());
    expect(edgeFaceDistance.value.distance >= 0.0, "edge-face distance must be non-negative");
    expect(edgeFaceDistance.value.pointsValid,
           "edge-face distance must expose closest point pair");

    const auto boundaryEdgeRef =
        tsrebar::parseStableSelectionString(QString::fromStdString(
            faceResult.value.boundaryLoops.front().edgeStableIds.front()));
    expect(boundaryEdgeRef.has_value(), "boundary edge stable id must be parseable");
    const auto edgeFaceIntersections =
        adapter.edgeFaceIntersections(*boundaryEdgeRef, faceRefs.front());
    expect(edgeFaceIntersections.ok,
           edgeFaceIntersections.diagnostic.toUtf8().constData());
    expect(edgeFaceIntersections.value.edgeStableId == boundaryEdgeRef->stableId,
           "intersection result must preserve edge stable id");
    expect(edgeFaceIntersections.value.faceStableId == faceRefs.front().stableId,
           "intersection result must preserve face stable id");
    expect(edgeFaceIntersections.value.overlap ||
               edgeFaceIntersections.value.points.size() >= 1,
           "boundary edge and owning face must intersect or overlap");
    expect(!edgeFaceIntersections.value.points.empty(),
           "boundary edge-face intersection must expose representative points");
    expect(edgeFaceIntersections.value.points.front().edgeParameterValid,
           "intersection point must carry edge parameter when projected");

    const double quarterParameter =
        edgeResult.value.firstParameter +
        (edgeResult.value.lastParameter - edgeResult.value.firstParameter) * 0.25;
    const double threeQuarterParameter =
        edgeResult.value.firstParameter +
        (edgeResult.value.lastParameter - edgeResult.value.firstParameter) * 0.75;
    const auto interval =
        adapter.edgeInterval(edgeRefs.front(), quarterParameter, threeQuarterParameter, 4);
    expect(interval.ok, interval.diagnostic.toUtf8().constData());
    expect(interval.value.sourceEdgeStableId == edgeRefs.front().stableId,
           "edge interval must preserve source edge stable id");
    expect(interval.value.firstParameter == quarterParameter,
           "edge interval must preserve first parameter");
    expect(interval.value.lastParameter == threeQuarterParameter,
           "edge interval must preserve last parameter");
    expect(interval.value.length > 0.0, "edge interval length must be positive");
    expect(interval.value.length < edgeResult.value.length,
           "edge interval length must be shorter than full edge");
    expect(interval.value.samplePoints.size() == 4,
           "edge interval sample count must match request");
    expectValidBox(interval.value.bounds, "edge interval bbox must be available");

    const auto reversedInterval =
        adapter.edgeInterval(edgeRefs.front(), threeQuarterParameter, quarterParameter, 2);
    expect(!reversedInterval.ok, "edge interval must reject reversed parameters");
    expect(reversedInterval.diagnostic.contains(QStringLiteral("ordered")),
           "edge interval reversed parameter diagnostic must be stable");

    const auto split = adapter.edgeSplitAtParameter(edgeRefs.front(), middleParameter, 3);
    expect(split.ok, split.diagnostic.toUtf8().constData());
    expect(split.value.sourceEdgeStableId == edgeRefs.front().stableId,
           "edge split must preserve source edge stable id");
    expect(split.value.splitParameter == middleParameter,
           "edge split must preserve split parameter");
    expect(distance(split.value.splitPoint, middlePoint.value) < 1.0e-6,
           "edge split point must match pointAtParameter");
    expect(split.value.firstInterval.length > 0.0,
           "edge split first interval length must be positive");
    expect(split.value.secondInterval.length > 0.0,
           "edge split second interval length must be positive");
    expect(split.value.firstInterval.lastParameter == middleParameter,
           "edge split first interval must end at split parameter");
    expect(split.value.secondInterval.firstParameter == middleParameter,
           "edge split second interval must start at split parameter");
    expect(split.value.firstInterval.samplePoints.size() == 3,
           "edge split first interval sample count must match request");
    expect(split.value.secondInterval.samplePoints.size() == 3,
           "edge split second interval sample count must match request");

    const auto pointSplit = adapter.edgeSplitAtPoint(edgeRefs.front(), middlePoint.value, 3);
    expect(pointSplit.ok, pointSplit.diagnostic.toUtf8().constData());
    expect(pointSplit.value.sourceEdgeStableId == edgeRefs.front().stableId,
           "edge split at point must preserve source edge stable id");
    expect(std::fabs(pointSplit.value.splitParameter - middleParameter) < 1.0e-6,
           "edge split at point must use projected parameter");
    expect(distance(pointSplit.value.splitPoint, middlePoint.value) < 1.0e-6,
           "edge split at point must use projected split point");
    expect(pointSplit.value.firstInterval.lastParameter == pointSplit.value.splitParameter,
           "edge split at point first interval must end at projected parameter");
    expect(pointSplit.value.secondInterval.firstParameter == pointSplit.value.splitParameter,
           "edge split at point second interval must start at projected parameter");

    const auto startTrim = adapter.edgeTrimEndpoint(edgeRefs.front(), -0.03, 0, 4);
    expect(startTrim.ok, startTrim.diagnostic.toUtf8().constData());
    expect(startTrim.value.sourceEdgeStableId == edgeRefs.front().stableId,
           "start trim must preserve source edge stable id");
    expect(startTrim.value.endFlag == 0, "start trim must preserve end flag");
    expect(std::fabs(startTrim.value.delta + 0.03) < 1.0e-12,
           "start trim must preserve legacy delta");
    expect(startTrim.value.interval.firstParameter > edgeResult.value.firstParameter,
           "start trim interval must move away from start parameter");
    expect(startTrim.value.interval.lastParameter == edgeResult.value.lastParameter,
           "start trim interval must keep original end parameter");
    expect(startTrim.value.interval.length > 0.0,
           "start trim interval length must be positive");
    expect(startTrim.value.interval.length < edgeResult.value.length,
           "start trim interval length must be shorter than original edge");
    expect(distance(startTrim.value.originalEndpoint, edgeResult.value.startPoint) < 1.0e-6,
           "start trim original endpoint must match edge start");
    expect(distance(startTrim.value.trimmedEndpoint, edgeResult.value.startPoint) > 0.0,
           "start trim endpoint must move inward");

    const auto endTrim = adapter.edgeTrimEndpoint(edgeRefs.front(), -0.03, 1, 4);
    expect(endTrim.ok, endTrim.diagnostic.toUtf8().constData());
    expect(endTrim.value.endFlag == 1, "end trim must preserve end flag");
    expect(endTrim.value.interval.firstParameter == edgeResult.value.firstParameter,
           "end trim interval must keep original start parameter");
    expect(endTrim.value.interval.lastParameter < edgeResult.value.lastParameter,
           "end trim interval must move away from end parameter");
    expect(endTrim.value.interval.length > 0.0,
           "end trim interval length must be positive");
    expect(endTrim.value.interval.length < edgeResult.value.length,
           "end trim interval length must be shorter than original edge");
    expect(distance(endTrim.value.originalEndpoint, edgeResult.value.endPoint) < 1.0e-6,
           "end trim original endpoint must match edge end");
    expect(distance(endTrim.value.trimmedEndpoint, edgeResult.value.endPoint) > 0.0,
           "end trim endpoint must move inward");

    const auto nearEndpointTrim = adapter.edgeTrimEndpoint(edgeRefs.front(), -1.0e-12, 0, 4);
    expect(!nearEndpointTrim.ok, "edge trim must reject near-endpoint no-op trim");
    expect(nearEndpointTrim.diagnostic.contains(QStringLiteral("too small")),
           "near-endpoint trim diagnostic must be stable");

    const auto tooShortTrim =
        adapter.edgeTrimEndpoint(edgeRefs.front(), -(edgeResult.value.length + 1.0), 0, 4);
    expect(!tooShortTrim.ok, "edge trim must reject over-trimming short intervals");
    expect(tooShortTrim.diagnostic.contains(QStringLiteral("minimum length")),
           "short edge trim diagnostic must be stable");

    const auto invalidEndFlagTrim = adapter.edgeTrimEndpoint(edgeRefs.front(), -0.03, 2, 4);
    expect(!invalidEndFlagTrim.ok, "edge trim must reject invalid end flag");
    expect(invalidEndFlagTrim.diagnostic.contains(QStringLiteral("endFlag")),
           "invalid end flag trim diagnostic must be stable");

    const auto endpointSplit =
        adapter.edgeSplitAtParameter(edgeRefs.front(), edgeResult.value.firstParameter, 3);
    expect(!endpointSplit.ok, "edge split must reject endpoint parameters");
    expect(endpointSplit.diagnostic.contains(QStringLiteral("inside edge interval")),
           "edge split endpoint diagnostic must be stable");

    const auto endpointPointSplit =
        adapter.edgeSplitAtPoint(edgeRefs.front(), edgeResult.value.startPoint, 3);
    expect(!endpointPointSplit.ok, "edge split at point must reject endpoint projections");
    expect(endpointPointSplit.diagnostic.contains(QStringLiteral("inside edge interval")),
           "edge split at point endpoint diagnostic must be stable");

    const tsrebar::LegacyPoint3d outsidePoint{
        edgeResult.value.startPoint.x + (edgeResult.value.startPoint.x - middlePoint.value.x) *
                                            10.0,
        edgeResult.value.startPoint.y + (edgeResult.value.startPoint.y - middlePoint.value.y) *
                                            10.0,
        edgeResult.value.startPoint.z + (edgeResult.value.startPoint.z - middlePoint.value.z) *
                                            10.0};
    const auto outsideProjection = adapter.edgeProjectPoint(edgeRefs.front(), outsidePoint);
    expect(outsideProjection.ok, outsideProjection.diagnostic.toUtf8().constData());
    expect(outsideProjection.value.parameterValid,
           "outside projection parameter must still be available");
    expect(!outsideProjection.value.inside,
           "outside projection must not be split-safe inside interval");
    expect(outsideProjection.value.distance > 1.0,
           "outside projection must report non-zero distance");

    const std::vector<tsrebar::LegacySelectionRef> singleEdgeGroup{edgeRefs.front()};
    const auto groupDistance =
        adapter.pointToEdgeGroupDistance(middlePoint.value, singleEdgeGroup, 0.002);
    expect(groupDistance.ok, groupDistance.diagnostic.toUtf8().constData());
    expect(groupDistance.value.inputPoint.x == middlePoint.value.x,
           "edge group distance must preserve input point");
    expect(groupDistance.value.threshold == 0.002,
           "edge group distance must preserve threshold");
    expect(groupDistance.value.hasNearest,
           "edge group distance must expose nearest candidate");
    expect(groupDistance.value.nearest.edgeStableId == edgeRefs.front().stableId,
           "edge group distance nearest edge stable id must match candidate");
    expect(groupDistance.value.nearest.edgeParameterValid,
           "edge group distance nearest candidate must carry edge parameter");
    expect(groupDistance.value.candidates.size() == 1,
           "single edge group distance must expose one candidate");
    expect(groupDistance.value.minDistance < 1.0e-6,
           "point on edge must have near-zero group distance");
    expect(groupDistance.value.tooClose,
           "point on edge must hit 0.002 legacy creation threshold");
    expect(distance(groupDistance.value.nearest.pointOnEdge, middlePoint.value) < 1.0e-6,
           "edge group nearest point must match point on edge");

    std::vector<tsrebar::LegacySelectionRef> multiEdgeGroup{edgeRefs.front()};
    if (edgeRefs.size() > 1) {
        multiEdgeGroup.push_back(edgeRefs.at(1));
    }
    const auto multiGroupDistance =
        adapter.pointToEdgeGroupDistance(middlePoint.value, multiEdgeGroup, 0.002);
    expect(multiGroupDistance.ok, multiGroupDistance.diagnostic.toUtf8().constData());
    expect(multiGroupDistance.value.candidates.size() == multiEdgeGroup.size(),
           "multi edge group distance must keep every valid candidate summary");
    expect(multiGroupDistance.value.nearest.edgeStableId == edgeRefs.front().stableId,
           "multi edge group distance must choose the nearest edge stable id");
    expect(multiGroupDistance.value.minDistance < 1.0e-6,
           "multi edge group nearest distance must be near zero for source edge");

    const auto outsideGroupDistance =
        adapter.pointToEdgeGroupDistance(outsidePoint, singleEdgeGroup, 0.002);
    expect(outsideGroupDistance.ok, outsideGroupDistance.diagnostic.toUtf8().constData());
    expect(outsideGroupDistance.value.minDistance > 1.0,
           "outside point must be farther than creation threshold");
    expect(!outsideGroupDistance.value.tooClose,
           "outside point must not hit 0.002 legacy creation threshold");

    const std::vector<tsrebar::LegacySelectionRef> emptyGroup;
    const auto emptyGroupDistance =
        adapter.pointToEdgeGroupDistance(middlePoint.value, emptyGroup, 0.002);
    expect(!emptyGroupDistance.ok, "edge group distance must reject empty groups");
    expect(emptyGroupDistance.diagnostic.contains(QStringLiteral("empty")),
           "empty edge group distance diagnostic must be stable");

    const auto wrongType = adapter.edgeGeometry(faceRefs.front());
    expect(!wrongType.ok, "edgeGeometry must reject face refs");
    expect(wrongType.diagnostic.contains(QStringLiteral("expected an edge ref")),
           "wrong type diagnostic must be stable");

    const auto wrongTypeProjection = adapter.edgeProjectPoint(faceRefs.front(), middlePoint.value);
    expect(!wrongTypeProjection.ok, "edgeProjectPoint must reject face refs");
    expect(wrongTypeProjection.diagnostic.contains(QStringLiteral("expected an edge ref")),
           "edgeProjectPoint wrong type diagnostic must be stable");

    const std::vector<tsrebar::LegacySelectionRef> wrongTypeGroup{faceRefs.front()};
    const auto wrongTypeGroupDistance =
        adapter.pointToEdgeGroupDistance(middlePoint.value, wrongTypeGroup, 0.002);
    expect(!wrongTypeGroupDistance.ok, "edge group distance must reject face refs");
    expect(wrongTypeGroupDistance.diagnostic.contains(QStringLiteral("expected an edge ref")),
           "edge group distance wrong type diagnostic must be stable");

    const auto wrongTypeWire = adapter.buildWireChain(wrongTypeGroup);
    expect(!wrongTypeWire.ok, "wire chain must reject face refs");
    expect(wrongTypeWire.diagnostic.contains(QStringLiteral("expected an edge ref")),
           "wire chain wrong type diagnostic must be stable");

    const tsrebar::LegacySelectionRef missingPart =
        tsrebar::makeLegacySelectionRef(QStringLiteral("missing-part"),
                                        false,
                                        0,
                                        tsrebar::LegacyShapeKind::Edge,
                                        1);
    const auto missingPartResult = adapter.edgeGeometry(missingPart);
    expect(!missingPartResult.ok, "adapter must reject missing part refs");
    expect(missingPartResult.diagnostic.contains(QStringLiteral("not in current document")),
           "missing part diagnostic must be stable");

    const auto missingPartProjection = adapter.edgeProjectPoint(missingPart, middlePoint.value);
    expect(!missingPartProjection.ok, "edgeProjectPoint must reject missing part refs");
    expect(missingPartProjection.diagnostic.contains(QStringLiteral("not in current document")),
           "edgeProjectPoint missing part diagnostic must be stable");

    const std::vector<tsrebar::LegacySelectionRef> missingPartGroup{missingPart};
    const auto missingPartGroupDistance =
        adapter.pointToEdgeGroupDistance(middlePoint.value, missingPartGroup, 0.002);
    expect(!missingPartGroupDistance.ok, "edge group distance must reject missing part refs");
    expect(missingPartGroupDistance.diagnostic.contains(QStringLiteral("not in current document")),
           "edge group distance missing part diagnostic must be stable");

    const auto missingPartWire = adapter.buildWireChain(missingPartGroup);
    expect(!missingPartWire.ok, "wire chain must reject missing part refs");
    expect(missingPartWire.diagnostic.contains(QStringLiteral("not in current document")),
           "wire chain missing part diagnostic must be stable");

    const tsrebar::LegacySelectionRef outOfRange =
        tsrebar::makeLegacySelectionRef(QString::fromStdString(edgeRefs.front().partEntry),
                                        false,
                                        0,
                                        tsrebar::LegacyShapeKind::Edge,
                                        999999);
    const auto outOfRangeResult = adapter.edgeGeometry(outOfRange);
    expect(!outOfRangeResult.ok, "adapter must reject out-of-range subshape refs");
    expect(outOfRangeResult.diagnostic.contains(QStringLiteral("out of range")),
           "out-of-range diagnostic must be stable");

    return 0;
}
