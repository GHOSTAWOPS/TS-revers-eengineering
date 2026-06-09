#include "app/MainWindow.h"

#include "command/LegacyUiCommandMap.h"
#include "geometry/occ/import/OcctStepImportService.h"
#include "presentation/occ/OccViewerWidget.h"

#include <QAction>
#include <QApplication>

#include <cstdlib>
#include <iostream>

namespace {

void expect(bool condition, const char* message)
{
    if (!condition) {
        std::cerr << message << '\n';
        std::cerr.flush();
        std::exit(1);
    }
}

QString lineGroupActionObjectName()
{
    const auto commands = tsrebar::legacyUiCommands();
    for (const auto& command : commands) {
        if (command.id == tsrebar::CommandId::RebarLineCreate) {
            return command.objectName;
        }
    }
    return {};
}

} // namespace

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    if (argc != 2) {
        std::cerr << "usage: line_group_display_smoke_tests <123.stp>\n";
        return 2;
    }

    MainWindow window;
    window.setAttribute(Qt::WA_DontShowOnScreen, true);
    window.resize(1280, 820);
    window.show();
    app.processEvents();

    auto* viewer = window.findChild<tsrebar::OccViewerWidget*>();
    expect(viewer != nullptr, "MainWindow must expose one OccViewerWidget child");

    const tsrebar::OcctImportedDocument document =
        tsrebar::OcctStepImportService().importDocument(QString::fromLocal8Bit(argv[1]));
    expect(document.isSuccess(), document.summary.error.c_str());

    QString error;
    expect(viewer->displayDocument(document, &error), error.toUtf8().constData());

    const QString actionObjectName = lineGroupActionObjectName();
    expect(!actionObjectName.isEmpty(), "RebarLineCreate action metadata must exist");
    QAction* action = window.findChild<QAction*>(actionObjectName);
    expect(action != nullptr, "MainWindow must render RebarLineCreate action");

    const int beforeFailure = viewer->displayedRebarShapeCount();
    action->trigger();
    app.processEvents();
    expect(viewer->displayedRebarShapeCount() == beforeFailure,
           "failed line group command must not refresh rebar AIS display");

    const auto edgeRefs = viewer->selectionIndex().refs(tsrebar::LegacyShapeKind::Edge);
    expect(!edgeRefs.isEmpty(), "test STEP must have edge selection refs");
    expect(viewer->selectByStableId(QString::fromStdString(edgeRefs.front().stableId), &error),
           error.toUtf8().constData());

    const int beforeSuccess = viewer->displayedRebarShapeCount();
    action->trigger();
    app.processEvents();

    expect(viewer->displayedRebarShapeCount() > beforeSuccess,
           "successful line group command must display created SteelBarGroup in AIS");
    const QString firstDisplayedGroup = viewer->lastDisplayedRebarGroupId();
    expect(!firstDisplayedGroup.isEmpty(),
           "successful line group command must record displayed group id");

    const int beforeSecondSuccess = viewer->displayedRebarShapeCount();
    action->trigger();
    app.processEvents();

    expect(viewer->displayedRebarShapeCount() > beforeSecondSuccess,
           "second successful line group command must add AIS display items");
    expect(viewer->lastDisplayedRebarGroupId() != firstDisplayedGroup,
           "second successful line group command must display the newly created group");
    return 0;
}
