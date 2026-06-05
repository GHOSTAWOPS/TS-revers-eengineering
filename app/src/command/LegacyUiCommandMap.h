#pragma once

#include "command/CommandId.h"

#include <QString>
#include <QVector>

namespace tsrebar {

struct LegacyUiCommandDefinition
{
    CommandId id;
    QString caption;
    QString objectName;
    QString tabObjectName;
    QString groupObjectName;
    QString evidence;
};

QVector<LegacyUiCommandDefinition> legacyUiCommands();
bool hasLegacyUiCommand(CommandId id);

} // namespace tsrebar

