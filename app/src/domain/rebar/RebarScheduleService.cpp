#include "domain/rebar/RebarScheduleService.h"

#include <algorithm>
#include <cmath>
#include <map>
#include <optional>
#include <string_view>
#include <utility>

namespace tsrebar {
namespace {

constexpr auto kMissingReference = "RS001_MISSING_REFERENCE";
constexpr auto kMassFormulaDeferred = "RS002_MATERIAL_MASS_FORMULA_DEFERRED";

std::string stableId(const SteelBar& bar)
{
    return bar.barId.empty() ? bar.id : bar.barId;
}

std::string stableId(const SteelBarSegment& segment)
{
    return segment.segmentId.empty() ? segment.id : segment.segmentId;
}

std::string stableId(const SteelBarGroup& group)
{
    return group.groupId.empty() ? group.id : group.groupId;
}

std::map<std::string, const SteelBar*> barsById(const SteelData& steelData)
{
    std::map<std::string, const SteelBar*> result;
    for (const SteelBar& bar : steelData.bars) {
        result.emplace(stableId(bar), &bar);
    }
    return result;
}

std::map<std::string, const SteelBarSegment*> segmentsById(const SteelData& steelData)
{
    std::map<std::string, const SteelBarSegment*> result;
    for (const SteelBarSegment& segment : steelData.segments) {
        result.emplace(stableId(segment), &segment);
    }
    return result;
}

std::vector<const SteelBar*> groupBars(const SteelBarGroup& group,
                                       const std::map<std::string, const SteelBar*>& byId)
{
    std::vector<const SteelBar*> result;
    for (const std::string& barId : group.barIds) {
        const auto it = byId.find(barId);
        if (it != byId.end()) {
            result.push_back(it->second);
        }
    }
    return result;
}

std::vector<const SteelBarSegment*> barSegments(
    const SteelBar& bar,
    const std::map<std::string, const SteelBarSegment*>& byId)
{
    std::vector<const SteelBarSegment*> result;
    for (const std::string& segmentId : bar.segmentIds) {
        const auto it = byId.find(segmentId);
        if (it != byId.end()) {
            result.push_back(it->second);
        }
    }
    return result;
}

double barLength(const SteelBar& bar,
                 const std::map<std::string, const SteelBarSegment*>& segmentIndex)
{
    if (bar.length > 0.0) {
        return bar.length;
    }

    double total = 0.0;
    for (const SteelBarSegment* segment : barSegments(bar, segmentIndex)) {
        total += segment->length;
    }
    return total;
}

int barCountFor(const SteelBarGroup& group, int actualBarRefs)
{
    return group.barCount > 0 ? group.barCount : actualBarRefs;
}

int segmentCountFor(const SteelBarGroup& group,
                    const std::vector<const SteelBar*>& bars,
                    const std::map<std::string, const SteelBarSegment*>& segmentIndex)
{
    if (group.segmentCount > 0) {
        return group.segmentCount;
    }
    if (bars.empty()) {
        return 0;
    }
    return static_cast<int>(barSegments(*bars.front(), segmentIndex).size());
}

void addDiagnostic(RebarSchedule& schedule, std::string code, std::string message)
{
    schedule.diagnostics.push_back({std::move(code), std::move(message)});
}

std::optional<double> legacyRawDouble(const LegacyRawBlock& raw, std::string_view fieldName)
{
    for (const LegacyRawField& field : raw.fields) {
        if (field.name == fieldName) {
            try {
                return std::stod(field.value);
            } catch (...) {
                return std::nullopt;
            }
        }
    }
    return std::nullopt;
}

std::optional<double> singleMassFor(const SteelBarGroup& group)
{
    if (const std::optional<double> fromGroup = legacyRawDouble(group.legacyRaw, "singleMass")) {
        return fromGroup;
    }
    if (const std::optional<double> fromGroup = legacyRawDouble(group.createdFromParameters, "singleMass")) {
        return fromGroup;
    }
    return std::nullopt;
}

double rounded3(double value)
{
    return std::round(value * 1000.0) / 1000.0;
}

struct MaterialKey
{
    double diameter = 0.0;
    std::string steelLevel;

