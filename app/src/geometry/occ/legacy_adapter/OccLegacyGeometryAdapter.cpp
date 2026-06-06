#include "geometry/occ/legacy_adapter/OccLegacyGeometryAdapter.h"

#include <BRepAlgoAPI_Section.hxx>
#include <BRepBndLib.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepGProp.hxx>
#include <BRepOffsetAPI_MakePipe.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <Bnd_Box.hxx>
#include <Extrema_ExtPC.hxx>
#include <Extrema_POnCurv.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <GC_MakeCircle.hxx>
#include <GProp_GProps.hxx>
#include <GeomAPI_PointsToBSpline.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_OffsetCurve.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <Precision.hxx>
#include <Standard_Failure.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

#include <algorithm>
#include <cmath>
#include <deque>
#include <iomanip>
#include <limits>
#include <sstream>
#include <utility>
#include <vector>

namespace {

tsrebar::LegacyPoint3d toLegacyPoint(const gp_Pnt& point)
{
    return {point.X(), point.Y(), point.Z()};
}

tsrebar::LegacyVector3d toLegacyVector(const gp_Vec& vector, bool valid)
{
    return {vector.X(), vector.Y(), vector.Z(), valid};
}

tsrebar::LegacyCurveKind toLegacyCurveKind(GeomAbs_CurveType type)
{
    switch (type) {
    case GeomAbs_Line:
        return tsrebar::LegacyCurveKind::Line;
    case GeomAbs_Circle:
        return tsrebar::LegacyCurveKind::Circle;
    case GeomAbs_Ellipse:
        return tsrebar::LegacyCurveKind::Ellipse;
    case GeomAbs_BezierCurve:
        return tsrebar::LegacyCurveKind::Bezier;
    case GeomAbs_BSplineCurve:
        return tsrebar::LegacyCurveKind::BSpline;
    default:
        return tsrebar::LegacyCurveKind::Other;
    }
}

tsrebar::LegacySurfaceKind toLegacySurfaceKind(GeomAbs_SurfaceType type)
{
    switch (type) {
    case GeomAbs_Plane:
        return tsrebar::LegacySurfaceKind::Plane;
    case GeomAbs_Cylinder:
        return tsrebar::LegacySurfaceKind::Cylinder;
    case GeomAbs_Cone:
        return tsrebar::LegacySurfaceKind::Cone;
    case GeomAbs_Sphere:
        return tsrebar::LegacySurfaceKind::Sphere;
    case GeomAbs_Torus:
        return tsrebar::LegacySurfaceKind::Torus;
    case GeomAbs_BezierSurface:
        return tsrebar::LegacySurfaceKind::Bezier;
    case GeomAbs_BSplineSurface:
        return tsrebar::LegacySurfaceKind::BSpline;
    default:
        return tsrebar::LegacySurfaceKind::Other;
    }
}

double midpoint(double first, double last)
{
    if (!std::isfinite(first) || !std::isfinite(last)) {
        return 0.0;
    }
    return first + (last - first) * 0.5;
}

int countBoundaryEdges(const TopoDS_Face& face)
{
    TopTools_IndexedMapOfShape edges;
    TopExp::MapShapes(face, TopAbs_EDGE, edges);
    return edges.Extent();
}

double squaredDistance(const gp_Pnt& lhs, const gp_Pnt& rhs)
{
    const double dx = lhs.X() - rhs.X();
    const double dy = lhs.Y() - rhs.Y();
    const double dz = lhs.Z() - rhs.Z();
    return dx * dx + dy * dy + dz * dz;
}

bool pointsClose(const gp_Pnt& lhs, const gp_Pnt& rhs, double tolerance)
{
    return squaredDistance(lhs, rhs) <= tolerance * tolerance;
}

bool pointFinite(const tsrebar::LegacyPoint3d& point)
{
    return std::isfinite(point.x) && std::isfinite(point.y) && std::isfinite(point.z);
}

bool vectorFinite(const tsrebar::LegacyVector3d& vector)
{
    return std::isfinite(vector.x) && std::isfinite(vector.y) &&
           std::isfinite(vector.z);
}

double vectorMagnitude(const tsrebar::LegacyVector3d& vector)
{
    return std::sqrt(vector.x * vector.x +
                     vector.y * vector.y +
                     vector.z * vector.z);
}

gp_Pnt toGpPoint(const tsrebar::LegacyPoint3d& point)
{
    return gp_Pnt(point.x, point.y, point.z);
}

gp_Vec toGpVec(const tsrebar::LegacyVector3d& vector)
{
    return gp_Vec(vector.x, vector.y, vector.z);
}

bool planeValid(const tsrebar::LegacyPlane& plane)
{
    return pointFinite(plane.origin) && plane.normal.valid &&
           vectorFinite(plane.normal) &&
           vectorMagnitude(plane.normal) > Precision::Confusion();
}

double signedDistanceToPlane(const tsrebar::LegacyPlane& plane,
                             const tsrebar::LegacyPoint3d& point)
{
    const double magnitude = vectorMagnitude(plane.normal);
    if (magnitude <= Precision::Confusion()) {
        return std::numeric_limits<double>::infinity();
    }

    const double dx = point.x - plane.origin.x;
    const double dy = point.y - plane.origin.y;
    const double dz = point.z - plane.origin.z;
    return (dx * plane.normal.x + dy * plane.normal.y + dz * plane.normal.z) /
           magnitude;
}

double splitGuard(double first, double last)
{
    return std::max((last - first) * 0.001, Precision::Confusion());
}

bool parameterInsideSplitInterval(double parameter, double first, double last)
{
    const double guard = splitGuard(first, last);
    return parameter > first + guard && parameter < last - guard;
}

double curveLength(BRepAdaptor_Curve& curve, double first, double last)
{
    if (!std::isfinite(first) || !std::isfinite(last) || first >= last) {
        return 0.0;
    }
    double length = GCPnts_AbscissaPoint::Length(
        curve,
        first,
        last,
        Precision::Confusion());
    if (!std::isfinite(length) || length < 0.0) {
        length = 0.0;
    }
    return length;
}

double parameterAtLengthFromStart(BRepAdaptor_Curve& curve,
                                  double first,
                                  double last,
                                  double targetLength)
{
    double low = first;
    double high = last;
    for (int iteration = 0; iteration < 64; ++iteration) {
        const double mid = midpoint(low, high);
        const double midLength = curveLength(curve, first, mid);
        if (midLength < targetLength) {
            low = mid;
        } else {
            high = mid;
        }
    }
    return midpoint(low, high);
}

tsrebar::LegacyBoundingBox makeBounds(const TopoDS_Shape& shape)
{
    tsrebar::LegacyBoundingBox bounds;
    if (shape.IsNull()) {
        return bounds;
    }

    Bnd_Box box;
    box.SetGap(0.0);
    BRepBndLib::AddOptimal(shape, box, Standard_False, Standard_False);
    if (box.IsVoid()) {
        BRepBndLib::Add(shape, box);
    }
    if (box.IsVoid()) {
        return bounds;
    }

    Standard_Real xmin = 0.0;
    Standard_Real ymin = 0.0;
    Standard_Real zmin = 0.0;
    Standard_Real xmax = 0.0;
    Standard_Real ymax = 0.0;
    Standard_Real zmax = 0.0;
    box.Get(xmin, ymin, zmin, xmax, ymax, zmax);
    if (!std::isfinite(xmin) || !std::isfinite(ymin) || !std::isfinite(zmin) ||
        !std::isfinite(xmax) || !std::isfinite(ymax) || !std::isfinite(zmax)) {
        return bounds;
    }

    bounds.minPoint = {xmin, ymin, zmin};
    bounds.maxPoint = {xmax, ymax, zmax};
    bounds.valid = true;
    return bounds;
}

tsrebar::LegacyBoundingBox makeBounds(const std::vector<tsrebar::LegacyPoint3d>& points)
{
    tsrebar::LegacyBoundingBox bounds;
    if (points.empty()) {
        return bounds;
    }

    bounds.minPoint = points.front();
    bounds.maxPoint = points.front();
    for (const tsrebar::LegacyPoint3d& point : points) {
        bounds.minPoint.x = std::min(bounds.minPoint.x, point.x);
        bounds.minPoint.y = std::min(bounds.minPoint.y, point.y);
        bounds.minPoint.z = std::min(bounds.minPoint.z, point.z);
        bounds.maxPoint.x = std::max(bounds.maxPoint.x, point.x);
        bounds.maxPoint.y = std::max(bounds.maxPoint.y, point.y);
        bounds.maxPoint.z = std::max(bounds.maxPoint.z, point.z);
    }
    bounds.valid = true;
    return bounds;
}

void mergeBounds(tsrebar::LegacyBoundingBox* target,
                 const tsrebar::LegacyBoundingBox& source)
{
    if (target == nullptr || !source.valid) {
        return;
    }
    if (!target->valid) {
        *target = source;
        return;
    }

    target->minPoint.x = std::min(target->minPoint.x, source.minPoint.x);
    target->minPoint.y = std::min(target->minPoint.y, source.minPoint.y);
    target->minPoint.z = std::min(target->minPoint.z, source.minPoint.z);
    target->maxPoint.x = std::max(target->maxPoint.x, source.maxPoint.x);
    target->maxPoint.y = std::max(target->maxPoint.y, source.maxPoint.y);
    target->maxPoint.z = std::max(target->maxPoint.z, source.maxPoint.z);
}

double legacyPointSquaredDistance(const tsrebar::LegacyPoint3d& lhs,
                                  const tsrebar::LegacyPoint3d& rhs)
{
    const double dx = lhs.x - rhs.x;
    const double dy = lhs.y - rhs.y;
    const double dz = lhs.z - rhs.z;
    return dx * dx + dy * dy + dz * dz;
}

bool legacyPointsClose(const tsrebar::LegacyPoint3d& lhs,
                       const tsrebar::LegacyPoint3d& rhs,
                       double tolerance)
{
    return legacyPointSquaredDistance(lhs, rhs) <= tolerance * tolerance;
}

double polylineLength(const std::vector<tsrebar::LegacyPoint3d>& points)
{
    if (points.size() < 2) {
        return 0.0;
    }

    double length = 0.0;
    for (std::size_t index = 1; index < points.size(); ++index) {
        const double dx = points[index].x - points[index - 1].x;
        const double dy = points[index].y - points[index - 1].y;
        const double dz = points[index].z - points[index - 1].z;
        length += std::sqrt(dx * dx + dy * dy + dz * dz);
    }
    return length;
}

int legacySplineSuggestedSampleCount(double length)
{
    if (!std::isfinite(length) || length <= 0.0) {
        return 5;
    }
    const double legacyCount = std::ceil(length * 50.0);
    if (!std::isfinite(legacyCount) || legacyCount < 5.0) {
        return 5;
    }
    if (legacyCount > static_cast<double>(std::numeric_limits<int>::max())) {
        return std::numeric_limits<int>::max();
    }
    return static_cast<int>(legacyCount);
}

bool hasCollapsedConsecutivePoints(const std::vector<tsrebar::LegacyPoint3d>& points)
{
    for (std::size_t index = 1; index < points.size(); ++index) {
        const gp_Pnt previous(points[index - 1].x,
                              points[index - 1].y,
                              points[index - 1].z);
        const gp_Pnt current(points[index].x, points[index].y, points[index].z);
        if (pointsClose(previous, current, Precision::Confusion())) {
            return true;
        }
    }
    return false;
}

bool makeStableOffsetNormal(BRepAdaptor_Curve& curve,
                            double parameter,
                            gp_Dir* normal)
{
    if (normal == nullptr) {
        return false;
    }

    gp_Pnt point;
    gp_Vec tangent;
    curve.D1(parameter, point, tangent);
    if (tangent.Magnitude() <= Precision::Confusion()) {
        return false;
    }
    tangent.Normalize();

    const std::vector<gp_Vec> candidates{
        gp_Vec(0.0, 0.0, 1.0),
        gp_Vec(0.0, 1.0, 0.0),
        gp_Vec(1.0, 0.0, 0.0),
    };
    for (const gp_Vec& candidate : candidates) {
        gp_Vec offsetNormal = tangent.Crossed(candidate);
        if (offsetNormal.Magnitude() > Precision::Confusion()) {
            offsetNormal.Normalize();
            *normal = gp_Dir(offsetNormal);
            return true;
        }
    }
    return false;
}

int countSubShapes(const TopoDS_Shape& shape, TopAbs_ShapeEnum shapeType)
{
    TopTools_IndexedMapOfShape subShapes;
    TopExp::MapShapes(shape, shapeType, subShapes);
    return subShapes.Extent();
}

bool makeSweepProfileFrame(BRepAdaptor_Curve& curve,
                           double parameter,
                           gp_Ax2* frame)
{
    if (frame == nullptr) {
        return false;
    }

    gp_Pnt point;
    gp_Vec firstDerivative;
    curve.D1(parameter, point, firstDerivative);
    if (firstDerivative.Magnitude() <= Precision::Confusion()) {
        return false;
    }

    const gp_Dir pathDirection(firstDerivative);
    gp_Dir candidateX(1.0, 0.0, 0.0);
    if (std::abs(pathDirection.Dot(candidateX)) > 0.95) {
        candidateX = gp_Dir(0.0, 1.0, 0.0);
    }

    gp_Vec yVector(pathDirection);
    yVector.Cross(gp_Vec(candidateX));
    if (yVector.Magnitude() <= Precision::Confusion()) {
        return false;
    }
    const gp_Dir yDirection(yVector);

    gp_Vec xVector(yDirection);
    xVector.Cross(gp_Vec(pathDirection));
    if (xVector.Magnitude() <= Precision::Confusion()) {
        return false;
    }
    const gp_Dir xDirection(xVector);

    *frame = gp_Ax2(point, pathDirection, xDirection);
    return true;
}

tsrebar::LegacySectionEdge sampleSectionEdge(const TopoDS_Edge& edge,
                                             int ordinal,
                                             std::string sourceEdgeStableId,
                                             int sampleCount)
{
    tsrebar::LegacySectionEdge sectionEdge;
    sectionEdge.ordinal = ordinal;
    sectionEdge.sourceEdgeStableId = std::move(sourceEdgeStableId);
    if (edge.IsNull() || sampleCount <= 0) {
        return sectionEdge;
    }

    BRepAdaptor_Curve curve(edge);
    const double first = curve.FirstParameter();
    const double last = curve.LastParameter();
    if (!std::isfinite(first) || !std::isfinite(last) || first >= last) {
        return sectionEdge;
    }

    sectionEdge.samplePoints.reserve(static_cast<std::size_t>(sampleCount));
    for (int index = 0; index < sampleCount; ++index) {
        const double ratio =
            sampleCount == 1
                ? 0.5
                : static_cast<double>(index) / static_cast<double>(sampleCount - 1);
        const double parameter = first + (last - first) * ratio;
        sectionEdge.samplePoints.push_back(toLegacyPoint(curve.Value(parameter)));
    }

    sectionEdge.length = curveLength(curve, first, last);
    if (!std::isfinite(sectionEdge.length) ||
        sectionEdge.length <= Precision::Confusion()) {
        sectionEdge.length = polylineLength(sectionEdge.samplePoints);
    }
    sectionEdge.bounds = makeBounds(edge);
    if (!sectionEdge.bounds.valid) {
        sectionEdge.bounds = makeBounds(sectionEdge.samplePoints);
    }
    return sectionEdge;
}

bool edgeSamplesOnPlane(const TopoDS_Edge& edge,
                        const tsrebar::LegacyPlane& plane,
                        int sampleCount)
{
    const tsrebar::LegacySectionEdge sectionEdge =
        sampleSectionEdge(edge, 0, std::string(), sampleCount);
    if (sectionEdge.samplePoints.empty() ||
        sectionEdge.length <= Precision::Confusion()) {
        return false;
    }

    constexpr double tolerance = 1.0e-5;
    for (const tsrebar::LegacyPoint3d& point : sectionEdge.samplePoints) {
        if (std::abs(signedDistanceToPlane(plane, point)) > tolerance) {
            return false;
        }
    }
    return true;
}

void appendPoint(std::ostringstream& output, const tsrebar::LegacyPoint3d& point)
{
    output << point.x << ',' << point.y << ',' << point.z;
}

void appendBounds(std::ostringstream& output, const tsrebar::LegacyBoundingBox& bounds)
{
    output << "bbox=";
    if (!bounds.valid) {
        output << "invalid";
        return;
    }
    appendPoint(output, bounds.minPoint);
    output << "->";
    appendPoint(output, bounds.maxPoint);
}

std::string makeEdgeFingerprint(const tsrebar::LegacyEdgeGeometry& geometry)
{
    std::ostringstream output;
    output << std::fixed << std::setprecision(9);
    output << "edge-fingerprint-v1"
           << "|id=" << geometry.stableId
           << "|kind=" << static_cast<int>(geometry.curveKind)
           << "|length=" << geometry.length
           << "|params=" << geometry.firstParameter << ',' << geometry.lastParameter << '|';
    appendBounds(output, geometry.bounds);
    output << "|closed=" << (geometry.closed ? 1 : 0);
    return output.str();
}

std::string makeFaceFingerprint(const tsrebar::LegacyFaceGeometry& geometry)
{
    std::ostringstream output;
    output << std::fixed << std::setprecision(9);
    output << "face-fingerprint-v1"
           << "|id=" << geometry.stableId
           << "|kind=" << static_cast<int>(geometry.surfaceKind)
           << "|area=" << geometry.area
           << "|edges=" << geometry.boundaryEdgeCount
           << "|loops=" << geometry.boundaryLoops.size() << '|';
    appendBounds(output, geometry.bounds);
    return output.str();
}

double projectParameterOnEdge(const TopoDS_Edge& edge, const gp_Pnt& point, bool* valid)
{
    if (valid != nullptr) {
        *valid = false;
    }
    if (edge.IsNull()) {
        return 0.0;
    }

    BRepAdaptor_Curve curve(edge);
    const double first = curve.FirstParameter();
    const double last = curve.LastParameter();
    if (!std::isfinite(first) || !std::isfinite(last)) {
        return 0.0;
    }

    double bestParameter = first;
    double bestSquareDistance = squaredDistance(curve.Value(first), point);

    const double lastSquareDistance = squaredDistance(curve.Value(last), point);
    if (lastSquareDistance < bestSquareDistance) {
        bestSquareDistance = lastSquareDistance;
        bestParameter = last;
    }

    Extrema_ExtPC extrema(point, curve, first, last, 1.0e-10);
    if (extrema.IsDone()) {
        for (int index = 1; index <= extrema.NbExt(); ++index) {
            const double squareDistance = extrema.SquareDistance(index);
            if (squareDistance < bestSquareDistance) {
                bestSquareDistance = squareDistance;
                bestParameter = extrema.Point(index).Parameter();
            }
        }
    }

    bestParameter = std::clamp(bestParameter, first, last);
    if (valid != nullptr) {
        *valid = std::isfinite(bestParameter);
    }
    return bestParameter;
}

void appendUniqueIntersectionPoint(tsrebar::LegacyIntersectionSet* intersections,
                                   const gp_Pnt& point,
                                   double edgeParameter,
                                   bool edgeParameterValid)
{
    if (intersections == nullptr) {
        return;
    }

    constexpr double duplicateTolerance = 1.0e-7;
    for (const tsrebar::LegacyIntersectionPoint& existing : intersections->points) {
        const gp_Pnt existingPoint(existing.point.x, existing.point.y, existing.point.z);
        if (pointsClose(existingPoint, point, duplicateTolerance)) {
            return;
        }
    }

    tsrebar::LegacyIntersectionPoint intersectionPoint;
    intersectionPoint.point = toLegacyPoint(point);
    intersectionPoint.edgeParameter = edgeParameter;
    intersectionPoint.edgeParameterValid = edgeParameterValid;
    intersections->points.push_back(intersectionPoint);
}

std::vector<tsrebar::LegacyBoundaryLoop> collectBoundaryLoops(
    const TopoDS_Face& face,
    const tsrebar::OccSelectionIndex& selectionIndex)
{
    std::vector<tsrebar::LegacyBoundaryLoop> loops;
    if (face.IsNull()) {
        return loops;
    }

    TopoDS_Wire outerWire;
    try {
        outerWire = BRepTools::OuterWire(face);
    } catch (...) {
        outerWire.Nullify();
    }

    for (TopExp_Explorer explorer(face, TopAbs_WIRE); explorer.More(); explorer.Next()) {
        const TopoDS_Wire wire = TopoDS::Wire(explorer.Current());
        if (wire.IsNull()) {
            continue;
        }

        TopTools_IndexedMapOfShape edges;
        TopExp::MapShapes(wire, TopAbs_EDGE, edges);

        GProp_GProps props;
        BRepGProp::LinearProperties(wire, props, Standard_False, Standard_False);

        tsrebar::LegacyBoundaryLoop loop;
        loop.outer = !outerWire.IsNull() && wire.IsSame(outerWire);
        loop.edgeCount = edges.Extent();
        loop.perimeter = props.Mass();
        loop.bounds = makeBounds(wire);
        loop.edgeStableIds.reserve(static_cast<std::size_t>(edges.Extent()));
        for (int edgeIndex = 1; edgeIndex <= edges.Extent(); ++edgeIndex) {
            const std::optional<tsrebar::LegacySelectionRef> edgeRef =
                selectionIndex.refForShape(edges.FindKey(edgeIndex));
            if (edgeRef.has_value()) {
                loop.edgeStableIds.push_back(edgeRef->stableId);
            }
        }
        loops.push_back(loop);
    }

    const auto outerIt = std::find_if(loops.begin(), loops.end(), [](const auto& loop) {
        return loop.outer;
    });
    if (outerIt != loops.end() && outerIt != loops.begin()) {
        std::iter_swap(loops.begin(), outerIt);
    }
    if (!loops.empty() && outerIt == loops.end()) {
        loops.front().outer = true;
    }
    for (std::size_t index = 0; index < loops.size(); ++index) {
        loops[index].ordinal = static_cast<int>(index + 1);
    }

    return loops;
}

} // namespace

