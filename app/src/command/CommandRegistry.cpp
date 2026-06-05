#include "command/CommandRegistry.h"

namespace tsrebar {

void CommandRegistry::registerHandler(CommandId id, Handler handler)
{
    m_handlers[id] = std::move(handler);
}

bool CommandRegistry::hasHandler(CommandId id) const
{
    return m_handlers.find(id) != m_handlers.end();
}

CommandResult CommandRegistry::execute(CommandId id) const
{
    const auto found = m_handlers.find(id);
    if (found == m_handlers.end()) {
        return {CommandStatus::NotImplemented,
                QStringLiteral("CommandId %1").arg(static_cast<int>(id))};
    }

    return found->second();
}

} // namespace tsrebar

