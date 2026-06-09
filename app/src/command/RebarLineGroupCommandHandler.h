#pragma once

#include "command/CommandRegistry.h"
#include "domain/rebar/RebarGroupCreator.h"

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace tsrebar {

struct RebarLineGroupCommandParameters
{
    std::string groupId;
    std::string barId;
    std::string segmentId;
    std::string steelDataId;
    double distanceA = 0.0;
    double distanceB = 0.0;
    char legacyFlag = 0;
    double diameter = 0.0;
    double interval = 0.0;
    int requestedBarCount = 1;
    std::string steelLevel;
    std::string rsdId;
    std::string componentName;
    std::string projectSteelName;
};

class RebarLineGroupCommandHandler
{
public:
    using SelectionProvider = std::function<std::vector<LegacySelectionRef>()>;

    RebarLineGroupCommandHandler(SelectionProvider selectionProvider,
                                 const LegacyRebarGeometryReader& geometry,
                                 SteelData& steelData,
                                 RebarLineGroupCommandParameters parameters);

    [[nodiscard]] CommandResult execute() const;

private:
    SelectionProvider m_selectionProvider;
    const LegacyRebarGeometryReader& m_geometry;
    SteelData& m_steelData;
    RebarLineGroupCommandParameters m_parameters;
    RebarGroupCreator m_creator;
};

void registerRebarLineGroupCommandHandler(CommandRegistry& registry,
                                          std::shared_ptr<RebarLineGroupCommandHandler> handler);

} // namespace tsrebar
