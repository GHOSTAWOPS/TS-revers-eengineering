#pragma once

#include "command/CommandId.h"

#include <QString>

#include <functional>
#include <unordered_map>

namespace tsrebar {

enum class CommandStatus {
    Completed,
    NotImplemented,
    Failed
};

struct CommandDirtyFlags
{
    bool projectDirty = false;
    bool geometryDirty = false;
    bool rebarDirty = false;
    bool drawingDirty = false;
    bool selectionDirty = false;
    bool viewDirty = false;
};

struct CommandTransactionState
{
    bool committed = false;
    QString commandKey;
    QString legacyDirtyEvidenceId;
    QString unresolvedDirtyParityGap;
};

struct CommandResult
{
    CommandStatus status = CommandStatus::NotImplemented;
    QString message;
    CommandDirtyFlags dirtyFlags;
    CommandTransactionState transaction;
};

class CommandRegistry
{
public:
    using Handler = std::function<CommandResult()>;

    void registerHandler(CommandId id, Handler handler);
    [[nodiscard]] bool hasHandler(CommandId id) const;
    [[nodiscard]] CommandResult execute(CommandId id) const;

private:
    struct CommandIdHash
    {
        std::size_t operator()(CommandId id) const noexcept
        {
            return static_cast<std::size_t>(id);
        }
    };

    std::unordered_map<CommandId, Handler, CommandIdHash> m_handlers;
};

} // namespace tsrebar
