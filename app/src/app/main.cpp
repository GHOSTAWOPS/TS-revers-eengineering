#include "app/MainWindow.h"

#include "geometry/occ/import/OcctStepImportService.h"
#include "presentation/occ/OccViewerWidget.h"

#include <QApplication>
#include <QStringList>

#include <iostream>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    const QStringList arguments = app.arguments();
    const int smokeDisplayIndex = arguments.indexOf(QStringLiteral("--smoke-display-step"));
    if (smokeDisplayIndex >= 0) {
        if (smokeDisplayIndex + 1 >= arguments.size()) {
            return 2;
        }

        const tsrebar::OcctImportedDocument document =
            tsrebar::OcctStepImportService().importDocument(arguments.at(smokeDisplayIndex + 1));
        if (!document.isSuccess()) {
            std::cerr << document.summary.error << '\n';
            return 1;
        }

        tsrebar::OccViewerWidget viewer;
        viewer.resize(800, 600);
        viewer.show();
        app.processEvents();

        QString error;
        if (!viewer.displayDocument(document, &error)) {
            std::cerr << error.toUtf8().constData() << '\n';
            return 1;
        }
        return 0;
    }

    MainWindow window;
    window.resize(1280, 820);
    if (arguments.contains(QStringLiteral("--smoke"))) {
        return 0;
    }

    window.show();

    return QApplication::exec();
}
