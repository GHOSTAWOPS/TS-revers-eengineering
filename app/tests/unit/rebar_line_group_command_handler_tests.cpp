#include "command/CommandRegistry.h"
#include "command/RebarLineGroupCommandHandler.h"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {

void expect(bool condition, const char* message)
{
    if (!condition) {
        std::cerr << message << '\n';
        std::exit(1);
    }
}

bool near(double left, double right)
{
    return std::abs(left - right) < 1.0e-9;
}

tsrebar::LegacySelectionRef edgeRef(const std::string& stableId)
{
    tsrebar::LegacySelectionRef ref;
    ref.stableId = stableId;
    ref.shapeKind = tsrebar::LegacyShapeKind::Edge;
    return ref;
}

tsrebar::LegacySelectionRef faceRef(const std::string& stableId)
{
    tsrebar::LegacySelectionRef ref;
    ref.stableId = stableId;
    ref.shapeKind = tsrebar::LegacyShapeKind::Face;
    return ref;
}

class FakeLegacyRebarGeometryReader final : public tsrebar::LegacyRebarGeometryReader
{
public:
    void addCurve(tsrebar::LegacyRebarCurveSnapshot curve)
    {
        curves_.insert_or_assign(curve.stableId, std::move(curve));
    }

    [[nodiscard]] tsrebar::LegacyGeometryQueryResult<tsrebar::LegacyRebarCurveSnapshot>
    curveSnapshot(const tsrebar::LegacySelectionRef& ref, int requestedSampleCount) const override
    {
        ++curveSnapshotCallCount_;
        lastRequestedSampleCount_ = requestedSampleCount;

        tsrebar::LegacyGeometryQueryResult<tsrebar::LegacyRebarCurveSnapshot> result;
        const auto found = curves_.find(ref.stableId);
        if (found == curves_.end()) {
            result.diagnostic = QStringLiteral("fake curve not found");
            return result;
        }

        result.ok = true;
        result.value = found->second;
        return result;
    }

    [[nodiscard]] tsrebar::LegacyGeometryQueryResult<tsrebar::LegacyRebarCurveSnapshot>
    normalizeSegmentCurve(
        const tsrebar::LegacyRebarCurveSnapshot& curve,
        const tsrebar::LegacySegmentCurveNormalizeRequest& request) const override
    {
        ++normalizeCallCount_;
        lastNormalizeRequest_ = request;

        tsrebar::LegacyGeometryQueryResult<tsrebar::LegacyRebarCurveSnapshot> result;
        if (!normalizeFailure_.isEmpty()) {
            result.diagnostic = normalizeFailure_;
            return result;
        }
        result.ok = true;
        result.value = normalizedCurve_.stableId.empty() ? curve : normalizedCurve_;
        return result;
    }

    void setNormalizedCurve(tsrebar::LegacyRebarCurveSnapshot curve)
    {
        normalizedCurve_ = std::move(curve);
    }

    void failNormalize(QString diagnostic)
    {
        normalizeFailure_ = std::move(diagnostic);
    }

    [[nodiscard]] int curveSnapshotCallCount() const { return curveSnapshotCallCount_; }
    [[nodiscard]] int normalizeCallCount() const { return normalizeCallCount_; }
    [[nodiscard]] int lastRequestedSampleCount() const { return lastRequestedSampleCount_; }
    [[nodiscard]] const tsrebar::LegacySegmentCurveNormalizeRequest& lastNormalizeRequest() const
    {
        return lastNormalizeRequest_;
    }

private:
    mutable int curveSnapshotCallCount_ = 0;
    mutable int normalizeCallCount_ = 0;
    mutable int lastRequestedSampleCount_ = 0;
    mutable tsrebar::LegacySegmentCurveNormalizeRequest lastNormalizeRequest_;
    QString normalizeFailure_;
    std::unordered_map<std::string, tsrebar::LegacyRebarCurveSnapshot> curves_;
    tsrebar::LegacyRebarCurveSnapshot normalizedCurve_;
};

tsrebar::LegacyRebarCurveSnapshot lineCurve(const std::string& stableId, double length)
{
    tsrebar::LegacyRebarCurveSnapshot curve;
    curve.stableId = stableId;
    curve.curveKind = tsrebar::LegacyCurveKind::Line;
    curve.length = length;
    curve.startPoint = {0.0, 0.0, 0.0};
    curve.endPoint = {length, 0.0, 0.0};
    curve.samplePoints = {
        curve.startPoint,
        {length * 0.5, 0.0, 0.0},
        curve.endPoint,
    };
    return curve;
}

