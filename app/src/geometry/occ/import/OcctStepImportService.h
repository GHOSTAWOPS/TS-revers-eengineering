#pragma once

#include "geometry/occ/import/OcctImportedDocument.h"
#include "import/StepImportResult.h"

#include <QString>

namespace tsrebar {

class OcctStepImportService
{
public:
    [[nodiscard]] StepImportResult importStepFile(const QString& stepPath) const;
    [[nodiscard]] OcctImportedDocument importDocument(const QString& stepPath) const;
};

} // namespace tsrebar
