#pragma once

#include "import/StepImportResult.h"

#include <QString>
#include <QVector>

#include <TopoDS_Shape.hxx>

namespace tsrebar {

struct OcctImportedPart
{
    int id = -1;
    QString name;
    QString stableEntry;
    TopoDS_Shape shape;
};

struct OcctImportedDocument
{
    StepImportResult summary;
    QVector<OcctImportedPart> parts;

    [[nodiscard]] bool isSuccess() const
    {
        return summary.ok && !parts.isEmpty();
    }
};

} // namespace tsrebar

