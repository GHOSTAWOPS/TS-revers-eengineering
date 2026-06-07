#pragma once

#include "domain/rebar/SteelData.h"

#include <string>
#include <vector>

namespace tsrebar {

struct RebarScheduleSegmentRef
{
    std::string segmentId;
};

struct RebarScheduleRow
{
    std::string rsdId;
    std::string componentName;
    std::string steelWay;
    double diameter = 0.0;
    double length = 0.0;
    int segmentCount = 0;
    int sameGroupCount = 1;
    int barNumberSum = 0;
    double lengthSum = 0.0;
    std::string steelLevel;
    std::string layer;
    std::string profile;
    std::string use;
    std::vector<RebarScheduleSegmentRef> segments;
};

struct RebarMaterialRow
{
    double diameter = 0.0;
    double lengthSum = 0.0;
    int countSum = 0;
    double singleMass = 0.0;
    double massSum = 0.0;
    std::string steelLevel;
    bool massDeferred = true;
};

struct RebarScheduleDiagnostic
{
    std::string code;
    std::string message;
};

struct RebarSchedule
{
    std::vector<RebarScheduleRow> scheduleRows;
    std::vector<RebarMaterialRow> materialRows;
    double totalMass = 0.0;
    double volume722 = 0.0;
    int massNum = 0;
    bool massFormulaDeferred = false;
    std::vector<RebarScheduleDiagnostic> diagnostics;
};

class RebarScheduleService
{
public:
    [[nodiscard]] RebarSchedule buildSchedule(const SteelData& steelData) const;
};

} // namespace tsrebar
