#include "geometry/occ/selection/OccSelectionIndex.h"

#include <TopAbs_ShapeEnum.hxx>
#include <TopExp.hxx>

namespace {

TopAbs_ShapeEnum toTopAbs(tsrebar::LegacyShapeKind kind)
{
    switch (kind) {
    case tsrebar::LegacyShapeKind::Solid:
        return TopAbs_SOLID;
    case tsrebar::LegacyShapeKind::Face:
        return TopAbs_FACE;
    case tsrebar::LegacyShapeKind::Edge:
        return TopAbs_EDGE;
    case tsrebar::LegacyShapeKind::Vertex:
        return TopAbs_VERTEX;
    case tsrebar::LegacyShapeKind::Whole:
        return TopAbs_SHAPE;
    }
    return TopAbs_SHAPE;
}

void mapSubShapes(const TopoDS_Shape& shape,
                  tsrebar::LegacyShapeKind kind,
                  TopTools_IndexedMapOfShape* output)
{
    if (shape.IsNull() || output == nullptr) {
        return;
    }
    TopExp::MapShapes(shape, toTopAbs(kind), *output);
}

QString partKey(const tsrebar::LegacySelectionRef& ref)
{
    return QStringLiteral("entry=%1;synthetic=%2;syntheticOrdinal=%3")
        .arg(QString::fromStdString(ref.partEntry))
        .arg(ref.syntheticPart ? 1 : 0)
        .arg(ref.syntheticOrdinal);
}

} // namespace

namespace tsrebar {

OccSelectionIndex OccSelectionIndex::fromDocument(const OcctImportedDocument& document)
{
    OccSelectionIndex index;
    index.m_parts.reserve(document.parts.size());

    for (const OcctImportedPart& importedPart : document.parts) {
        if (importedPart.shape.IsNull() || importedPart.stableEntry.isEmpty()) {
            continue;
        }

        PartRecord record;
        record.id = importedPart.id;
        record.name = importedPart.name;
        record.stableEntry = importedPart.stableEntry;
        record.shape = importedPart.shape;
        mapSubShapes(record.shape, LegacyShapeKind::Solid, &record.solids);
        mapSubShapes(record.shape, LegacyShapeKind::Face, &record.faces);
        mapSubShapes(record.shape, LegacyShapeKind::Edge, &record.edges);
        mapSubShapes(record.shape, LegacyShapeKind::Vertex, &record.vertices);
        index.m_parts.push_back(record);
    }

    return index;
}

int OccSelectionIndex::partCount() const
{
    return m_parts.size();
}

int OccSelectionIndex::count(LegacyShapeKind kind) const
{
    int total = 0;
    for (const PartRecord& part : m_parts) {
        const TopTools_IndexedMapOfShape* shapeMap = mapForKind(part, kind);
        if (shapeMap != nullptr) {
            total += shapeMap->Extent();
        }
    }
    return total;
}

QVector<LegacySelectionRef> OccSelectionIndex::refs(LegacyShapeKind kind) const
{
    QVector<LegacySelectionRef> result;
    if (!isSelectableShapeKind(kind)) {
        return result;
    }

    result.reserve(count(kind));
    for (const PartRecord& part : m_parts) {
        const TopTools_IndexedMapOfShape* shapeMap = mapForKind(part, kind);
        if (shapeMap == nullptr) {
            continue;
        }

        for (int index = 1; index <= shapeMap->Extent(); ++index) {
            result.push_back(makeLegacySelectionRef(part.stableEntry,
                                                    false,
                                                    0,
                                                    kind,
                                                    index));
        }
    }
    return result;
}

std::optional<LegacySelectionRef> OccSelectionIndex::refForShape(
    const TopoDS_Shape& selectedShape) const
{
    if (selectedShape.IsNull()) {
        return std::nullopt;
    }

    const LegacyShapeKind kind = [&selectedShape]() {
        switch (selectedShape.ShapeType()) {
        case TopAbs_SOLID:
            return LegacyShapeKind::Solid;
        case TopAbs_FACE:
            return LegacyShapeKind::Face;
        case TopAbs_EDGE:
            return LegacyShapeKind::Edge;
        case TopAbs_VERTEX:
            return LegacyShapeKind::Vertex;
        default:
            return LegacyShapeKind::Whole;
        }
    }();

    if (!isSelectableShapeKind(kind)) {
        return std::nullopt;
    }

    for (const PartRecord& part : m_parts) {
        const TopTools_IndexedMapOfShape* shapeMap = mapForKind(part, kind);
        if (shapeMap == nullptr) {
            continue;
        }
        const int index = shapeMap->FindIndex(selectedShape);
        if (index > 0) {
            return makeLegacySelectionRef(part.stableEntry, false, 0, kind, index);
        }
    }
    return std::nullopt;
}

OccSelectionResolveResult OccSelectionIndex::resolve(const LegacySelectionRef& ref) const
{
    OccSelectionResolveResult result;
    result.ref = ref;

    const PartRecord* part = findPart(ref);
    if (part == nullptr) {
        result.diagnostic = QStringLiteral("selection ref part is not in current document: %1")
                                .arg(partKey(ref));
        return result;
    }

    const TopTools_IndexedMapOfShape* shapeMap = mapForKind(*part, ref.shapeKind);
    if (shapeMap == nullptr || ref.subShapeIndex <= 0 ||
        ref.subShapeIndex > shapeMap->Extent()) {
        result.diagnostic = QStringLiteral("selection ref sub-shape index is out of range");
        return result;
    }

    result.shape = shapeMap->FindKey(ref.subShapeIndex);
    result.found = !result.shape.IsNull();
    if (!result.found) {
        result.diagnostic = QStringLiteral("selection ref resolved to null shape");
    }
    return result;
}

OccSelectionResolveResult OccSelectionIndex::resolveStableId(const QString& stableId) const
{
    QString diagnostic;
    const std::optional<LegacySelectionRef> ref =
        parseStableSelectionString(stableId, &diagnostic);
    if (!ref.has_value()) {
        OccSelectionResolveResult result;
        result.diagnostic = diagnostic;
        return result;
    }
    return resolve(*ref);
}

const TopTools_IndexedMapOfShape* OccSelectionIndex::mapForKind(const PartRecord& part,
                                                               LegacyShapeKind kind)
{
    switch (kind) {
    case LegacyShapeKind::Solid:
        return &part.solids;
    case LegacyShapeKind::Face:
        return &part.faces;
    case LegacyShapeKind::Edge:
        return &part.edges;
    case LegacyShapeKind::Vertex:
        return &part.vertices;
    case LegacyShapeKind::Whole:
        return nullptr;
    }
    return nullptr;
}

const OccSelectionIndex::PartRecord* OccSelectionIndex::findPart(
    const LegacySelectionRef& ref) const
{
    for (const PartRecord& part : m_parts) {
        if (part.stableEntry.toStdString() == ref.partEntry &&
            !ref.syntheticPart &&
            ref.syntheticOrdinal == 0) {
            return &part;
        }
    }
    return nullptr;
}

} // namespace tsrebar
