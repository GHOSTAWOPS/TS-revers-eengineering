#pragma once

#include "domain/rebar/RebarDomainTypes.h"

#include <string>
#include <vector>

namespace tsrebar {

struct SteelBarSegment
{
    std::string id; // Backward-compatible alias until older callers move to segmentId.
    std::string segmentId;
    std::string barId;
    int sequenceNo = 0;
    std::string segmentType;
    SteelBarSegmentShape shapeType = SteelBarSegmentShape::Unknown;
    DomainPoint3d startPoint;
    DomainPoint3d endPoint;
    DomainPoint3d middlePoint;
    double startRadius = 0.0;
    double endRadius = 0.0;
    DomainPoint3d offset;
    double length = 0.0;
    double lengthMm = 0.0;
    LegacyRawBlock legacyRaw;
    GeometryReference geometryRef;
    BindingAnchor binding;
    std::vector<UnresolvedLegacyField> unresolvedLegacyFields;
    std::vector<RebarEvidenceRef> evidence;

    [[nodiscard]] std::string detailShapeTypeCode() const
    {
        return tsrebar::detailShapeTypeCode(shapeType);
    }
};

} // namespace tsrebar
