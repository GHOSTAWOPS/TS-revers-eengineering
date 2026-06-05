#pragma once

#include "geometry/legacy/LegacySelectionRef.h"
#include "geometry/occ/import/OcctImportedDocument.h"

#include <QVector>
#include <QString>

#include <TopTools_IndexedMapOfShape.hxx>
#include <TopoDS_Shape.hxx>

#include <optional>

namespace tsrebar {

struct OccSelectionResolveResult
{
    bool found = false;
    LegacySelectionRef ref;
    TopoDS_Shape shape;
    QString diagnostic;
};

class OccSelectionIndex
{
public:
    [[nodiscard]] static OccSelectionIndex fromDocument(const OcctImportedDocument& document);

    [[nodiscard]] int partCount() const;
    [[nodiscard]] int count(LegacyShapeKind kind) const;
    [[nodiscard]] QVector<LegacySelectionRef> refs(LegacyShapeKind kind) const;
    [[nodiscard]] std::optional<LegacySelectionRef> refForShape(
        const TopoDS_Shape& selectedShape) const;
    [[nodiscard]] OccSelectionResolveResult resolve(const LegacySelectionRef& ref) const;
    [[nodiscard]] OccSelectionResolveResult resolveStableId(const QString& stableId) const;

private:
    struct PartRecord
    {
        int id = -1;
        QString name;
        QString stableEntry;
        TopoDS_Shape shape;
        TopTools_IndexedMapOfShape solids;
        TopTools_IndexedMapOfShape faces;
        TopTools_IndexedMapOfShape edges;
        TopTools_IndexedMapOfShape vertices;
    };

    [[nodiscard]] static const TopTools_IndexedMapOfShape* mapForKind(
        const PartRecord& part,
        LegacyShapeKind kind);
    [[nodiscard]] const PartRecord* findPart(const LegacySelectionRef& ref) const;

    QVector<PartRecord> m_parts;
};

} // namespace tsrebar