tsrebar::RebarLineGroupCommandParameters commandParameters()
{
    tsrebar::RebarLineGroupCommandParameters parameters;
    parameters.groupId = "cmd-line-group-001";
    parameters.barId = "cmd-line-bar-001";
    parameters.segmentId = "cmd-line-segment-001";
    parameters.steelDataId = "cmd-steel-data-001";
    parameters.distanceA = 0.25;
    parameters.distanceB = 1.2;
    parameters.legacyFlag = 0;
    parameters.diameter = 25.0;
    parameters.interval = 200.0;
    parameters.requestedBarCount = 3;
    parameters.steelLevel = "HRB400";
    parameters.rsdId = "Y12";
    parameters.componentName = "pier";
    parameters.projectSteelName = "main bar";
    return parameters;
}

void testLineGroupHandlerRejectsEmptySelectionWithoutGeometryQuery()
{
    std::vector<tsrebar::LegacySelectionRef> selection;
    FakeLegacyRebarGeometryReader reader;
    tsrebar::SteelData model;

    auto handler = std::make_shared<tsrebar::RebarLineGroupCommandHandler>(
        [&selection]() { return selection; },
        reader,
        model,
        commandParameters());

    tsrebar::CommandRegistry registry;
    tsrebar::registerRebarLineGroupCommandHandler(registry, handler);

    const tsrebar::CommandResult result =
        registry.execute(tsrebar::CommandId::RebarLineCreate);

    expect(result.status == tsrebar::CommandStatus::Failed,
           "empty selection must fail the command");
    expect(result.message.contains(QStringLiteral("exactly one")),
           "empty selection diagnostic must mention single selection");
    expect(reader.curveSnapshotCallCount() == 0,
           "empty selection must not query legacy geometry");
    expect(model.groups.empty(), "empty selection must not mutate steel data");
}

void testLineGroupHandlerRejectsWrongSelectionType()
{
    std::vector<tsrebar::LegacySelectionRef> selection = {
        faceRef("selection-v1:face:3"),
    };
    FakeLegacyRebarGeometryReader reader;
    tsrebar::SteelData model;

    tsrebar::RebarLineGroupCommandHandler handler(
        [&selection]() { return selection; },
        reader,
        model,
        commandParameters());

    const tsrebar::CommandResult result = handler.execute();

    expect(result.status == tsrebar::CommandStatus::Failed,
           "wrong selection type must fail the command");
    expect(result.message.contains(QStringLiteral("edge")),
           "wrong type diagnostic must mention edge");
    expect(reader.curveSnapshotCallCount() == 0,
           "wrong selection type must not query legacy geometry");
    expect(model.groups.empty(), "wrong selection type must not mutate steel data");
}

void testLineGroupHandlerCreatesSteelDataFromValidEdge()
{
    std::vector<tsrebar::LegacySelectionRef> selection = {
        edgeRef("selection-v1:edge:17"),
    };

    FakeLegacyRebarGeometryReader reader;
    reader.addCurve(lineCurve("selection-v1:edge:17", 12.5));
    auto normalized = lineCurve("selection-v1:edge:17", 11.5);
    normalized.startPoint = {0.5, 0.0, 0.0};
    normalized.endPoint = {12.0, 0.0, 0.0};
    reader.setNormalizedCurve(normalized);

    tsrebar::SteelData model;
    tsrebar::RebarLineGroupCommandHandler handler(
        [&selection]() { return selection; },
        reader,
        model,
        commandParameters());

    const tsrebar::CommandResult result = handler.execute();

    expect(result.status == tsrebar::CommandStatus::Completed,
           "valid edge must complete the command");
    expect(result.message.contains(QStringLiteral("Rebar.Create.LineGroup")),
           "success message must mention the command key");
    expect(model.groups.size() == 1, "valid edge must append one group");
    expect(model.bars.size() == 1, "valid edge must append one bar");
    expect(model.segments.size() == 1, "valid edge must append one segment");
    expect(model.groups.front().createCommand == "Rebar.Create.LineGroup",
           "created group command mismatch");
    expect(model.groups.front().legacyCommand == "sgroupbarline",
           "created group legacy command mismatch");
    expect(!model.groups.front().sourceCurveIds.empty(),
           "created group must expose source curve ids");
    expect(model.groups.front().sourceCurveIds.front() == "selection-v1:edge:17",
           "created group must keep selected legacy edge");
    expect(model.groups.front().barCount == 3,
           "handler must pass parameter bar count to RebarGroupCreator");
    expect(near(model.segments.front().length, 11.5),
           "handler must use normalized legacy curve output");
    expect(reader.curveSnapshotCallCount() == 1,
           "valid edge must query legacy curve once");
    expect(reader.normalizeCallCount() == 1,
           "valid edge must enter SegmentCurveNormalizer P0 once");
    expect(reader.lastRequestedSampleCount() == 5,
           "handler must preserve old minimum spline sample request");
    expect(near(reader.lastNormalizeRequest().trimDelta, -0.03),
           "handler path must preserve VisualTS trim delta through creator");
}

