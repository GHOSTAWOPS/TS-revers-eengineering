#pragma once

#include "command/CommandId.h"

#include <optional>
#include <QString>
#include <QVector>

namespace tsrebar {

class CommandRegistry;

enum class LegacyCommandUiSurface {
    Ribbon,
    ContextMenu
};

enum class LegacyCommandImplementationState {
    Implemented,
    NotImplemented
};

enum class LegacyCommandInteractionState {
    Idle,
    Armed,
    Picking,
    Previewing,
    Completed,
    Cancelled,
    Failed
};

struct LegacyUiCommandDefinition
{
    CommandId id;
    LegacyCommandUiSurface uiSurface = LegacyCommandUiSurface::Ribbon;
    QString commandKey;
    QString caption;
    QString objectName;
    QString tabObjectName;
    QString groupObjectName;
    QString legacyUiPath;
    QString legacyCommand;
    QString legacyContextCommandId;
    QVector<QString> evidenceIds;
    QVector<QString> sourceRefs;
    QVector<QString> gapIds;
    QVector<QString> inputSelectionTypes;
    LegacyCommandImplementationState implementationState =
        LegacyCommandImplementationState::NotImplemented;
    QVector<LegacyCommandInteractionState> interactionStates;
};

QVector<LegacyUiCommandDefinition> legacyUiCommands();
bool hasLegacyUiCommand(CommandId id);
std::optional<LegacyUiCommandDefinition> legacyUiCommandDefinition(CommandId id);
void registerLegacyUiCommandPlaceholders(CommandRegistry& registry);

} // namespace tsrebar
