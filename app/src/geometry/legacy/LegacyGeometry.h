#pragma once

#include <QString>

#include <string>
#include <vector>

namespace tsrebar {

struct LegacyPoint3d
{
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
};

struct LegacyVector3d
{
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
    bool valid = false;
};

struct LegacyBoundingBox
{
    LegacyPoint3d minPoint;
    LegacyPoint3d maxPoint;
    bool valid = false;
};

struct LegacyBoundaryLoop
{
    int ordinal = 0;
    bool outer = false;
    int edgeCount = 0;
    double perimeter = 0.0;
    LegacyBoundingBox bounds;
    std::vector<std::string> edgeStableIds;
};

struct LegacyEdgeDifferential
{
    std::string stableId;
    double parameter = 0.0;
    LegacyPoint3d point;
    LegacyVector3d tangent;
};

struct LegacyDistanceMeasure
{
    std::string firstStableId;
    std::string secondStableId;
    double distance = 0.0;
    LegacyPoint3d pointOnFirst;
    LegacyPoint3d pointOnSecond;
    bool pointsValid = false;
};

struct LegacyIntersectionPoint
{
    LegacyPoint3d point;
    double edgeParameter = 0.0;
    bool edgeParameterValid = false;
};

struct LegacyIntersectionSet
{
    std::string edgeStableId;
    std::string faceStableId;
    std::vector<LegacyIntersectionPoint> points;
    bool overlap = false;
};

struct LegacyCurveInterval
{
    std::string sourceEdgeStableId;
    double firstParameter = 0.0;
    double lastParameter = 0.0;
    double length = 0.0;
    LegacyPoint3d startPoint;
    LegacyPoint3d endPoint;
    LegacyBoundingBox bounds;
    std::vector<LegacyPoint3d> samplePoints;
};

struct LegacyCurveSplit
{
    std::string sourceEdgeStableId;
    double splitParameter = 0.0;
    LegacyPoint3d splitPoint;
    LegacyCurveInterval firstInterval;
    LegacyCurveInterval secondInterval;
};

struct LegacyEdgePointProjection
{
    std::string edgeStableId;
    LegacyPoint3d inputPoint;
    LegacyPoint3d projectedPoint;
    double parameter = 0.0;
    double distance = 0.0;
    bool parameterValid = false;
    bool inside = false;
};

struct LegacyEdgeEndpointTrim
{
    std::string sourceEdgeStableId;
    double delta = 0.0;
    int endFlag = 0;
    double trimDistance = 0.0;
    double originalParameter = 0.0;
    double trimmedParameter = 0.0;
    LegacyPoint3d originalEndpoint;
    LegacyPoint3d trimmedEndpoint;
    LegacyCurveInterval interval;
};

struct LegacyEdgeGroupDistanceCandidate
{
    std::string edgeStableId;
    double distance = 0.0;
    LegacyPoint3d pointOnEdge;
    double edgeParameter = 0.0;
    bool edgeParameterValid = false;
    bool inside = false;
};

struct LegacyEdgeGroupDistance
{
    LegacyPoint3d inputPoint;
    double threshold = 0.0;
    double minDistance = 0.0;
    bool tooClose = false;
    bool hasNearest = false;
    LegacyEdgeGroupDistanceCandidate nearest;
    std::vector<LegacyEdgeGroupDistanceCandidate> candidates;
};

enum class LegacyCurveKind {
    Unknown,
    Line,
    Circle,
    Ellipse,
    Bezier,
    BSpline,
    Other
};

struct LegacySplineCurveBuild
{
    std::vector<LegacyPoint3d> inputPoints;
    int inputPointCount = 0;
    int requestedSampleCount = 0;
    int effectiveSampleCount = 0;
    int legacySuggestedSampleCount = 0;
    double sourcePolylineLength = 0.0;
    double length = 0.0;
    LegacyCurveKind curveKind = LegacyCurveKind::Unknown;
    LegacyBoundingBox bounds;
    std::vector<LegacyPoint3d> samplePoints;
    bool buildable = false;
    std::string failureReason;
};

struct LegacyWireChainEdge
{
    std::string edgeStableId;
    int inputOrdinal = 0;
    double length = 0.0;
    LegacyPoint3d startPoint;
    LegacyPoint3d endPoint;
    bool reversed = false;
};

struct LegacyWireChain
{
    std::vector<std::string> inputEdgeStableIds;
    std::vector<LegacyWireChainEdge> orderedEdges;
    int inputEdgeCount = 0;
    int orderedEdgeCount = 0;
    double totalLength = 0.0;
    LegacyPoint3d startPoint;
    LegacyPoint3d endPoint;
    LegacyBoundingBox bounds;
    bool connected = false;
    bool closed = false;
    std::string failureReason;
};

enum class LegacySurfaceKind {
    Unknown,
    Plane,
    Cylinder,
    Cone,
    Sphere,
    Torus,
    Bezier,
    BSpline,
    Other
};

struct LegacyEdgeGeometry
{
    std::string stableId;
    LegacyCurveKind curveKind = LegacyCurveKind::Unknown;
    double length = 0.0;
    double firstParameter = 0.0;
    double lastParameter = 0.0;
    LegacyPoint3d startPoint;
    LegacyPoint3d endPoint;
    LegacyBoundingBox bounds;
    std::string fingerprint;
    bool closed = false;
};

struct LegacyFaceGeometry
{
    std::string stableId;
    LegacySurfaceKind surfaceKind = LegacySurfaceKind::Unknown;
    double area = 0.0;
    LegacyPoint3d samplePoint;
    LegacyVector3d normal;
    LegacyBoundingBox bounds;
    std::string fingerprint;
    std::vector<LegacyBoundaryLoop> boundaryLoops;
    int boundaryEdgeCount = 0;
};

template <typename TValue>
struct LegacyGeometryQueryResult
{
    bool ok = false;
    TValue value;
    QString diagnostic;
};

} // namespace tsrebar
