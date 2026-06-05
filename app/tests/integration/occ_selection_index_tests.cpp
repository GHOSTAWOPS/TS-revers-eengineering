#include "geometry/occ/import/OcctStepImportService.h"
#include "geometry/occ/selection/OccSelectionIndex.h"

#include <TopAbs_ShapeEnum.hxx>

#include <iostream>

namespace {

void expect(bool condition, const char* message)
{
    if (!condition) {
        std::cerr << message << '\n';
        std::exit(1);
    }
}

void expectRefRoundTrip(const tsrebar::OccSelectionIndex& index,
                        tsrebar::LegacyShapeKind kind,
                        int expectedTopAbsType)
{
    const auto refs = index.refs(kind);
    expect(!refs.isEmpty(), "selection index must produce refs for requested shape kind");

    const tsrebar::LegacySelectionRef& ref = refs.front();
    expect(ref.subShapeIndex == 1, "first generated ref must be one-based");
    expect(ref.stableId.find("ref=selection-v1;") == 0,
           "stable ref must use selection-v1 schema");
    expect(ref.stableId.find("part=") == std::string::npos,
           "stable ref must not contain transient part id");
    expect(ref.stableId.find("type=" + std::to_string(expectedTopAbsType)) != std::string::npos,
           "stable ref must encode old TopAbs-compatible type value");

    const tsrebar::OccSelectionResolveResult resolved = index.resolve(ref);
    expect(resolved.found, resolved.diagnostic.toUtf8().constData());
    expect(!resolved.shape.IsNull(), "resolved shape must not be null");
    expect(resolved.ref.stableId == ref.stableId, "resolve must preserve stable id");
}

} // namespace

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "usage: occ_selection_index_tests <123.stp>\n";
        return 2;
    }

    const tsrebar::OcctImportedDocument document =
        tsrebar::OcctStepImportService().importDocument(QString::fromLocal8Bit(argv[1]));
    expect(document.isSuccess(), document.summary.error.c_str());

    const tsrebar::OccSelectionIndex index =
        tsrebar::OccSelectionIndex::fromDocument(document);
    expect(index.partCount() == document.parts.size(), "selection index part count mismatch");
    expect(index.count(tsrebar::LegacyShapeKind::Face) == document.summary.faces,
           "face refs must match imported face count");
    expect(index.count(tsrebar::LegacyShapeKind::Edge) == 4524,
           "123.stp selectable unique edge count must stay stable");
    expect(index.count(tsrebar::LegacyShapeKind::Vertex) == 3016,
           "123.stp selectable unique vertex count must stay stable");

    expectRefRoundTrip(index, tsrebar::LegacyShapeKind::Face, static_cast<int>(TopAbs_FACE));
    expectRefRoundTrip(index, tsrebar::LegacyShapeKind::Edge, static_cast<int>(TopAbs_EDGE));
    expectRefRoundTrip(index, tsrebar::LegacyShapeKind::Vertex, static_cast<int>(TopAbs_VERTEX));

    return 0;
}
