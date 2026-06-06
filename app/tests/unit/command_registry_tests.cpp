#include "command/CommandRegistry.h"
#include "command/LegacyUiCommandMap.h"
#include "ui/MainTabs.h"

#include <iostream>

namespace {

bool fail(const char* message)
{
    std::cerr << message << '\n';
    return false;
}

bool testMainTabs()
{
    const auto tabs = tsrebar::mainTabDefinitions();
    if (tabs.size() != 5) {
        return fail("expected five main tabs");
    }

    const QStringList expected = {
        QStringLiteral("开始"),
        QStringLiteral("显示"),
        QStringLiteral("钢筋"),
        QStringLiteral("查询"),
        QStringLiteral("工程图"),
    };

    for (const QString& name : expected) {
        bool found = false;
        for (const auto& tab : tabs) {
            found = found || tab.title == name;
        }
        if (!found) {
            std::cerr << "missing tab: " << name.toStdString() << '\n';
            return false;
        }
    }

    return true;
}

bool testLegacyCommandMap()
{
    if (!tsrebar::hasLegacyUiCommand(tsrebar::CommandId::ExportRebarStep)) {
        return fail("ExportRebarStep command is not registered");
    }

    if (!tsrebar::hasLegacyUiCommand(tsrebar::CommandId::DrawingGeneratePackage)) {
        return fail("DrawingGeneratePackage command is not registered");
    }

    if (!tsrebar::hasLegacyUiCommand(tsrebar::CommandId::ViewSelectFace) ||
        !tsrebar::hasLegacyUiCommand(tsrebar::CommandId::ViewSelectEdge) ||
        !tsrebar::hasLegacyUiCommand(tsrebar::CommandId::ViewSelectVertex)) {
        return fail("face/edge/vertex selection commands are not registered");
    }

    return true;
}

bool testLegacyRebarCommandContracts()
{
    const auto lineGroup =
        tsrebar::legacyUiCommandDefinition(tsrebar::CommandId::RebarLineCreate);
    if (!lineGroup.has_value()) {
        return fail("Rebar.Create.LineGroup contract is missing");
    }
    if (lineGroup->commandKey != QStringLiteral("Rebar.Create.LineGroup") ||
        lineGroup->legacyCommand != QStringLiteral("sgroupbarline") ||
        lineGroup->legacyUiPath != QStringLiteral("钢筋/主要钢筋/线配筋")) {
        return fail("Rebar.Create.LineGroup contract metadata mismatch");
    }
    if (!lineGroup->evidenceIds.contains(QStringLiteral("E-IDA-002")) ||
        !lineGroup->evidenceIds.contains(QStringLiteral("E-IDA-010")) ||
        !lineGroup->gapIds.contains(QStringLiteral("GAP-IDA-002")) ||
        !lineGroup->inputSelectionTypes.contains(QStringLiteral("single-selected-entity-list"))) {
        return fail("Rebar.Create.LineGroup evidence, gap or input contract mismatch");
    }
    if (lineGroup->uiSurface != tsrebar::LegacyCommandUiSurface::Ribbon) {
        return fail("Rebar.Create.LineGroup must be a ribbon command");
    }
    if (lineGroup->implementationState !=
        tsrebar::LegacyCommandImplementationState::NotImplemented) {
        return fail("Rebar.Create.LineGroup must remain NotImplemented in M1-App-019");
    }
    if (!lineGroup->interactionStates.contains(tsrebar::LegacyCommandInteractionState::Idle) ||
        !lineGroup->interactionStates.contains(tsrebar::LegacyCommandInteractionState::Picking) ||
        !lineGroup->interactionStates.contains(tsrebar::LegacyCommandInteractionState::Completed)) {
        return fail("Rebar.Create.LineGroup interaction state contract mismatch");
    }

    const auto arcGroup =
        tsrebar::legacyUiCommandDefinition(tsrebar::CommandId::RebarArcGroupCreate);
    if (!arcGroup.has_value()) {
        return fail("Rebar.Create.ArcGroup contract is missing");
    }
    if (arcGroup->commandKey != QStringLiteral("Rebar.Create.ArcGroup") ||
        arcGroup->legacyCommand != QStringLiteral("sgroupbararc") ||
        arcGroup->legacyUiPath != QStringLiteral("钢筋/主要钢筋/扇形筋")) {
        return fail("Rebar.Create.ArcGroup contract metadata mismatch");
    }
    if (!arcGroup->evidenceIds.contains(QStringLiteral("E-IDA-001")) ||
        !arcGroup->gapIds.contains(QStringLiteral("GAP-IDA-001")) ||
        !arcGroup->inputSelectionTypes.contains(QStringLiteral("single-selected-chain"))) {
        return fail("Rebar.Create.ArcGroup evidence, gap or input contract mismatch");
    }

    const auto trimByLine =
        tsrebar::legacyUiCommandDefinition(tsrebar::CommandId::RebarGroupTrimByLine);
    const auto trimByFace =
        tsrebar::legacyUiCommandDefinition(tsrebar::CommandId::RebarGroupTrimByFace);
    if (!trimByLine.has_value() || !trimByFace.has_value()) {
        return fail("trim command contracts are missing");
    }
    if (trimByLine->legacyContextCommandId != QStringLiteral("0x8939") ||
        trimByLine->commandKey != QStringLiteral("RebarGroup.TrimByLine") ||
        !trimByLine->inputSelectionTypes.contains(QStringLiteral("rebar-group+line"))) {
        return fail("RebarGroup.TrimByLine contract metadata mismatch");
    }
    if (trimByLine->uiSurface != tsrebar::LegacyCommandUiSurface::ContextMenu ||
        !trimByLine->tabObjectName.isEmpty() || !trimByLine->groupObjectName.isEmpty() ||
        !trimByLine->evidenceIds.contains(QStringLiteral("E-CTX-8939"))) {
        return fail("RebarGroup.TrimByLine placement or evidence contract mismatch");
    }
    if (trimByFace->legacyContextCommandId != QStringLiteral("0x8CE5") ||
        trimByFace->commandKey != QStringLiteral("RebarGroup.TrimByFace") ||
        !trimByFace->inputSelectionTypes.contains(QStringLiteral("rebar-group+face"))) {
        return fail("RebarGroup.TrimByFace contract metadata mismatch");
    }
    if (trimByFace->uiSurface != tsrebar::LegacyCommandUiSurface::ContextMenu ||
        !trimByFace->tabObjectName.isEmpty() || !trimByFace->groupObjectName.isEmpty() ||
        !trimByFace->evidenceIds.contains(QStringLiteral("E-CTX-8CE5"))) {
        return fail("RebarGroup.TrimByFace placement or evidence contract mismatch");
    }

    return true;
}

bool testLegacyPlaceholdersRegisterStableNotImplementedHandlers()
{
    tsrebar::CommandRegistry registry;
    tsrebar::registerLegacyUiCommandPlaceholders(registry);

    if (!registry.hasHandler(tsrebar::CommandId::RebarLineCreate) ||
        !registry.hasHandler(tsrebar::CommandId::RebarArcGroupCreate) ||
        !registry.hasHandler(tsrebar::CommandId::RebarGroupTrimByLine) ||
        !registry.hasHandler(tsrebar::CommandId::RebarGroupTrimByFace)) {
        return fail("legacy rebar command placeholders must register handlers");
    }

    const tsrebar::CommandResult result =
        registry.execute(tsrebar::CommandId::RebarGroupTrimByFace);
    if (result.status != tsrebar::CommandStatus::NotImplemented) {
        return fail("legacy command placeholder must return NotImplemented");
    }
    if (!result.message.contains(QStringLiteral("RebarGroup.TrimByFace")) ||
        !result.message.contains(QStringLiteral("旧图石行为待确认"))) {
        return fail("legacy command placeholder message is not stable or traceable");
    }

    return true;
}

bool testLegacyPlaceholdersDoNotOverrideRealHandlers()
{
    tsrebar::CommandRegistry registry;
    registry.registerHandler(tsrebar::CommandId::RebarLineCreate, []() {
        return tsrebar::CommandResult{tsrebar::CommandStatus::Completed,
                                      QStringLiteral("real handler")};
    });

    tsrebar::registerLegacyUiCommandPlaceholders(registry);

    const tsrebar::CommandResult result =
        registry.execute(tsrebar::CommandId::RebarLineCreate);
    if (result.status != tsrebar::CommandStatus::Completed ||
        result.message != QStringLiteral("real handler")) {
        return fail("legacy placeholder must not override an existing real handler");
    }

    return true;
}

bool testLegacyEvidenceIdsAreNormalized()
{
    const auto commands = tsrebar::legacyUiCommands();
    for (const auto& command : commands) {
        for (const QString& evidenceId : command.evidenceIds) {
            if (!evidenceId.startsWith(QStringLiteral("E-"))) {
                std::cerr << "non-normalized evidence id in "
                          << command.commandKey.toStdString() << ": "
                          << evidenceId.toStdString() << '\n';
                return false;
            }
        }
    }

    const auto openStep =
        tsrebar::legacyUiCommandDefinition(tsrebar::CommandId::ImportStep);
    if (!openStep.has_value() ||
        !openStep->sourceRefs.contains(QStringLiteral("36"))) {
        return fail("non-evidence source references must remain traceable");
    }

    return true;
}

bool testNotImplementedDefault()
{
    const tsrebar::CommandRegistry registry;
    const tsrebar::CommandResult result =
        registry.execute(tsrebar::CommandId::ExportRebarStep);

    if (result.status != tsrebar::CommandStatus::NotImplemented) {
        return fail("unimplemented command must return NotImplemented");
    }

    return true;
}

} // namespace

int main()
{
    if (!testMainTabs()) {
        return 1;
    }
    if (!testLegacyCommandMap()) {
        return 1;
    }
    if (!testLegacyRebarCommandContracts()) {
        return 1;
    }
    if (!testLegacyPlaceholdersRegisterStableNotImplementedHandlers()) {
        return 1;
    }
    if (!testLegacyPlaceholdersDoNotOverrideRealHandlers()) {
        return 1;
    }
    if (!testLegacyEvidenceIdsAreNormalized()) {
        return 1;
    }
    if (!testNotImplementedDefault()) {
        return 1;
    }
    return 0;
}
