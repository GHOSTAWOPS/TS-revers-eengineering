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
    if (!testNotImplementedDefault()) {
        return 1;
    }
    return 0;
}
