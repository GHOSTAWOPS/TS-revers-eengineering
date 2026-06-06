#pragma once

#include "domain/rebar/SteelData.h"

#include <AIS_Shape.hxx>
#include <TopoDS_Shape.hxx>

#include <QString>

#include <string>
#include <vector>

namespace tsrebar {

struct RebarAisDisplayItem
{
    std::string groupId;
    std::string barId;
    std::string segmentId;
    SteelBarSegmentShape shapeType = SteelBarSegmentShape::Unknown;
    TopoDS_Shape shape;
    Handle(AIS_Shape) aisShape;
    QString diagnostic;
};

struct RebarAisPresentationResult
{
    bool ok = false;
    std::string groupId;
    std::vector<RebarAisDisplayItem> items;
    QString diagnostic;
};

class RebarAisPresentationAdapter
{
public:
    [[nodiscard]] RebarAisPresentationResult buildGroupPresentation(
        const SteelData& steelData,
        const std::string& groupId) const;
};

} // namespace tsrebar