    bool operator<(const MaterialKey& other) const
    {
        if (diameter != other.diameter) {
            return diameter < other.diameter;
        }
        return steelLevel < other.steelLevel;
    }
};

} // namespace

RebarSchedule RebarScheduleService::buildSchedule(const SteelData& steelData) const
{
    RebarSchedule result;
    const auto barIndex = barsById(steelData);
    const auto segmentIndex = segmentsById(steelData);
    std::map<MaterialKey, RebarMaterialRow> materialRows;

    for (const SteelBarGroup& group : steelData.groups) {
        const std::vector<const SteelBar*> bars = groupBars(group, barIndex);
        if (bars.empty()) {
            addDiagnostic(result,
                          kMissingReference,
                          "SteelBarGroup " + stableId(group) +
                              " has no resolvable SteelBar references");
            continue;
        }

        const SteelBar& firstBar = *bars.front();
        const std::vector<const SteelBarSegment*> firstSegments =
            barSegments(firstBar, segmentIndex);
        if (firstSegments.empty()) {
            addDiagnostic(result,
                          kMissingReference,
                          "SteelBar " + stableId(firstBar) +
                              " has no resolvable SteelBarSegment references");
            continue;
        }

        const double length = barLength(firstBar, segmentIndex);
        const int barCount = barCountFor(group, static_cast<int>(bars.size()));

        RebarScheduleRow row;
        row.rsdId = group.rsdId;
        row.componentName = group.componentName;
        row.steelWay = group.steelWay;
        row.diameter = group.diameter > 0.0 ? group.diameter : firstBar.diameter;
        row.length = length;
        row.segmentCount = segmentCountFor(group, bars, segmentIndex);
        row.sameGroupCount = 1;
        row.barNumberSum = barCount;
        row.lengthSum = length * static_cast<double>(barCount);
        row.steelLevel = group.steelLevel.empty() ? firstBar.steelLevel : group.steelLevel;
        row.layer = group.layer;
        row.profile = group.profile;
        row.use = group.use;
        for (const SteelBarSegment* segment : firstSegments) {
            row.segments.push_back({stableId(*segment)});
        }
        result.scheduleRows.push_back(row);

        const MaterialKey key{row.diameter, row.steelLevel};
        RebarMaterialRow& material = materialRows[key];
        if (material.countSum == 0) {
            material.diameter = row.diameter;
            material.steelLevel = row.steelLevel;
            material.massDeferred = false;
        }
        material.lengthSum += row.lengthSum;
        material.countSum += row.barNumberSum;

        const std::optional<double> singleMass = singleMassFor(group);
        if (singleMass && *singleMass > 0.0) {
            if (material.singleMass == 0.0) {
                material.singleMass = *singleMass;
            }
            if (std::abs(material.singleMass - *singleMass) > 1.0e-9) {
                material.massDeferred = true;
                material.massSum = 0.0;
                result.massFormulaDeferred = true;
            } else if (!material.massDeferred) {
                material.massSum = rounded3(material.lengthSum * material.singleMass);
            }
        } else {
            material.massDeferred = true;
            material.massSum = 0.0;
            result.massFormulaDeferred = true;
        }
    }

    for (const auto& item : materialRows) {
        result.materialRows.push_back(item.second);
        if (!item.second.massDeferred) {
            result.totalMass += item.second.massSum;
        }
    }

    if (result.massFormulaDeferred) {
        for (RebarMaterialRow& row : result.materialRows) {
            row.massDeferred = true;
            row.massSum = 0.0;
        }
        result.totalMass = 0.0;
        addDiagnostic(result,
                      kMassFormulaDeferred,
                      "legacy singleMass source is not fully confirmed; P0 keeps material mass deferred");
    } else {
        result.totalMass = rounded3(result.totalMass / 1000.0);
    }

    return result;
}

} // namespace tsrebar
