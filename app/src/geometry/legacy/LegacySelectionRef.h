#pragma once

#include <optional>
#include <string>

#include <QString>

namespace tsrebar {

enum class LegacyShapeKind {
    Whole,
    Solid,
    Face,
    Edge,
    Vertex
};

struct LegacySelectionRef
{
    std::string stableId;
    std::string partEntry;
    bool syntheticPart = false;
    int syntheticOrdinal = 0;
    LegacyShapeKind shapeKind = LegacyShapeKind::Whole;
    int subShapeIndex = 0;
};

struct LegacyFaceRef
{
    LegacySelectionRef base;
};

struct LegacyEdgeRef
{
    LegacySelectionRef base;
};

[[nodiscard]] int stableShapeType(LegacyShapeKind kind);
[[nodiscard]] std::optional<LegacyShapeKind> legacyShapeKindFromStableType(int stableType);
[[nodiscard]] bool isSelectableShapeKind(LegacyShapeKind kind);

[[nodiscard]] LegacySelectionRef makeLegacySelectionRef(const QString& partEntry,
                                                        bool syntheticPart,
                                                        int syntheticOrdinal,
                                                        LegacyShapeKind shapeKind,
                                                        int subShapeIndex);

[[nodiscard]] QString toStableSelectionString(const LegacySelectionRef& ref);
[[nodiscard]] std::optional<LegacySelectionRef> parseStableSelectionString(
    const QString& stableId,
    QString* diagnostic = nullptr);

} // namespace tsrebar
