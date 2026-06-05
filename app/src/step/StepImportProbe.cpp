#include "geometry/occ/import/OcctStepImportService.h"

#include <QCoreApplication>
#include <QString>
#include <QTextStream>

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    QTextStream out(stdout);
    QTextStream err(stderr);

    const QStringList args = app.arguments();
    if (args.size() != 2) {
        err << "usage: step_import_probe <file.stp|file.step>\n";
        return 2;
    }

    const tsrebar::StepImportResult result =
        tsrebar::OcctStepImportService().importStepFile(args.at(1));

    out << "ok=" << (result.ok ? "true" : "false") << "\n";
    out << "read_ok=" << (result.readOk ? "true" : "false") << "\n";
    out << "transfer_ok=" << (result.transferOk ? "true" : "false") << "\n";
    out << "roots=" << result.roots << "\n";
    out << "free_shapes=" << result.freeShapes << "\n";
    out << "solids=" << result.solids << "\n";
    out << "faces=" << result.faces << "\n";
    out << "edges=" << result.edges << "\n";
    out << "vertices=" << result.vertices << "\n";
    out << "length_unit=" << QString::fromStdString(result.lengthUnit) << "\n";
    if (!result.error.empty()) {
        out << "error=" << QString::fromStdString(result.error) << "\n";
    }

    return result.ok ? 0 : 1;
}

