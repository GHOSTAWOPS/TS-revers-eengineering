#pragma once

#include <string>

namespace tsrebar {

struct StepImportResult
{
    bool ok = false;
    bool readOk = false;
    bool transferOk = false;
    std::string filePath;
    std::string lengthUnit;
    int roots = 0;
    int freeShapes = 0;
    int solids = 0;
    int faces = 0;
    int edges = 0;
    int vertices = 0;
    std::string error;
};

} // namespace tsrebar

