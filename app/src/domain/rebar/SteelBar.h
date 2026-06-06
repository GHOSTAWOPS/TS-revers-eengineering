#pragma once

#include "domain/rebar/RebarDomainTypes.h"

#include <string>
#include <vector>

namespace tsrebar {

struct SteelBar
{
    std::string id; // Backward-compatible alias until older callers move to barId.
    std::string barId;
    std::string groupId;
    int sequenceNo = 0;
    std::string displayNumber;
    double diameter = 0.0;
    std::string steelLevel;
    std::vector<std::string> segmentIds;
    double length = 0.0;
    std::string shapeType;
    bool active = true;
    bool visible = true;
    LegacyRawBlock legacyRaw;
    GeometryReference geometryRef;
    BindingAnchor binding;
    std::vector<UnresolvedLegacyField> unresolvedLegacyFields;
    std::vector<RebarEvidenceRef> evidence;
};

} // namespace tsrebar
