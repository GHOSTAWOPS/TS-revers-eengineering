#pragma once

#include "domain/rebar/SteelBar.h"

#include <string>
#include <vector>

namespace tsrebar {

struct SteelBarGroup
{
    std::string id;
    std::string legacyCommand;
    std::vector<SteelBar> bars;
};

} // namespace tsrebar

