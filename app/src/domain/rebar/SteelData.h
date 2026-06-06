#pragma once

#include "domain/rebar/RebarDomainTypes.h"
#include "domain/rebar/SteelBar.h"
#include "domain/rebar/SteelBarGroup.h"
#include "domain/rebar/SteelBarSegment.h"

#include <string>
#include <vector>

namespace tsrebar {

struct SteelData
{
    std::string steelDataId;
    std::string level;
    std::string gradeName;
    std::vector<double> diameterSet;
    std::string defaultHook;
    std::string defaultBend;
    LegacyRawBlock legacyRaw;
    BindingAnchor binding;
    std::vector<UnresolvedLegacyField> unresolvedLegacyFields;
    std::vector<RebarEvidenceRef> evidence;
    std::vector<SteelBarSegment> segments;
    std::vector<SteelBar> bars;
    std::vector<SteelBarGroup> groups;
};

} // namespace tsrebar
