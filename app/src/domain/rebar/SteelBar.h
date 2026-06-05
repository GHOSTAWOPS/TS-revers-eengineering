#pragma once

#include "domain/rebar/SteelBarSegment.h"

#include <string>
#include <vector>

namespace tsrebar {

struct SteelBar
{
    std::string id;
    std::string diameter;
    std::vector<SteelBarSegment> segments;
};

} // namespace tsrebar

