#pragma once

#include <string>

namespace tsrebar {

struct SteelBarSegment
{
    std::string id;
    std::string segmentType;
    double lengthMm = 0.0;
};

} // namespace tsrebar

