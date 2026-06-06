#include "domain/rebar/SteelData.h"
#include "presentation/occ/RebarAisPresentationAdapter.h"

#include <BRepAdaptor_Curve.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>

#include <cmath>
#include <cstdlib>
#include <iostream>

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
    return std::abs(left - right) < 1.0e-7;
}

tsrebar::SteelBarSegment lineSegment()
{
    tsrebar::SteelBarSegment segment;
    segment.segmentId = "segment-line-001";
    segment.barId = "bar-001";
    segment.sequenceNo = 1;
    segment.shapeType = tsrebar::SteelBarSegmentShape::Line;
    segment.startPoint = {0.0, 0.0, 0.0};
    segment.endPoint = {10.0, 0.0, 0.0};
    segment.middlePoint = {5.0, 0.0, 0.0};
    segment.length = 10.0;
    return segment;
}

tsrebar::SteelBarSegment arcSegment()
{
    tsrebar::SteelBarSegment segment;
    segment.segmentId = "segment-arc-001";
    segment.barId = "bar-001";
    segment.sequenceNo = 1;
    segment.shapeType = tsrebar::SteelBarSegmentShape::Arc;
    segment.startPoint = {10.0, 0.0, 0.0};
    segment.middlePoint = {7.0710678119, 7.0710678119, 0.0};
    segment.endPoint = {0.0, 10.0, 0.0};
    segment.length = 15.7;
    return segment;
}

tsrebar::SteelData steelDataWithSegment(tsrebar::SteelBarSegment segment)
{
    tsrebar::SteelBar bar;
    bar.barId = "bar-001";
    bar.groupId = "group-001";
    bar.sequenceNo = 1;
    bar.segmentIds.push_back(segment.segmentId);
    bar.length = segment.length;

    tsrebar::SteelBarGroup group;
    group.groupId = "group-001";
    group.barIds.push_back(bar.barId);
    group.visible = true;
    group.active = true;

    tsrebar::SteelData steelData;
    steelData.steelDataId = "steel-data-001";
    steelData.groups.push_back(group);
    steelData.bars.push_back(bar);
    steelData.segments.push_back(segment);
    return steelData;
}

void testLineSegmentMapsToAisEdge()
{
    const tsrebar::SteelData steelData = steelDataWithSegment(lineSegment());

    const auto result = tsrebar::RebarAisPresentationAdapter{}.buildGroupPresentation(
        steelData, "group-001");

    expect(result.ok, "line segment presentation must succeed");
    expect(result.items.size() == 1, "line group must emit one display item");
    const auto& item = result.items.front();
    expect(item.groupId == "group-001", "line display item group id mismatch");
    expect(item.segmentId == "segment-line-001", "line display item segment id mismatch");
    expect(item.shapeType == tsrebar::SteelBarSegmentShape::Line,
           "line display item shape type mismatch");
    expect(!item.aisShape.IsNull(), "line display item must carry AIS_Shape");
    expect(!item.shape.IsNull(), "line display item must carry TopoDS shape");
    expect(item.shape.ShapeType() == TopAbs_EDGE, "line display shape must be an edge");

    BRepAdaptor_Curve curve(TopoDS::Edge(item.shape));
    expect(curve.GetType() == GeomAbs_Line, "line display edge must use an OCCT line curve");
}

void testArcSegmentMapsToAisCircularEdge()
{
    const tsrebar::SteelData steelData = steelDataWithSegment(arcSegment());

    const auto result = tsrebar::RebarAisPresentationAdapter{}.buildGroupPresentation(
        steelData, "group-001");

    expect(result.ok, "arc segment presentation must succeed");
    expect(result.items.size() == 1, "arc group must emit one display item");
    const auto& item = result.items.front();
    expect(item.segmentId == "segment-arc-001", "arc display item segment id mismatch");
    expect(item.shapeType == tsrebar::SteelBarSegmentShape::Arc,
           "arc display item shape type mismatch");
    expect(!item.aisShape.IsNull(), "arc display item must carry AIS_Shape");
    expect(!item.shape.IsNull(), "arc display item must carry TopoDS shape");
    expect(item.shape.ShapeType() == TopAbs_EDGE, "arc display shape must be an edge");

    BRepAdaptor_Curve curve(TopoDS::Edge(item.shape));
    expect(curve.GetType() == GeomAbs_Circle,
           "arc display edge must use an OCCT circular curve");
    expect(near(curve.Value(curve.FirstParameter()).X(), 10.0),
           "arc display start point mismatch");
}

void testEmptyGroupReturnsStableDiagnostic()
{
    tsrebar::SteelData steelData;
    tsrebar::SteelBarGroup group;
    group.groupId = "empty-group";
    steelData.groups.push_back(group);

    const auto result = tsrebar::RebarAisPresentationAdapter{}.buildGroupPresentation(
        steelData, "empty-group");

    expect(!result.ok, "empty group must be rejected");
    expect(result.items.empty(), "empty group must not emit display items");
    expect(result.diagnostic.contains(QStringLiteral("no displayable segments")),
           "empty group diagnostic must be stable");
}

void testMissingGroupReturnsStableDiagnostic()
{
    const auto result = tsrebar::RebarAisPresentationAdapter{}.buildGroupPresentation(
        tsrebar::SteelData{}, "missing-group");

    expect(!result.ok, "missing group must be rejected");
    expect(result.diagnostic.contains(QStringLiteral("missing group")),
           "missing group diagnostic must be stable");
}

} // namespace

int main()
{
    testLineSegmentMapsToAisEdge();
    testArcSegmentMapsToAisCircularEdge();
    testEmptyGroupReturnsStableDiagnostic();
    testMissingGroupReturnsStableDiagnostic();
    return 0;
}