namespace tsrebar {

OccLegacyGeometryAdapter::OccLegacyGeometryAdapter(OccSelectionIndex selectionIndex)
    : m_selectionIndex(std::move(selectionIndex))
{
}

LegacyGeometryQueryResult<LegacyEdgeGeometry> OccLegacyGeometryAdapter::edgeGeometry(
    const LegacySelectionRef& ref) const
{
    LegacyGeometryQueryResult<LegacyEdgeGeometry> result;
    if (ref.shapeKind != LegacyShapeKind::Edge) {
        result.diagnostic = QStringLiteral("LegacyGeometryAdapter expected an edge ref.");
        return result;
    }

    try {
        const OccSelectionResolveResult resolved = m_selectionIndex.resolve(ref);
        if (!resolved.found) {
            result.diagnostic = resolved.diagnostic;
            return result;
        }

        const TopoDS_Edge edge = TopoDS::Edge(resolved.shape);
        if (edge.IsNull()) {
            result.diagnostic = QStringLiteral("SelectionRef did not resolve to an edge.");
            return result;
        }

        BRepAdaptor_Curve curve(edge);
        GProp_GProps props;
        BRepGProp::LinearProperties(edge, props, Standard_False, Standard_False);

        LegacyEdgeGeometry geometry;
        geometry.stableId = ref.stableId;
        geometry.curveKind = toLegacyCurveKind(curve.GetType());
        geometry.length = props.Mass();
        geometry.firstParameter = curve.FirstParameter();
        geometry.lastParameter = curve.LastParameter();
        geometry.startPoint = toLegacyPoint(curve.Value(geometry.firstParameter));
        geometry.endPoint = toLegacyPoint(curve.Value(geometry.lastParameter));
        geometry.bounds = makeBounds(edge);
        geometry.closed = curve.IsClosed();
        geometry.fingerprint = makeEdgeFingerprint(geometry);

        result.value = geometry;
        result.ok = true;
        return result;
    } catch (const Standard_Failure& failure) {
        result.diagnostic = QStringLiteral("OCCT edge query failed: %1")
                                .arg(QString::fromUtf8(failure.GetMessageString()));
    } catch (...) {
        result.diagnostic = QStringLiteral("OCCT edge query failed: unknown exception.");
    }
    return result;
}

LegacyGeometryQueryResult<LegacyFaceGeometry> OccLegacyGeometryAdapter::faceGeometry(
    const LegacySelectionRef& ref) const
{
    LegacyGeometryQueryResult<LegacyFaceGeometry> result;
    if (ref.shapeKind != LegacyShapeKind::Face) {
        result.diagnostic = QStringLiteral("LegacyGeometryAdapter expected a face ref.");
        return result;
    }

    try {
        const OccSelectionResolveResult resolved = m_selectionIndex.resolve(ref);
        if (!resolved.found) {
            result.diagnostic = resolved.diagnostic;
            return result;
        }

        const TopoDS_Face face = TopoDS::Face(resolved.shape);
        if (face.IsNull()) {
            result.diagnostic = QStringLiteral("SelectionRef did not resolve to a face.");
            return result;
        }

        BRepAdaptor_Surface surface(face, Standard_True);
        GProp_GProps props;
        BRepGProp::SurfaceProperties(face, props, Standard_False, Standard_False);

        const double u = midpoint(surface.FirstUParameter(), surface.LastUParameter());
        const double v = midpoint(surface.FirstVParameter(), surface.LastVParameter());
        gp_Pnt samplePoint;
        gp_Vec du;
        gp_Vec dv;
        surface.D1(u, v, samplePoint, du, dv);

        gp_Vec normal = du.Crossed(dv);
        bool normalValid = normal.Magnitude() > Precision::Confusion();
        if (normalValid) {
            normal.Normalize();
            if (face.Orientation() == TopAbs_REVERSED) {
                normal.Reverse();
            }
        }

        LegacyFaceGeometry geometry;
        geometry.stableId = ref.stableId;
        geometry.surfaceKind = toLegacySurfaceKind(surface.GetType());
        geometry.area = props.Mass();
        geometry.samplePoint = toLegacyPoint(samplePoint);
        geometry.normal = toLegacyVector(normal, normalValid);
        geometry.bounds = makeBounds(face);
        geometry.boundaryLoops = collectBoundaryLoops(face, m_selectionIndex);
        geometry.boundaryEdgeCount = countBoundaryEdges(face);
        geometry.fingerprint = makeFaceFingerprint(geometry);

        result.value = geometry;
        result.ok = true;
        return result;
    } catch (const Standard_Failure& failure) {
        result.diagnostic = QStringLiteral("OCCT face query failed: %1")
                                .arg(QString::fromUtf8(failure.GetMessageString()));
    } catch (...) {
        result.diagnostic = QStringLiteral("OCCT face query failed: unknown exception.");
    }
    return result;
}

LegacyGeometryQueryResult<LegacyPoint3d> OccLegacyGeometryAdapter::edgePointAtParameter(
    const LegacySelectionRef& ref,
    double parameter) const
{
    LegacyGeometryQueryResult<LegacyPoint3d> result;
    if (ref.shapeKind != LegacyShapeKind::Edge) {
        result.diagnostic = QStringLiteral("LegacyGeometryAdapter expected an edge ref.");
        return result;
    }

    try {
        const OccSelectionResolveResult resolved = m_selectionIndex.resolve(ref);
        if (!resolved.found) {
            result.diagnostic = resolved.diagnostic;
            return result;
        }

        const TopoDS_Edge edge = TopoDS::Edge(resolved.shape);
        if (edge.IsNull()) {
            result.diagnostic = QStringLiteral("SelectionRef did not resolve to an edge.");
            return result;
        }

        BRepAdaptor_Curve curve(edge);
        const double first = curve.FirstParameter();
        const double last = curve.LastParameter();
        if (!std::isfinite(parameter) || parameter < first - Precision::Confusion() ||
            parameter > last + Precision::Confusion()) {
            result.diagnostic = QStringLiteral("edge parameter is out of range");
            return result;
        }

        result.value = toLegacyPoint(curve.Value(parameter));
        result.ok = true;
        return result;
    } catch (const Standard_Failure& failure) {
        result.diagnostic = QStringLiteral("OCCT edge point query failed: %1")
                                .arg(QString::fromUtf8(failure.GetMessageString()));
    } catch (...) {
        result.diagnostic = QStringLiteral("OCCT edge point query failed: unknown exception.");
    }
    return result;
}

LegacyGeometryQueryResult<std::vector<LegacyPoint3d>> OccLegacyGeometryAdapter::edgeSamplePoints(
    const LegacySelectionRef& ref,
    int sampleCount) const
{
    LegacyGeometryQueryResult<std::vector<LegacyPoint3d>> result;
    if (ref.shapeKind != LegacyShapeKind::Edge) {
        result.diagnostic = QStringLiteral("LegacyGeometryAdapter expected an edge ref.");
        return result;
    }
    if (sampleCount <= 0) {
        result.diagnostic = QStringLiteral("edge sample count must be positive");
        return result;
    }

    try {
        const OccSelectionResolveResult resolved = m_selectionIndex.resolve(ref);
        if (!resolved.found) {
            result.diagnostic = resolved.diagnostic;
            return result;
        }

        const TopoDS_Edge edge = TopoDS::Edge(resolved.shape);
        if (edge.IsNull()) {
            result.diagnostic = QStringLiteral("SelectionRef did not resolve to an edge.");
            return result;
        }

        BRepAdaptor_Curve curve(edge);
        const double first = curve.FirstParameter();
        const double last = curve.LastParameter();
        if (!std::isfinite(first) || !std::isfinite(last)) {
            result.diagnostic = QStringLiteral("edge parameter range is not finite");
            return result;
        }

        result.value.reserve(static_cast<std::size_t>(sampleCount));
        if (sampleCount == 1) {
            result.value.push_back(toLegacyPoint(curve.Value(midpoint(first, last))));
        } else {
            for (int index = 0; index < sampleCount; ++index) {
                const double ratio = static_cast<double>(index) /
                                     static_cast<double>(sampleCount - 1);
                const double parameter = first + (last - first) * ratio;
                result.value.push_back(toLegacyPoint(curve.Value(parameter)));
            }
        }

        result.ok = true;
        return result;
    } catch (const Standard_Failure& failure) {
        result.diagnostic = QStringLiteral("OCCT edge sampling query failed: %1")
                                .arg(QString::fromUtf8(failure.GetMessageString()));
    } catch (...) {
        result.diagnostic =
            QStringLiteral("OCCT edge sampling query failed: unknown exception.");
    }
    return result;
}

LegacyGeometryQueryResult<LegacyEdgeDifferential>
OccLegacyGeometryAdapter::edgeDifferentialAtParameter(const LegacySelectionRef& ref,
                                                      double parameter) const
{
    LegacyGeometryQueryResult<LegacyEdgeDifferential> result;
    if (ref.shapeKind != LegacyShapeKind::Edge) {
        result.diagnostic = QStringLiteral("LegacyGeometryAdapter expected an edge ref.");
        return result;
    }

    try {
        const OccSelectionResolveResult resolved = m_selectionIndex.resolve(ref);
        if (!resolved.found) {
            result.diagnostic = resolved.diagnostic;
            return result;
        }

        const TopoDS_Edge edge = TopoDS::Edge(resolved.shape);
        if (edge.IsNull()) {
            result.diagnostic = QStringLiteral("SelectionRef did not resolve to an edge.");
            return result;
        }

        BRepAdaptor_Curve curve(edge);
        const double first = curve.FirstParameter();
        const double last = curve.LastParameter();
        if (!std::isfinite(parameter) || parameter < first - Precision::Confusion() ||
            parameter > last + Precision::Confusion()) {
            result.diagnostic = QStringLiteral("edge parameter is out of range");
            return result;
        }

        gp_Pnt point;
        gp_Vec tangent;
        curve.D1(parameter, point, tangent);

        const bool tangentValid = tangent.Magnitude() > Precision::Confusion();
        if (tangentValid) {
            tangent.Normalize();
        }

        LegacyEdgeDifferential differential;
        differential.stableId = ref.stableId;
        differential.parameter = parameter;
        differential.point = toLegacyPoint(point);
        differential.tangent = toLegacyVector(tangent, tangentValid);

        result.value = differential;
        result.ok = true;
        return result;
    } catch (const Standard_Failure& failure) {
        result.diagnostic = QStringLiteral("OCCT edge differential query failed: %1")
                                .arg(QString::fromUtf8(failure.GetMessageString()));
    } catch (...) {
        result.diagnostic =
            QStringLiteral("OCCT edge differential query failed: unknown exception.");
    }
    return result;
}

LegacyGeometryQueryResult<LegacyDistanceMeasure> OccLegacyGeometryAdapter::distanceBetween(
    const LegacySelectionRef& first,
    const LegacySelectionRef& second) const
{
    LegacyGeometryQueryResult<LegacyDistanceMeasure> result;

    try {
        const OccSelectionResolveResult firstResolved = m_selectionIndex.resolve(first);
        if (!firstResolved.found) {
            result.diagnostic = QStringLiteral("first selection failed: %1")
                                    .arg(firstResolved.diagnostic);
            return result;
        }

        const OccSelectionResolveResult secondResolved = m_selectionIndex.resolve(second);
        if (!secondResolved.found) {
            result.diagnostic = QStringLiteral("second selection failed: %1")
                                    .arg(secondResolved.diagnostic);
            return result;
        }

        BRepExtrema_DistShapeShape distance(firstResolved.shape, secondResolved.shape);
        distance.Perform();
        if (!distance.IsDone() || distance.NbSolution() <= 0) {
            result.diagnostic =
                QStringLiteral("OCCT distance query did not produce a solution.");
            return result;
        }

        LegacyDistanceMeasure measure;
        measure.firstStableId = first.stableId;
        measure.secondStableId = second.stableId;
        measure.distance = distance.Value();
        measure.pointOnFirst = toLegacyPoint(distance.PointOnShape1(1));
        measure.pointOnSecond = toLegacyPoint(distance.PointOnShape2(1));
        measure.pointsValid = true;

        result.value = measure;
        result.ok = true;
        return result;
    } catch (const Standard_Failure& failure) {
        result.diagnostic = QStringLiteral("OCCT distance query failed: %1")
                                .arg(QString::fromUtf8(failure.GetMessageString()));
    } catch (...) {
        result.diagnostic = QStringLiteral("OCCT distance query failed: unknown exception.");
    }
    return result;
}

LegacyGeometryQueryResult<LegacyIntersectionSet>
OccLegacyGeometryAdapter::edgeFaceIntersections(const LegacySelectionRef& edgeRef,
                                                const LegacySelectionRef& faceRef) const
{
    LegacyGeometryQueryResult<LegacyIntersectionSet> result;
    if (edgeRef.shapeKind != LegacyShapeKind::Edge) {
        result.diagnostic = QStringLiteral("LegacyGeometryAdapter expected an edge ref.");
        return result;
    }
    if (faceRef.shapeKind != LegacyShapeKind::Face) {
        result.diagnostic = QStringLiteral("LegacyGeometryAdapter expected a face ref.");
        return result;
    }

    try {
        const OccSelectionResolveResult edgeResolved = m_selectionIndex.resolve(edgeRef);
        if (!edgeResolved.found) {
            result.diagnostic = edgeResolved.diagnostic;
            return result;
        }

        const OccSelectionResolveResult faceResolved = m_selectionIndex.resolve(faceRef);
        if (!faceResolved.found) {
            result.diagnostic = faceResolved.diagnostic;
            return result;
        }

        const TopoDS_Edge edge = TopoDS::Edge(edgeResolved.shape);
        const TopoDS_Face face = TopoDS::Face(faceResolved.shape);
        if (edge.IsNull()) {
            result.diagnostic = QStringLiteral("SelectionRef did not resolve to an edge.");
            return result;
        }
        if (face.IsNull()) {
            result.diagnostic = QStringLiteral("SelectionRef did not resolve to a face.");
            return result;
        }

        BRepExtrema_DistShapeShape distance(edge, face, Precision::Confusion());
        distance.Perform();
        if (!distance.IsDone()) {
            result.diagnostic = QStringLiteral("OCCT edge-face intersection query failed.");
            return result;
        }

        LegacyIntersectionSet intersections;
        intersections.edgeStableId = edgeRef.stableId;
        intersections.faceStableId = faceRef.stableId;

        const double tolerance = Precision::Confusion() * 10.0;
        intersections.overlap = distance.Value() <= tolerance;
        if (intersections.overlap) {
            for (int index = 1; index <= distance.NbSolution(); ++index) {
                const gp_Pnt edgePoint = distance.PointOnShape1(index);
                const gp_Pnt facePoint = distance.PointOnShape2(index);
                const gp_Pnt representativePoint =
                    pointsClose(edgePoint, facePoint, tolerance)
                        ? edgePoint
                        : gp_Pnt((edgePoint.X() + facePoint.X()) * 0.5,
                                 (edgePoint.Y() + facePoint.Y()) * 0.5,
                                 (edgePoint.Z() + facePoint.Z()) * 0.5);

                bool parameterValid = false;
                double edgeParameter = 0.0;
                try {
                    distance.ParOnEdgeS1(index, edgeParameter);
                    parameterValid = std::isfinite(edgeParameter);
                } catch (...) {
                    edgeParameter = projectParameterOnEdge(edge,
                                                           representativePoint,
                                                           &parameterValid);
                }
                if (!parameterValid) {
                    edgeParameter = projectParameterOnEdge(edge,
                                                           representativePoint,
                                                           &parameterValid);
                }

                appendUniqueIntersectionPoint(&intersections,
                                              representativePoint,
                                              edgeParameter,
                                              parameterValid);
            }
        }

        result.value = intersections;
        result.ok = true;
        return result;
    } catch (const Standard_Failure& failure) {
        result.diagnostic = QStringLiteral("OCCT edge-face intersection query failed: %1")
                                .arg(QString::fromUtf8(failure.GetMessageString()));
    } catch (...) {
        result.diagnostic =
            QStringLiteral("OCCT edge-face intersection query failed: unknown exception.");
    }
    return result;
}

LegacyGeometryQueryResult<LegacyCurveInterval> OccLegacyGeometryAdapter::edgeInterval(
    const LegacySelectionRef& edgeRef,
    double firstParameter,
    double lastParameter,
    int sampleCount) const
{
    LegacyGeometryQueryResult<LegacyCurveInterval> result;
    if (edgeRef.shapeKind != LegacyShapeKind::Edge) {
        result.diagnostic = QStringLiteral("LegacyGeometryAdapter expected an edge ref.");
        return result;
    }
    if (sampleCount <= 0) {
        result.diagnostic = QStringLiteral("edge interval sample count must be positive");
        return result;
    }
    if (!std::isfinite(firstParameter) || !std::isfinite(lastParameter)) {
        result.diagnostic = QStringLiteral("edge interval parameters must be finite");
        return result;
    }
    if (firstParameter >= lastParameter) {
        result.diagnostic = QStringLiteral("edge interval parameters must be ordered");
        return result;
    }

    try {
        const OccSelectionResolveResult resolved = m_selectionIndex.resolve(edgeRef);
        if (!resolved.found) {
            result.diagnostic = resolved.diagnostic;
            return result;
        }

        const TopoDS_Edge edge = TopoDS::Edge(resolved.shape);
        if (edge.IsNull()) {
            result.diagnostic = QStringLiteral("SelectionRef did not resolve to an edge.");
            return result;
        }

        BRepAdaptor_Curve curve(edge);
        const double edgeFirst = curve.FirstParameter();
        const double edgeLast = curve.LastParameter();
        if (!std::isfinite(edgeFirst) || !std::isfinite(edgeLast)) {
            result.diagnostic = QStringLiteral("edge parameter range is not finite");
            return result;
        }
        if (firstParameter < edgeFirst - Precision::Confusion() ||
            lastParameter > edgeLast + Precision::Confusion()) {
            result.diagnostic = QStringLiteral("edge interval parameters are out of range");
            return result;
        }

        LegacyCurveInterval interval;
        interval.sourceEdgeStableId = edgeRef.stableId;
        interval.firstParameter = firstParameter;
        interval.lastParameter = lastParameter;
        interval.startPoint = toLegacyPoint(curve.Value(firstParameter));
        interval.endPoint = toLegacyPoint(curve.Value(lastParameter));

        interval.samplePoints.reserve(static_cast<std::size_t>(sampleCount));
        if (sampleCount == 1) {
            interval.samplePoints.push_back(toLegacyPoint(curve.Value(
                midpoint(firstParameter, lastParameter))));
        } else {
            for (int index = 0; index < sampleCount; ++index) {
                const double ratio = static_cast<double>(index) /
                                     static_cast<double>(sampleCount - 1);
                const double parameter =
                    firstParameter + (lastParameter - firstParameter) * ratio;
                interval.samplePoints.push_back(toLegacyPoint(curve.Value(parameter)));
            }
        }

        interval.bounds = makeBounds(interval.samplePoints);
        interval.length = GCPnts_AbscissaPoint::Length(
            curve,
            firstParameter,
            lastParameter,
            Precision::Confusion());
        if (!std::isfinite(interval.length) || interval.length <= 0.0) {
            interval.length = polylineLength(interval.samplePoints);
        }

        result.value = interval;
        result.ok = true;
        return result;
    } catch (const Standard_Failure& failure) {
        result.diagnostic = QStringLiteral("OCCT edge interval query failed: %1")
                                .arg(QString::fromUtf8(failure.GetMessageString()));
    } catch (...) {
        result.diagnostic =
            QStringLiteral("OCCT edge interval query failed: unknown exception.");
    }
    return result;
}

LegacyGeometryQueryResult<LegacyEdgePointProjection> OccLegacyGeometryAdapter::edgeProjectPoint(
    const LegacySelectionRef& edgeRef,
    LegacyPoint3d point) const
{
    LegacyGeometryQueryResult<LegacyEdgePointProjection> result;
    if (edgeRef.shapeKind != LegacyShapeKind::Edge) {
        result.diagnostic = QStringLiteral("LegacyGeometryAdapter expected an edge ref.");
        return result;
    }
    if (!pointFinite(point)) {
        result.diagnostic = QStringLiteral("edge projection input point must be finite");
        return result;
    }

    try {
        const OccSelectionResolveResult resolved = m_selectionIndex.resolve(edgeRef);
        if (!resolved.found) {
            result.diagnostic = resolved.diagnostic;
            return result;
        }

        const TopoDS_Edge edge = TopoDS::Edge(resolved.shape);
        if (edge.IsNull()) {
            result.diagnostic = QStringLiteral("SelectionRef did not resolve to an edge.");
            return result;
        }

        BRepAdaptor_Curve curve(edge);
        const double first = curve.FirstParameter();
        const double last = curve.LastParameter();
        if (!std::isfinite(first) || !std::isfinite(last)) {
            result.diagnostic = QStringLiteral("edge parameter range is not finite");
            return result;
        }

        const gp_Pnt inputPoint(point.x, point.y, point.z);
        bool parameterValid = false;
        const double parameter = projectParameterOnEdge(edge, inputPoint, &parameterValid);
        if (!parameterValid || !std::isfinite(parameter)) {
            result.diagnostic = QStringLiteral("edge projection parameter is not finite");
            return result;
        }

        const gp_Pnt projectedPoint = curve.Value(parameter);

        LegacyEdgePointProjection projection;
        projection.edgeStableId = edgeRef.stableId;
        projection.inputPoint = point;
        projection.projectedPoint = toLegacyPoint(projectedPoint);
        projection.parameter = parameter;
        projection.distance = std::sqrt(squaredDistance(inputPoint, projectedPoint));
        projection.parameterValid = true;
        projection.inside = parameterInsideSplitInterval(parameter, first, last);

        result.value = projection;
        result.ok = true;
        return result;
    } catch (const Standard_Failure& failure) {
        result.diagnostic = QStringLiteral("OCCT edge projection query failed: %1")
                                .arg(QString::fromUtf8(failure.GetMessageString()));
    } catch (...) {
        result.diagnostic =
            QStringLiteral("OCCT edge projection query failed: unknown exception.");
    }
    return result;
}

LegacyGeometryQueryResult<LegacyCurveSplit> OccLegacyGeometryAdapter::edgeSplitAtParameter(
    const LegacySelectionRef& edgeRef,
    double splitParameter,
    int sampleCountPerInterval) const
{
    LegacyGeometryQueryResult<LegacyCurveSplit> result;
    if (edgeRef.shapeKind != LegacyShapeKind::Edge) {
        result.diagnostic = QStringLiteral("LegacyGeometryAdapter expected an edge ref.");
        return result;
    }
    if (sampleCountPerInterval <= 0) {
        result.diagnostic = QStringLiteral("edge split sample count must be positive");
        return result;
    }
    if (!std::isfinite(splitParameter)) {
        result.diagnostic = QStringLiteral("edge split parameter must be finite");
        return result;
    }

    try {
        const OccSelectionResolveResult resolved = m_selectionIndex.resolve(edgeRef);
        if (!resolved.found) {
            result.diagnostic = resolved.diagnostic;
            return result;
        }

        const TopoDS_Edge edge = TopoDS::Edge(resolved.shape);
        if (edge.IsNull()) {
            result.diagnostic = QStringLiteral("SelectionRef did not resolve to an edge.");
            return result;
        }

        BRepAdaptor_Curve curve(edge);
        const double first = curve.FirstParameter();
        const double last = curve.LastParameter();
        if (!std::isfinite(first) || !std::isfinite(last)) {
            result.diagnostic = QStringLiteral("edge parameter range is not finite");
            return result;
        }

        if (!parameterInsideSplitInterval(splitParameter, first, last)) {
            result.diagnostic =
                QStringLiteral("edge split parameter must be inside edge interval");
            return result;
        }

        const auto firstInterval =
            edgeInterval(edgeRef, first, splitParameter, sampleCountPerInterval);
        if (!firstInterval.ok) {
            result.diagnostic = QStringLiteral("first split interval failed: %1")
                                    .arg(firstInterval.diagnostic);
            return result;
        }

        const auto secondInterval =
            edgeInterval(edgeRef, splitParameter, last, sampleCountPerInterval);
        if (!secondInterval.ok) {
            result.diagnostic = QStringLiteral("second split interval failed: %1")
                                    .arg(secondInterval.diagnostic);
            return result;
        }

        LegacyCurveSplit split;
        split.sourceEdgeStableId = edgeRef.stableId;
        split.splitParameter = splitParameter;
        split.splitPoint = toLegacyPoint(curve.Value(splitParameter));
        split.firstInterval = firstInterval.value;
        split.secondInterval = secondInterval.value;

        result.value = split;
        result.ok = true;
        return result;
    } catch (const Standard_Failure& failure) {
        result.diagnostic = QStringLiteral("OCCT edge split query failed: %1")
                                .arg(QString::fromUtf8(failure.GetMessageString()));
    } catch (...) {
        result.diagnostic = QStringLiteral("OCCT edge split query failed: unknown exception.");
    }
    return result;
}

LegacyGeometryQueryResult<LegacyCurveSplit> OccLegacyGeometryAdapter::edgeSplitAtPoint(
    const LegacySelectionRef& edgeRef,
    LegacyPoint3d point,
    int sampleCountPerInterval) const
{
    LegacyGeometryQueryResult<LegacyCurveSplit> result;
    if (edgeRef.shapeKind != LegacyShapeKind::Edge) {
        result.diagnostic = QStringLiteral("LegacyGeometryAdapter expected an edge ref.");
        return result;
    }
    if (sampleCountPerInterval <= 0) {
        result.diagnostic = QStringLiteral("edge split sample count must be positive");
        return result;
    }

    const auto projection = edgeProjectPoint(edgeRef, point);
    if (!projection.ok) {
        result.diagnostic = QStringLiteral("edge split point projection failed: %1")
                                .arg(projection.diagnostic);
        return result;
    }
    if (!projection.value.parameterValid || !projection.value.inside) {
        result.diagnostic =
            QStringLiteral("edge split projected parameter must be inside edge interval");
        return result;
    }

    const auto split =
        edgeSplitAtParameter(edgeRef, projection.value.parameter, sampleCountPerInterval);
    if (!split.ok) {
        result.diagnostic = QStringLiteral("edge split at projected point failed: %1")
                                .arg(split.diagnostic);
        return result;
    }

    result.value = split.value;
    result.value.splitPoint = projection.value.projectedPoint;
    result.ok = true;
    return result;
}

LegacyGeometryQueryResult<LegacyEdgeEndpointTrim> OccLegacyGeometryAdapter::edgeTrimEndpoint(
    const LegacySelectionRef& edgeRef,
    double delta,
    int endFlag,
    int sampleCount) const
{
    LegacyGeometryQueryResult<LegacyEdgeEndpointTrim> result;
    if (edgeRef.shapeKind != LegacyShapeKind::Edge) {
        result.diagnostic = QStringLiteral("LegacyGeometryAdapter expected an edge ref.");
        return result;
    }
    if (!std::isfinite(delta)) {
        result.diagnostic = QStringLiteral("edge trim delta must be finite");
        return result;
    }
    if (endFlag != 0 && endFlag != 1) {
        result.diagnostic = QStringLiteral("edge trim endFlag must be 0 or 1");
        return result;
    }
    if (sampleCount <= 0) {
        result.diagnostic = QStringLiteral("edge trim sample count must be positive");
        return result;
    }

    constexpr double minimumTrimmedLength = 0.01;
    const double trimDistance = std::abs(delta);
    if (trimDistance <= Precision::Confusion()) {
        result.diagnostic =
            QStringLiteral("edge trim distance is too small to move endpoint safely");
        return result;
    }
    if (delta >= 0.0) {
        result.diagnostic =
            QStringLiteral("edge trim delta must be negative for inward trim");
        return result;
    }

    try {
        const OccSelectionResolveResult resolved = m_selectionIndex.resolve(edgeRef);
        if (!resolved.found) {
            result.diagnostic = resolved.diagnostic;
            return result;
        }

        const TopoDS_Edge edge = TopoDS::Edge(resolved.shape);
        if (edge.IsNull()) {
            result.diagnostic = QStringLiteral("SelectionRef did not resolve to an edge.");
            return result;
        }

        BRepAdaptor_Curve curve(edge);
        const double first = curve.FirstParameter();
        const double last = curve.LastParameter();
        if (!std::isfinite(first) || !std::isfinite(last) || first >= last) {
            result.diagnostic = QStringLiteral("edge parameter range is not finite");
            return result;
        }

        const double totalLength = curveLength(curve, first, last);
        if (totalLength <= trimDistance + minimumTrimmedLength) {
            result.diagnostic =
                QStringLiteral("edge trim would violate minimum length threshold");
            return result;
        }

        const double startParameter =
            endFlag == 0
                ? parameterAtLengthFromStart(curve, first, last, trimDistance)
                : first;
        const double endParameter =
            endFlag == 1
                ? parameterAtLengthFromStart(curve, first, last, totalLength - trimDistance)
                : last;
        if (startParameter >= endParameter) {
            result.diagnostic =
                QStringLiteral("edge trim would violate minimum length threshold");
            return result;
        }

        const auto interval = edgeInterval(edgeRef, startParameter, endParameter, sampleCount);
        if (!interval.ok) {
            result.diagnostic = QStringLiteral("edge trim interval failed: %1")
                                    .arg(interval.diagnostic);
            return result;
        }

        LegacyEdgeEndpointTrim trim;
        trim.sourceEdgeStableId = edgeRef.stableId;
        trim.delta = delta;
        trim.endFlag = endFlag;
        trim.trimDistance = trimDistance;
        trim.originalParameter = endFlag == 0 ? first : last;
        trim.trimmedParameter = endFlag == 0 ? startParameter : endParameter;
        trim.originalEndpoint = toLegacyPoint(curve.Value(trim.originalParameter));
        trim.trimmedEndpoint = toLegacyPoint(curve.Value(trim.trimmedParameter));
        trim.interval = interval.value;

        result.value = trim;
        result.ok = true;
        return result;
    } catch (const Standard_Failure& failure) {
        result.diagnostic = QStringLiteral("OCCT edge trim query failed: %1")
                                .arg(QString::fromUtf8(failure.GetMessageString()));
    } catch (...) {
        result.diagnostic = QStringLiteral("OCCT edge trim query failed: unknown exception.");
    }
    return result;
}

LegacyGeometryQueryResult<LegacyEdgeGroupDistance>
OccLegacyGeometryAdapter::pointToEdgeGroupDistance(
    LegacyPoint3d point,
    const std::vector<LegacySelectionRef>& edgeRefs,
    double threshold) const
{
    LegacyGeometryQueryResult<LegacyEdgeGroupDistance> result;
    if (!pointFinite(point)) {
        result.diagnostic = QStringLiteral("edge group distance input point must be finite");
        return result;
    }
    if (!std::isfinite(threshold) || threshold < 0.0) {
        result.diagnostic =
            QStringLiteral("edge group distance threshold must be finite and non-negative");
        return result;
    }
    if (edgeRefs.empty()) {
        result.diagnostic = QStringLiteral("edge group distance candidate list is empty");
        return result;
    }

    LegacyEdgeGroupDistance groupDistance;
    groupDistance.inputPoint = point;
    groupDistance.threshold = threshold;
    groupDistance.minDistance = std::numeric_limits<double>::infinity();
    groupDistance.candidates.reserve(edgeRefs.size());

    for (std::size_t index = 0; index < edgeRefs.size(); ++index) {
        const LegacySelectionRef& edgeRef = edgeRefs[index];
        if (edgeRef.shapeKind != LegacyShapeKind::Edge) {
            result.diagnostic =
                QStringLiteral("edge group distance candidate %1 expected an edge ref")
                    .arg(static_cast<qulonglong>(index));
            return result;
        }

        const auto projection = edgeProjectPoint(edgeRef, point);
        if (!projection.ok) {
            result.diagnostic =
                QStringLiteral("edge group distance candidate %1 projection failed: %2")
                    .arg(static_cast<qulonglong>(index))
                    .arg(projection.diagnostic);
            return result;
        }

        LegacyEdgeGroupDistanceCandidate candidate;
        candidate.edgeStableId = edgeRef.stableId;
        candidate.distance = projection.value.distance;
        candidate.pointOnEdge = projection.value.projectedPoint;
        candidate.edgeParameter = projection.value.parameter;
        candidate.edgeParameterValid = projection.value.parameterValid;
        candidate.inside = projection.value.inside;
        groupDistance.candidates.push_back(candidate);

        if (!groupDistance.hasNearest ||
            candidate.distance < groupDistance.minDistance) {
            groupDistance.hasNearest = true;
            groupDistance.minDistance = candidate.distance;
            groupDistance.nearest = candidate;
        }
    }

    if (!groupDistance.hasNearest || !std::isfinite(groupDistance.minDistance)) {
        result.diagnostic = QStringLiteral("edge group distance has no valid candidates");
        return result;
    }

    groupDistance.tooClose = groupDistance.minDistance <= threshold;
    result.value = groupDistance;
    result.ok = true;
    return result;
}

LegacyGeometryQueryResult<LegacySplineCurveBuild>
OccLegacyGeometryAdapter::buildSplineFromPoints(
    const std::vector<LegacyPoint3d>& points,
    int sampleCount) const
{
    LegacyGeometryQueryResult<LegacySplineCurveBuild> result;
    LegacySplineCurveBuild build;
    build.inputPoints = points;
    build.inputPointCount = static_cast<int>(points.size());
    build.requestedSampleCount = sampleCount;
    build.effectiveSampleCount = std::max(5, sampleCount);
    build.sourcePolylineLength = polylineLength(points);
    build.legacySuggestedSampleCount =
        legacySplineSuggestedSampleCount(build.sourcePolylineLength);

    auto reject = [&result, &build](QString diagnostic) {
        build.failureReason = diagnostic.toStdString();
        result.value = build;
        result.diagnostic = std::move(diagnostic);
        return result;
    };

    if (sampleCount <= 0) {
        return reject(QStringLiteral("spline sample count must be positive"));
    }
    if (points.size() < 3) {
        return reject(QStringLiteral("spline point list must contain at least 3 points"));
    }
    for (const LegacyPoint3d& point : points) {
        if (!pointFinite(point)) {
            return reject(QStringLiteral("spline point list must contain finite points"));
        }
    }
    if (build.sourcePolylineLength <= Precision::Confusion()) {
        return reject(QStringLiteral("spline point list length too short"));
    }
    if (hasCollapsedConsecutivePoints(points)) {
        return reject(QStringLiteral("spline point list has duplicate consecutive points"));
    }

    try {
        TColgp_Array1OfPnt pointArray(1, static_cast<Standard_Integer>(points.size()));
        for (std::size_t index = 0; index < points.size(); ++index) {
            pointArray.SetValue(static_cast<Standard_Integer>(index + 1),
                                gp_Pnt(points[index].x, points[index].y, points[index].z));
        }

        GeomAPI_PointsToBSpline splineBuilder(pointArray);
        if (!splineBuilder.IsDone()) {
            return reject(QStringLiteral("OCCT spline rebuild did not converge"));
        }

        const Handle(Geom_BSplineCurve) curve = splineBuilder.Curve();
        if (curve.IsNull()) {
            return reject(QStringLiteral("OCCT spline rebuild returned null curve"));
        }

        const double first = curve->FirstParameter();
        const double last = curve->LastParameter();
        if (!std::isfinite(first) || !std::isfinite(last) || first >= last) {
            return reject(QStringLiteral("OCCT spline parameter range is not finite"));
        }

        build.samplePoints.reserve(static_cast<std::size_t>(build.effectiveSampleCount));
        for (int index = 0; index < build.effectiveSampleCount; ++index) {
            const double ratio =
                build.effectiveSampleCount == 1
                    ? 0.5
                    : static_cast<double>(index) /
                          static_cast<double>(build.effectiveSampleCount - 1);
            const double parameter = first + (last - first) * ratio;
            build.samplePoints.push_back(toLegacyPoint(curve->Value(parameter)));
        }
        build.samplePoints.front() = points.front();
        build.samplePoints.back() = points.back();

        GeomAdaptor_Curve curveAdaptor(curve);
        build.length = GCPnts_AbscissaPoint::Length(
            curveAdaptor,
            first,
            last,
            Precision::Confusion());
        if (!std::isfinite(build.length) || build.length <= 0.0) {
            build.length = polylineLength(build.samplePoints);
        }
        if (!std::isfinite(build.length) || build.length <= Precision::Confusion()) {
            return reject(QStringLiteral("spline rebuilt curve length too short"));
        }

        build.bounds = makeBounds(build.samplePoints);
        build.curveKind = LegacyCurveKind::BSpline;
        build.buildable = true;
        result.value = build;
        result.ok = true;
        return result;
    } catch (const Standard_Failure& failure) {
        return reject(QStringLiteral("OCCT spline rebuild failed: %1")
                          .arg(QString::fromUtf8(failure.GetMessageString())));
    } catch (...) {
        return reject(QStringLiteral("OCCT spline rebuild failed: unknown exception."));
    }
}

LegacyGeometryQueryResult<LegacyWireChain>
OccLegacyGeometryAdapter::buildWireChain(
    const std::vector<LegacySelectionRef>& edgeRefs) const
{
    struct EdgeRecord
    {
        LegacyEdgeGeometry geometry;
        int inputOrdinal = 0;
    };

    LegacyGeometryQueryResult<LegacyWireChain> result;
    LegacyWireChain chain;
    chain.inputEdgeCount = static_cast<int>(edgeRefs.size());
    chain.inputEdgeStableIds.reserve(edgeRefs.size());

    auto reject = [&result, &chain](QString diagnostic) {
        chain.failureReason = diagnostic.toStdString();
        result.value = chain;
        result.diagnostic = std::move(diagnostic);
        return result;
    };

    if (edgeRefs.empty()) {
        return reject(QStringLiteral("wire chain edge list is empty"));
    }

    std::vector<EdgeRecord> records;
    records.reserve(edgeRefs.size());
    for (std::size_t index = 0; index < edgeRefs.size(); ++index) {
        const LegacySelectionRef& edgeRef = edgeRefs[index];
        chain.inputEdgeStableIds.push_back(edgeRef.stableId);
        if (edgeRef.shapeKind != LegacyShapeKind::Edge) {
            return reject(QStringLiteral("wire chain edge %1 expected an edge ref")
                              .arg(static_cast<qulonglong>(index)));
        }

        const auto geometry = edgeGeometry(edgeRef);
        if (!geometry.ok) {
            return reject(QStringLiteral("wire chain edge %1 geometry failed: %2")
                              .arg(static_cast<qulonglong>(index))
                              .arg(geometry.diagnostic));
        }
        if (geometry.value.length <= Precision::Confusion()) {
            return reject(QStringLiteral("wire chain edge %1 length too short")
                              .arg(static_cast<qulonglong>(index)));
        }

        chain.totalLength += geometry.value.length;
        mergeBounds(&chain.bounds, geometry.value.bounds);
        records.push_back({geometry.value, static_cast<int>(index)});
    }

    auto makeOrderedEdge = [](const EdgeRecord& record, bool reversed) {
        LegacyWireChainEdge edge;
        edge.edgeStableId = record.geometry.stableId;
        edge.inputOrdinal = record.inputOrdinal;
        edge.length = record.geometry.length;
        edge.reversed = reversed;
        edge.startPoint = reversed ? record.geometry.endPoint : record.geometry.startPoint;
        edge.endPoint = reversed ? record.geometry.startPoint : record.geometry.endPoint;
        return edge;
    };

    std::deque<LegacyWireChainEdge> ordered;
    ordered.push_back(makeOrderedEdge(records.front(), false));
    std::vector<bool> used(records.size(), false);
    used.front() = true;

    constexpr double connectionTolerance = 1.0e-6;
    int usedCount = 1;
    while (usedCount < static_cast<int>(records.size())) {
        bool matched = false;
        for (std::size_t index = 1; index < records.size(); ++index) {
            if (used[index]) {
                continue;
            }

            const EdgeRecord& candidate = records[index];
            const LegacyPoint3d currentStart = ordered.front().startPoint;
            const LegacyPoint3d currentEnd = ordered.back().endPoint;

            if (legacyPointsClose(currentEnd,
                                  candidate.geometry.startPoint,
                                  connectionTolerance)) {
                ordered.push_back(makeOrderedEdge(candidate, false));
            } else if (legacyPointsClose(currentEnd,
                                         candidate.geometry.endPoint,
                                         connectionTolerance)) {
                ordered.push_back(makeOrderedEdge(candidate, true));
            } else if (legacyPointsClose(currentStart,
                                         candidate.geometry.endPoint,
                                         connectionTolerance)) {
                ordered.push_front(makeOrderedEdge(candidate, false));
            } else if (legacyPointsClose(currentStart,
                                         candidate.geometry.startPoint,
                                         connectionTolerance)) {
                ordered.push_front(makeOrderedEdge(candidate, true));
            } else {
                continue;
            }

            used[index] = true;
            ++usedCount;
            matched = true;
            break;
        }

        if (!matched) {
            chain.orderedEdges.assign(ordered.begin(), ordered.end());
            chain.orderedEdgeCount = static_cast<int>(chain.orderedEdges.size());
            chain.connected = false;
            if (!chain.orderedEdges.empty()) {
                chain.startPoint = chain.orderedEdges.front().startPoint;
                chain.endPoint = chain.orderedEdges.back().endPoint;
            }
            return reject(QStringLiteral("wire chain edges are not connected"));
        }
    }

    chain.orderedEdges.assign(ordered.begin(), ordered.end());
    chain.orderedEdgeCount = static_cast<int>(chain.orderedEdges.size());
    chain.startPoint = chain.orderedEdges.front().startPoint;
    chain.endPoint = chain.orderedEdges.back().endPoint;
    chain.connected = true;
    chain.closed = legacyPointsClose(chain.startPoint,
                                     chain.endPoint,
                                     connectionTolerance) ||
                   (records.size() == 1 && records.front().geometry.closed);

    result.value = chain;
    result.ok = true;
    return result;
}

LegacyGeometryQueryResult<LegacyOffsetCurvePreview>
OccLegacyGeometryAdapter::offsetEdgePreview(
    const LegacySelectionRef& edgeRef,
    double offsetDistance,
    int sampleCount) const
{
    LegacyGeometryQueryResult<LegacyOffsetCurvePreview> result;
    LegacyOffsetCurvePreview preview;
    preview.sourceEdgeStableId = edgeRef.stableId;
    preview.offsetDistance = offsetDistance;
    preview.requestedSampleCount = sampleCount;
    preview.effectiveSampleCount = std::max(2, sampleCount);

    auto reject = [&result, &preview](QString diagnostic) {
        preview.failureReason = diagnostic.toStdString();
        result.value = preview;
        result.diagnostic = std::move(diagnostic);
        return result;
    };

    if (edgeRef.shapeKind != LegacyShapeKind::Edge) {
        return reject(QStringLiteral("LegacyGeometryAdapter expected an edge ref."));
    }
    if (!std::isfinite(offsetDistance)) {
        return reject(QStringLiteral("edge offset distance must be finite"));
    }
    if (std::abs(offsetDistance) <= Precision::Confusion()) {
        return reject(QStringLiteral("edge offset distance must be non-zero"));
    }
    if (sampleCount <= 0) {
        return reject(QStringLiteral("edge offset sample count must be positive"));
    }

    try {
        const OccSelectionResolveResult resolved = m_selectionIndex.resolve(edgeRef);
        if (!resolved.found) {
            return reject(resolved.diagnostic);
        }

        const TopoDS_Edge edge = TopoDS::Edge(resolved.shape);
        if (edge.IsNull()) {
            return reject(QStringLiteral("SelectionRef did not resolve to an edge."));
        }

        BRepAdaptor_Curve sourceAdaptor(edge);
        const double first = sourceAdaptor.FirstParameter();
        const double last = sourceAdaptor.LastParameter();
        if (!std::isfinite(first) || !std::isfinite(last) || first >= last) {
            return reject(QStringLiteral("edge offset source parameter range is not finite"));
        }

        preview.sourceCurveKind = toLegacyCurveKind(sourceAdaptor.GetType());
        preview.sourceLength = curveLength(sourceAdaptor, first, last);
        if (preview.sourceLength <= Precision::Confusion()) {
            return reject(QStringLiteral("edge offset source length too short"));
        }

        Standard_Real curveFirst = 0.0;
        Standard_Real curveLast = 0.0;
        Handle(Geom_Curve) sourceCurve = BRep_Tool::Curve(edge, curveFirst, curveLast);
        if (sourceCurve.IsNull()) {
            return reject(QStringLiteral("edge offset source curve is null"));
        }

        gp_Dir offsetNormal;
        if (!makeStableOffsetNormal(sourceAdaptor, midpoint(first, last), &offsetNormal)) {
            return reject(QStringLiteral("edge offset normal could not be computed"));
        }

        Handle(Geom_OffsetCurve) offsetCurve =
            new Geom_OffsetCurve(sourceCurve, offsetDistance, offsetNormal);
        if (offsetCurve.IsNull()) {
            return reject(QStringLiteral("OCCT edge offset returned null curve"));
        }

        GeomAdaptor_Curve offsetAdaptor(offsetCurve);
        const double offsetFirst = std::max(offsetCurve->FirstParameter(), curveFirst);
        const double offsetLast = std::min(offsetCurve->LastParameter(), curveLast);
        const double sampleFirst = std::isfinite(offsetFirst) ? offsetFirst : first;
        const double sampleLast = std::isfinite(offsetLast) ? offsetLast : last;
        if (!std::isfinite(sampleFirst) || !std::isfinite(sampleLast) ||
            sampleFirst >= sampleLast) {
            return reject(QStringLiteral("edge offset parameter range is not finite"));
        }

        preview.samplePoints.reserve(static_cast<std::size_t>(preview.effectiveSampleCount));
        if (preview.effectiveSampleCount == 1) {
            preview.samplePoints.push_back(
                toLegacyPoint(offsetCurve->Value(midpoint(sampleFirst, sampleLast))));
        } else {
            for (int index = 0; index < preview.effectiveSampleCount; ++index) {
                const double ratio =
                    static_cast<double>(index) /
                    static_cast<double>(preview.effectiveSampleCount - 1);
                const double parameter = sampleFirst + (sampleLast - sampleFirst) * ratio;
                preview.samplePoints.push_back(toLegacyPoint(offsetCurve->Value(parameter)));
            }
        }

        preview.bounds = makeBounds(preview.samplePoints);
        preview.length = GCPnts_AbscissaPoint::Length(
            offsetAdaptor,
            sampleFirst,
            sampleLast,
            Precision::Confusion());
        if (!std::isfinite(preview.length) || preview.length <= 0.0) {
            preview.length = polylineLength(preview.samplePoints);
        }
        if (!std::isfinite(preview.length) || preview.length <= Precision::Confusion()) {
            return reject(QStringLiteral("edge offset preview length too short"));
        }

        preview.offsettable = true;
        result.value = preview;
        result.ok = true;
        return result;
    } catch (const Standard_Failure& failure) {
        return reject(QStringLiteral("OCCT edge offset preview failed: %1")
                          .arg(QString::fromUtf8(failure.GetMessageString())));
    } catch (...) {
        return reject(QStringLiteral("OCCT edge offset preview failed: unknown exception."));
    }
}

LegacyGeometryQueryResult<LegacyFacePlaneSectionPreview>
OccLegacyGeometryAdapter::facePlaneSectionPreview(
    const LegacySelectionRef& faceRef,
    LegacyPlane plane,
    int sampleCount) const
{
    LegacyGeometryQueryResult<LegacyFacePlaneSectionPreview> result;
    LegacyFacePlaneSectionPreview preview;
    preview.sourceFaceStableId = faceRef.stableId;
    preview.plane = plane;
    preview.requestedSampleCount = sampleCount;
    preview.effectiveSampleCount = std::max(2, sampleCount);

    auto reject = [&result, &preview](QString diagnostic) {
        preview.failureReason = diagnostic.toStdString();
        result.value = preview;
        result.diagnostic = std::move(diagnostic);
        return result;
    };

    if (faceRef.shapeKind != LegacyShapeKind::Face) {
        return reject(QStringLiteral("LegacyGeometryAdapter expected a face ref."));
    }
    if (sampleCount <= 0) {
        return reject(QStringLiteral("face section sample count must be positive"));
    }
    if (!planeValid(plane)) {
        return reject(QStringLiteral(
            "face section plane must have finite origin and valid normal"));
    }

    try {
        const OccSelectionResolveResult resolved = m_selectionIndex.resolve(faceRef);
        if (!resolved.found) {
            return reject(resolved.diagnostic);
        }

        const TopoDS_Face face = TopoDS::Face(resolved.shape);
        if (face.IsNull()) {
            return reject(QStringLiteral("SelectionRef did not resolve to a face."));
        }

        const gp_Pln sectionPlane(toGpPoint(plane.origin), gp_Dir(toGpVec(plane.normal)));
        BRepAlgoAPI_Section section(face, sectionPlane, Standard_True);
        section.ComputePCurveOn1(Standard_False);
        section.Approximation(Standard_True);
        if (!section.IsDone()) {
            return reject(QStringLiteral("OCCT face-plane section did not finish"));
        }

        auto appendSectionEdge = [&preview](const TopoDS_Edge& edge,
                                            std::string sourceEdgeStableId) {
            LegacySectionEdge sectionEdge =
                sampleSectionEdge(edge,
                                  static_cast<int>(preview.sectionEdges.size() + 1),
                                  std::move(sourceEdgeStableId),
                                  preview.effectiveSampleCount);
            if (sectionEdge.samplePoints.empty() ||
                sectionEdge.length <= Precision::Confusion()) {
                return;
            }

            preview.totalLength += sectionEdge.length;
            mergeBounds(&preview.bounds, sectionEdge.bounds);
            preview.samplePoints.insert(preview.samplePoints.end(),
                                        sectionEdge.samplePoints.begin(),
                                        sectionEdge.samplePoints.end());
            preview.sectionEdges.push_back(std::move(sectionEdge));
        };

        const TopoDS_Shape sectionShape = section.Shape();
        for (TopExp_Explorer explorer(sectionShape, TopAbs_EDGE);
             explorer.More();
             explorer.Next()) {
            appendSectionEdge(TopoDS::Edge(explorer.Current()), std::string());
        }

        if (preview.sectionEdges.empty()) {
            TopTools_IndexedMapOfShape boundaryEdges;
            TopExp::MapShapes(face, TopAbs_EDGE, boundaryEdges);
            for (int edgeIndex = 1; edgeIndex <= boundaryEdges.Extent(); ++edgeIndex) {
                const TopoDS_Edge boundaryEdge =
                    TopoDS::Edge(boundaryEdges.FindKey(edgeIndex));
                if (!edgeSamplesOnPlane(boundaryEdge,
                                        plane,
                                        preview.effectiveSampleCount)) {
                    continue;
                }

                std::string stableId;
                const std::optional<LegacySelectionRef> boundaryRef =
                    m_selectionIndex.refForShape(boundaryEdge);
                if (boundaryRef.has_value()) {
                    stableId = boundaryRef->stableId;
                }
                appendSectionEdge(boundaryEdge, std::move(stableId));
            }
        }

        preview.hitCount = static_cast<int>(preview.sectionEdges.size());
        if (!preview.bounds.valid) {
            preview.bounds = makeBounds(preview.samplePoints);
        }
        preview.sectionable =
            preview.hitCount > 0 &&
            preview.totalLength > Precision::Confusion() &&
            !preview.samplePoints.empty();

        result.value = preview;
        result.ok = true;
        return result;
    } catch (const Standard_Failure& failure) {
        return reject(QStringLiteral("OCCT face-plane section preview failed: %1")
                          .arg(QString::fromUtf8(failure.GetMessageString())));
    } catch (...) {
        return reject(
            QStringLiteral("OCCT face-plane section preview failed: unknown exception."));
    }
}

LegacyGeometryQueryResult<LegacyCircularSweepPreview>
OccLegacyGeometryAdapter::edgeCircularSweepPreview(
    const LegacySelectionRef& edgeRef,
    double radius,
    int sampleCount) const
{
    LegacyGeometryQueryResult<LegacyCircularSweepPreview> result;
    LegacyCircularSweepPreview preview;
    preview.sourceEdgeStableId = edgeRef.stableId;
    preview.radius = radius;
    preview.requestedSampleCount = sampleCount;
    preview.effectiveSampleCount = std::max(2, sampleCount);

    auto reject = [&result, &preview](QString diagnostic) {
        preview.failureReason = diagnostic.toStdString();
        result.value = preview;
        result.diagnostic = std::move(diagnostic);
        return result;
    };

    if (edgeRef.shapeKind != LegacyShapeKind::Edge) {
        return reject(QStringLiteral("LegacyGeometryAdapter expected an edge ref."));
    }
    if (!std::isfinite(radius)) {
        return reject(QStringLiteral("edge circular sweep radius must be finite"));
    }
    if (radius <= Precision::Confusion()) {
        return reject(QStringLiteral("edge circular sweep radius must be positive"));
    }
    if (sampleCount <= 0) {
        return reject(QStringLiteral("edge circular sweep sample count must be positive"));
    }

    try {
        const OccSelectionResolveResult resolved = m_selectionIndex.resolve(edgeRef);
        if (!resolved.found) {
            return reject(resolved.diagnostic);
        }

        const TopoDS_Edge edge = TopoDS::Edge(resolved.shape);
        if (edge.IsNull()) {
            return reject(QStringLiteral("SelectionRef did not resolve to an edge."));
        }

        BRepAdaptor_Curve sourceAdaptor(edge);
        const double first = sourceAdaptor.FirstParameter();
        const double last = sourceAdaptor.LastParameter();
        if (!std::isfinite(first) || !std::isfinite(last) || first >= last) {
            return reject(QStringLiteral("edge circular sweep source parameter range is not finite"));
        }

        preview.sourceCurveKind = toLegacyCurveKind(sourceAdaptor.GetType());
        preview.pathLength = curveLength(sourceAdaptor, first, last);
        if (!std::isfinite(preview.pathLength) ||
            preview.pathLength <= Precision::Confusion()) {
            return reject(QStringLiteral("edge circular sweep source length too short"));
        }

        preview.samplePoints.reserve(static_cast<std::size_t>(preview.effectiveSampleCount));
        for (int index = 0; index < preview.effectiveSampleCount; ++index) {
            const double ratio =
                preview.effectiveSampleCount == 1
                    ? 0.5
                    : static_cast<double>(index) /
                          static_cast<double>(preview.effectiveSampleCount - 1);
            const double parameter = first + (last - first) * ratio;
            preview.samplePoints.push_back(toLegacyPoint(sourceAdaptor.Value(parameter)));
        }

        gp_Ax2 profileFrame;
        if (!makeSweepProfileFrame(sourceAdaptor, first, &profileFrame)) {
            return reject(QStringLiteral("edge circular sweep profile frame could not be computed"));
        }

        GC_MakeCircle circleMaker(profileFrame, radius);
        if (!circleMaker.IsDone() || circleMaker.Value().IsNull()) {
            return reject(QStringLiteral("edge circular sweep profile circle could not be built"));
        }

        BRepBuilderAPI_MakeEdge profileEdgeMaker(circleMaker.Value());
        if (!profileEdgeMaker.IsDone()) {
            return reject(QStringLiteral("edge circular sweep profile edge could not be built"));
        }

        BRepBuilderAPI_MakeWire profileWireMaker(profileEdgeMaker.Edge());
        if (!profileWireMaker.IsDone()) {
            return reject(QStringLiteral("edge circular sweep profile wire could not be built"));
        }

        BRepBuilderAPI_MakeWire spineWireMaker(edge);
        if (!spineWireMaker.IsDone()) {
            return reject(QStringLiteral("edge circular sweep spine wire could not be built"));
        }

        BRepOffsetAPI_MakePipe pipeMaker(spineWireMaker.Wire(), profileWireMaker.Wire());
        pipeMaker.Build();
        if (!pipeMaker.IsDone()) {
            return reject(QStringLiteral("OCCT edge circular sweep pipe did not finish"));
        }

        const TopoDS_Shape sweptShape = pipeMaker.Shape();
        if (sweptShape.IsNull()) {
            return reject(QStringLiteral("OCCT edge circular sweep returned null shape"));
        }

        preview.bounds = makeBounds(sweptShape);
        if (!preview.bounds.valid) {
            return reject(QStringLiteral("edge circular sweep bbox could not be computed"));
        }
        preview.shapeFaceCount = countSubShapes(sweptShape, TopAbs_FACE);
        preview.shapeEdgeCount = countSubShapes(sweptShape, TopAbs_EDGE);
        preview.shapeVertexCount = countSubShapes(sweptShape, TopAbs_VERTEX);
        if (preview.shapeFaceCount <= 0 || preview.shapeEdgeCount <= 0) {
            return reject(QStringLiteral("edge circular sweep produced empty shape summary"));
        }

        preview.sweepable = true;
        result.value = preview;
        result.ok = true;
        return result;
    } catch (const Standard_Failure& failure) {
        return reject(QStringLiteral("OCCT edge circular sweep preview failed: %1")
                          .arg(QString::fromUtf8(failure.GetMessageString())));
    } catch (...) {
        return reject(
            QStringLiteral("OCCT edge circular sweep preview failed: unknown exception."));
    }
}

} // namespace tsrebar
