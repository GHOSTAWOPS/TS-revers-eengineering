#include "command/RebarLineGroupCommandHandler.h"

#include <algorithm>
#include <iterator>
#include <utility>

namespace tsrebar {
namespace {

CommandResult failed(QString message)
{
    return {CommandStatus::Failed, std::move(message)};
}

RebarGroupCreationRequest toCreationRequest(
    const RebarLineGroupCommandParameters& parameters,
    const LegacySelectionRef& sourceCurve)
{
    RebarGroupCreationRequest request;
    request.groupId = parameters.groupId;
    request.barId = parameters.barId;
    request.segmentId = parameters.segmentId;
    request.steelDataId = parameters.steelDataId;
    request.sourceCurve = sourceCurve;
    request.distanceA = parameters.distanceA;
    request.distanceB = parameters.distanceB;
    request.legacyFlag = parameters.legacyFlag;
    request.diameter = parameters.diameter;
    request.interval = parameters.interval;
    request.requestedBarCount = parameters.requestedBarCount;
    request.steelLevel = parameters.steelLevel;
    request.rsdId = parameters.rsdId;
    request.componentName = parameters.componentName;
    request.projectSteelName = parameters.projectSteelName;
    return request;
}

void appendCreatedSteelData(SteelData& target, SteelData&& created)
{
    if (target.steelDataId.empty()) {
        target.steelDataId = std::move(created.steelDataId);
    }
    if (target.level.empty()) {
        target.level = std::move(created.level);
    }
    if (target.gradeName.empty()) {
        target.gradeName = std::move(created.gradeName);
    }

    target.diameterSet.insert(target.diameterSet.end(),
                              created.diameterSet.begin(),
                              created.diameterSet.end());
    target.evidence.insert(target.evidence.end(),
                           std::make_move_iterator(created.evidence.begin()),
                           std::make_move_iterator(created.evidence.end()));
    target.unresolvedLegacyFields.insert(
        target.unresolvedLegacyFields.end(),
        std::make_move_iterator(created.unresolvedLegacyFields.begin()),
        std::make_move_iterator(created.unresolvedLegacyFields.end()));
    target.segments.insert(target.segments.end(),
                           std::make_move_iterator(created.segments.begin()),
                           std::make_move_iterator(created.segments.end()));
    target.bars.insert(target.bars.end(),
                       std::make_move_iterator(created.bars.begin()),
                       std::make_move_iterator(created.bars.end()));
    target.groups.insert(target.groups.end(),
                         std::make_move_iterator(created.groups.begin()),
                         std::make_move_iterator(created.groups.end()));
}

} // namespace

RebarLineGroupCommandHandler::RebarLineGroupCommandHandler(
    SelectionProvider selectionProvider,
    const LegacyRebarGeometryReader& geometry,
    SteelData& steelData,
    RebarLineGroupCommandParameters parameters)
    : m_selectionProvider(std::move(selectionProvider))
    , m_geometry(geometry)
    , m_steelData(steelData)
    , m_parameters(std::move(parameters))
{
}

void RebarLineGroupCommandHandler::setParameters(RebarLineGroupCommandParameters parameters)
{
    m_parameters = std::move(parameters);
}

CommandResult RebarLineGroupCommandHandler::execute() const
{
    if (!m_selectionProvider) {
        return failed(QStringLiteral(
            "Rebar.Create.LineGroup requires a LegacySelectionRef provider."));
    }

    const std::vector<LegacySelectionRef> selection = m_selectionProvider();
    if (selection.size() != 1) {
        return failed(QStringLiteral(
            "Rebar.Create.LineGroup requires exactly one LegacySelectionRef."));
    }

    const LegacySelectionRef& sourceCurve = selection.front();
    if (sourceCurve.shapeKind != LegacyShapeKind::Edge || sourceCurve.stableId.empty()) {
        return failed(QStringLiteral(
            "Rebar.Create.LineGroup requires an edge LegacySelectionRef."));
    }

    RebarGroupCreationResult created =
        m_creator.createLineGroup(toCreationRequest(m_parameters, sourceCurve), m_geometry);
    if (!created.ok) {
        return failed(created.diagnostic.isEmpty()
                          ? QStringLiteral("Rebar.Create.LineGroup creation failed.")
                          : created.diagnostic);
    }

    const int groupCount = static_cast<int>(created.steelData.groups.size());
    appendCreatedSteelData(m_steelData, std::move(created.steelData));

    return {CommandStatus::Completed,
            QStringLiteral("Rebar.Create.LineGroup created %1 SteelBarGroup(s).")
                .arg(groupCount)};
}

void registerRebarLineGroupCommandHandler(CommandRegistry& registry,
                                          std::shared_ptr<RebarLineGroupCommandHandler> handler)
{
    registry.registerHandler(CommandId::RebarLineCreate, [handler = std::move(handler)]() {
        return handler->execute();
    });
}

} // namespace tsrebar
