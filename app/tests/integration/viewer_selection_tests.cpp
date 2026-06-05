#include "geometry/occ/import/OcctStepImportService.h"
#include "geometry/occ/selection/OccSelectionIndex.h"
#include "presentation/occ/OccViewerWidget.h"

#include <QApplication>

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

} // namespace

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    if (argc != 2) {
        std::cerr << "usage: viewer_selection_tests <123.stp>\n";
        return 2;
    }

    const QString stepPath = QString::fromLocal8Bit(argv[1]);
    const tsrebar::OcctImportedDocument document =
        tsrebar::OcctStepImportService().importDocument(stepPath);
    expect(document.isSuccess(), document.summary.error.c_str());

    tsrebar::OccViewerWidget viewer;
    viewer.resize(800, 600);
    viewer.show();
    app.processEvents();

    QString error;
    expect(viewer.displayDocument(document, &error), error.toUtf8().constData());

    const tsrebar::OccSelectionIndex index =
        tsrebar::OccSelectionIndex::fromDocument(document);
    const auto faceRefs = index.refs(tsrebar::LegacyShapeKind::Face);
    expect(!faceRefs.isEmpty(), "test STEP must have face refs");

    const QString stableId = QString::fromStdString(faceRefs.front().stableId);
    expect(viewer.selectByStableId(stableId, &error), error.toUtf8().constData());
    expect(viewer.currentSelectionStableId() == stableId.toStdString(),
           "viewer must retain selected stable id");
    expect(viewer.currentSelectionSummary().contains(QStringLiteral("Face")),
           "viewer summary must describe selected face");
    expect(viewer.currentSelectionDetails().contains(stableId),
           "viewer details must include stable id");

    return 0;
}
