#include "geometry/occ/legacy_adapter/OccLegacyGeometryAdapter.h"

#include <BRepBndLib.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepGProp.hxx>
#include <BRepTools.hxx>
#include <Bnd_Box.hxx>
#include <Extrema_ExtPC.hxx>
#include <Extrema_POnCurv.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <GProp_GProps.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <Precision.hxx>
#include <Standard_Failure.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

#include <algorithm>
#include <cmath>
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

} // namespace tsrebar