void testLineGroupHandlerUsesUpdatedParameters()
{
    std::vector<tsrebar::LegacySelectionRef> selection = {
        edgeRef("selection-v1:edge:17"),
    };

    FakeLegacyRebarGeometryReader reader;
    reader.addCurve(lineCurve("selection-v1:edge:17", 12.5));

    tsrebar::SteelData model;
    tsrebar::RebarLineGroupCommandHandler handler(
        [&selection]() { return selection; },
        reader,
        model,
        commandParameters());

    tsrebar::RebarLineGroupCommandParameters updated = commandParameters();
    updated.groupId = "cmd-line-group-002";
    updated.barId = "cmd-line-bar-002";
    updated.segmentId = "cmd-line-segment-002";
    updated.requestedBarCount = 2;
    handler.setParameters(updated);

    const tsrebar::CommandResult result = handler.execute();

    expect(result.status == tsrebar::CommandStatus::Completed,
           "updated parameters must still complete the command");
    expect(model.groups.size() == 1, "updated parameters must create one group");
    expect(model.groups.front().groupId == "cmd-line-group-002",
           "handler must use updated group id");
    expect(model.bars.front().barId == "cmd-line-bar-002",
           "handler must use updated bar id");
    expect(model.segments.front().segmentId == "cmd-line-segment-002",
           "handler must use updated segment id");
    expect(model.groups.front().barCount == 2,
           "handler must use updated bar count");
}

void testLineGroupHandlerLeavesModelUntouchedWhenGeometryFails()
{
    std::vector<tsrebar::LegacySelectionRef> selection = {
        edgeRef("selection-v1:edge:missing"),
    };
    FakeLegacyRebarGeometryReader reader;
    tsrebar::SteelData model;

    tsrebar::RebarLineGroupCommandHandler handler(
        [&selection]() { return selection; },
        reader,
        model,
        commandParameters());

    const tsrebar::CommandResult result = handler.execute();

    expect(result.status == tsrebar::CommandStatus::Failed,
           "missing legacy curve must fail the command");
    expect(result.message.contains(QStringLiteral("fake curve not found")),
           "geometry failure diagnostic must pass through");
    expect(reader.curveSnapshotCallCount() == 1,
           "valid edge must reach legacy geometry before failure");
    expect(reader.normalizeCallCount() == 0,
           "missing curve must fail before normalizer");
    expect(model.groups.empty() && model.bars.empty() && model.segments.empty(),
           "geometry failure must not mutate steel data");
}

void testLineGroupHandlerLeavesModelUntouchedWhenNormalizerFails()
{
    std::vector<tsrebar::LegacySelectionRef> selection = {
        edgeRef("selection-v1:edge:17"),
    };
    FakeLegacyRebarGeometryReader reader;
    reader.addCurve(lineCurve("selection-v1:edge:17", 12.5));
    reader.failNormalize(QStringLiteral("fake normalizer failed"));
    tsrebar::SteelData model;

    tsrebar::RebarLineGroupCommandHandler handler(
        [&selection]() { return selection; },
        reader,
        model,
        commandParameters());

    const tsrebar::CommandResult result = handler.execute();

    expect(result.status == tsrebar::CommandStatus::Failed,
           "normalizer failure must fail the command");
    expect(result.message.contains(QStringLiteral("fake normalizer failed")),
           "normalizer failure diagnostic must pass through");
    expect(reader.curveSnapshotCallCount() == 1,
           "normalizer failure path must read legacy curve once");
    expect(reader.normalizeCallCount() == 1,
           "normalizer failure path must call normalizer once");
    expect(model.groups.empty() && model.bars.empty() && model.segments.empty(),
           "normalizer failure must not mutate steel data");
}

} // namespace

int main()
{
    testLineGroupHandlerRejectsEmptySelectionWithoutGeometryQuery();
    testLineGroupHandlerRejectsWrongSelectionType();
    testLineGroupHandlerCreatesSteelDataFromValidEdge();
    testLineGroupHandlerUsesUpdatedParameters();
    testLineGroupHandlerLeavesModelUntouchedWhenGeometryFails();
    testLineGroupHandlerLeavesModelUntouchedWhenNormalizerFails();
    return 0;
}
