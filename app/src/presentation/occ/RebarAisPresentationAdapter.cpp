#include "presentation/occ/RebarAisPresentationAdapter.h"

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <GC_MakeArcOfCircle.hxx>
#include <Precision.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Failure.hxx>
#include <TopoDS_Edge.hxx>
#include <gp_Pnt.hxx>

#include <cmath>
#include <utility>

namespace tsrebar {
namespace {

bool finitePoint(const DomainPoint3d& point)
{
    return std::isfinite(point.x) && std::isfinite(point.y) && std::isfinite(point.z);
}

gp_Pnt toGpPoint(const DomainPoint3d& point)
{
    return gp_Pnt(point.x, point.y, point.z);
}

double squaredDistance(const DomainPoint3d& lhs, const DomainPoint3d& rhs)
{
    const double dx = lhs.x - rhs.x;
    const double dy = lhs.y - rhs.y;
    const double dz = lhs.z - rhs.z;
    return dx * dx + dy * dy + dz * dz;
}

bool pointsDistinct(const DomainPoint3d& lhs, const DomainPoint3d& rhs)
{
    return squaredDistance(lhs, rhs) > Precision::Confusion() * Precision::Confusion();
}

const SteelBarGroup* findGroup(const SteelData& steelData, const std::string& groupId)
{
    for (const SteelBarGroup& group : steelData.groups) {
        if (group.groupId == groupId || group.id == groupId) {
            return &group;
        }
    }
    return nullptr;
}

const SteelBar* findBar(const SteelData& steelData, const std::string& barId)
{
    for (const SteelBar& bar : steelData.bars) {
        if (bar.barId == barId || bar.id == barId) {
            return &bar;
        }
    }
    return nullptr;
}

const SteelBarSegment* findSegment(const SteelData& steelData, const std::string& segmentId)
{
    for (const SteelBarSegment& segment : steelData.segments) {
        if (segment.segmentId == segmentId || segment.id == segmentId) {
            return &segment;
        }
    }
    return nullptr;
}

RebarAisDisplayItem rejectItem(const SteelBarSegment& segment, QString diagnostic)
{
    RebarAisDisplayItem item;
    item.segmentId = segment.segmentId;
    item.shapeType = segment.shapeType;
    item.diagnostic = std::move(diagnostic);
    return item;
}

RebarAisDisplayItem buildLineItem(const SteelBarSegment& segment)
{
    if (!finitePoint(segment.startPoint) || !finitePoint(segment.endPoint)) {
        return rejectItem(segment, QStringLiteral("line segment endpoints must be finite"));
    }
    if (!pointsDistinct(segment.startPoint, segment.endPoint)) {
        return rejectItem(segment, QStringLiteral("line segment endpoints must be distinct"));
    }

    BRepBuilderAPI_MakeEdge edgeBuilder(toGpPoint(segment.startPoint),
                                        toGpPoint(segment.endPoint));
    if (!edgeBuilder.IsDone()) {
        return rejectItem(segment, QStringLiteral("line segment edge creation failed"));
    }

    RebarAisDisplayItem item;
    item.segmentId = segment.segmentId;
    item.shapeType = segment.shapeType;
    item.shape = edgeBuilder.Edge();
    item.aisShape = new AIS_Shape(item.shape);
    item.aisShape->SetColor(Quantity_Color(Quantity_NOC_RED));
    item.aisShape->SetWidth(3.0);
    return item;
}

RebarAisDisplayItem buildArcItem(const SteelBarSegment& segment)
{
    if (!finitePoint(segment.startPoint) || !finitePoint(segment.middlePoint) ||
        !finitePoint(segment.endPoint)) {
        return rejectItem(segment, QStringLiteral("arc segment points must be finite"));
    }
    if (!pointsDistinct(segment.startPoint, segment.middlePoint) ||
        !pointsDistinct(segment.middlePoint, segment.endPoint) ||
        !pointsDistinct(segment.startPoint, segment.endPoint)) {
        return rejectItem(segment, QStringLiteral("arc segment points must be distinct"));
    }

    GC_MakeArcOfCircle arcBuilder(toGpPoint(segment.startPoint),
                                  toGpPoint(segment.middlePoint),
                                  toGpPoint(segment.endPoint));
    if (!arcBuilder.IsDone()) {
        return rejectItem(segment, QStringLiteral("arc segment curve creation failed"));
    }

    BRepBuilderAPI_MakeEdge edgeBuilder(arcBuilder.Value());
    if (!edgeBuilder.IsDone()) {
        return rejectItem(segment, QStringLiteral("arc segment edge creation failed"));
    }

    RebarAisDisplayItem item;
    item.segmentId = segment.segmentId;
    item.shapeType = segment.shapeType;
    item.shape = edgeBuilder.Edge();
    item.aisShape = new AIS_Shape(item.shape);
    item.aisShape->SetColor(Quantity_Color(Quantity_NOC_RED));
    item.aisShape->SetWidth(3.0);
    return item;
}

RebarAisDisplayItem buildSegmentItem(const SteelBarSegment& segment)
{
    try {
        switch (segment.shapeType) {
        case SteelBarSegmentShape::Line:
            return buildLineItem(segment);
        case SteelBarSegmentShape::Arc:
            return buildArcItem(segment);
        case SteelBarSegmentShape::Unknown:
        case SteelBarSegmentShape::Point:
            return rejectItem(segment,
                              QStringLiteral("unsupported rebar segment shape for AIS display"));
        }
    } catch (const Standard_Failure& failure) {
        RebarAisDisplayItem item = rejectItem(
            segment,
            QStringLiteral("OCCT rebar display mapping failed: %1")
                .arg(QString::fromUtf8(failure.GetMessageString())));
        return item;
    } catch (...) {
        return rejectItem(segment,
                          QStringLiteral("OCCT rebar display mapping failed: unknown exception"));
    }
    return rejectItem(segment, QStringLiteral("unsupported rebar segment shape for AIS display"));
}

} // namespace

RebarAisPresentationResult RebarAisPresentationAdapter::buildGroupPresentation(
    const SteelData& steelData,
    const std::string& groupId) const
{
    RebarAisPresentationResult result;
    result.groupId = groupId;

    const SteelBarGroup* group = findGroup(steelData, groupId);
    if (group == nullptr) {
        result.diagnostic = QStringLiteral("missing group for AIS rebar presentation");
        return result;
    }
    if (!group->active || !group->visible) {
        result.diagnostic = QStringLiteral("group is not displayable");
        return result;
    }

    QString firstFailure;
    for (const std::string& barId : group->barIds) {
        const SteelBar* bar = findBar(steelData, barId);
        if (bar == nullptr || !bar->active || !bar->visible) {
            if (firstFailure.isEmpty()) {
                firstFailure = QStringLiteral("group contains missing or hidden bar");
            }
            continue;
        }

        for (const std::string& segmentId : bar->segmentIds) {
            const SteelBarSegment* segment = findSegment(steelData, segmentId);
            if (segment == nullptr) {
                if (firstFailure.isEmpty()) {
                    firstFailure =
                        QStringLiteral("group contains missing segment reference");
                }
                continue;
            }

            RebarAisDisplayItem item = buildSegmentItem(*segment);
            item.groupId = group->groupId;
            item.barId = bar->barId;
            if (item.segmentId.empty()) {
                item.segmentId = segment->segmentId;
            }
            if (item.aisShape.IsNull() || item.shape.IsNull()) {
                if (firstFailure.isEmpty()) {
                    firstFailure = item.diagnostic;
                }
                continue;
            }
            result.items.push_back(std::move(item));
        }
    }

    if (result.items.empty()) {
        result.diagnostic = firstFailure.isEmpty()
                                ? QStringLiteral("group has no displayable segments")
                                : QStringLiteral("group has no displayable segments: %1")
                                      .arg(firstFailure);
        return result;
    }

    result.ok = true;
    return result;
}

} // namespace tsrebar
