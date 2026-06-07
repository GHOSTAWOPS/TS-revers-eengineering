#pragma once

#include <string>
#include <vector>

namespace tsrebar {

struct DomainPoint3d
{
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
};

struct RebarEvidenceRef
{
    std::string evidenceId;
    std::string note;
};

struct LegacyRawField
{
    std::string name;
    std::string value;
    std::string evidenceId;
};

struct LegacyRawBlock
{
    std::string legacyType;
    std::vector<std::string> writeOrder;
    std::vector<LegacyRawField> fields;
};

struct UnresolvedLegacyField
{
    std::string fieldName;
    std::string reason;
    std::string gapId;
};

enum class RebarFaceEdgeShape {
    Unknown,
    Line,
    Arc
};

inline std::string detailFaceEdgeShapeTypeCode(RebarFaceEdgeShape shape)
{
    switch (shape) {
    case RebarFaceEdgeShape::Line:
        return "L";
    case RebarFaceEdgeShape::Arc:
        return "A";
    case RebarFaceEdgeShape::Unknown:
        return {};
    }
    return {};
}

struct RebarFaceEdgeGeometry
{
    RebarFaceEdgeShape shapeType = RebarFaceEdgeShape::Unknown;
    DomainPoint3d startPoint;
    DomainPoint3d middlePoint;
    DomainPoint3d endPoint;
    bool arcDotReverse = false;
    std::vector<UnresolvedLegacyField> unresolvedLegacyFields;
    std::vector<RebarEvidenceRef> evidence;

    [[nodiscard]] std::string detailShapeTypeCode() const
    {
        return detailFaceEdgeShapeTypeCode(shapeType);
    }
};

struct GeometryReference
{
    std::vector<std::string> faceStableIds;
    std::vector<std::string> edgeStableIds;
    std::vector<std::string> curveStableIds;
};

enum class BindingState {
    Resolved,
    UnresolvedGeometry,
    PendingLegacyEvidence
};

struct BindingItem
{
    std::string legacyPath;
    std::string geometryPath;
    std::string evidenceId;
};

struct BindingAnchor
{
    BindingState state = BindingState::PendingLegacyEvidence;
    std::vector<BindingItem> items;
};

enum class SteelBarSegmentShape {
    Unknown,
    Line,
    Arc,
    Point
};

inline std::string detailShapeTypeCode(SteelBarSegmentShape shape)
{
    switch (shape) {
    case SteelBarSegmentShape::Line:
        return "L";
    case SteelBarSegmentShape::Arc:
        return "A";
    case SteelBarSegmentShape::Point:
        return "C";
    case SteelBarSegmentShape::Unknown:
        return {};
    }
    return {};
}

} // namespace tsrebar
