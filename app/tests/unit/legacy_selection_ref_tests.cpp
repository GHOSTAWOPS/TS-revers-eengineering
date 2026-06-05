#include "geometry/legacy/LegacySelectionRef.h"

#include <iostream>

namespace {

void expect(bool condition, const char* message)
{
    if (!condition) {
        std::cerr << message << '\n';
        std::exit(1);
    }
}

} // namespace

int main()
{
    const tsrebar::LegacySelectionRef faceRef =
        tsrebar::makeLegacySelectionRef(QStringLiteral("0:1:1:1"),
                                        false,
                                        0,
                                        tsrebar::LegacyShapeKind::Face,
                                        12);

    const QString stableId = tsrebar::toStableSelectionString(faceRef);
    expect(stableId ==
               QStringLiteral("ref=selection-v1;entry=0:1:1:1;synthetic=0;"
                              "syntheticOrdinal=0;type=4;index=12"),
           "face stable selection string must match selection-v1 contract");
    expect(faceRef.stableId == stableId.toStdString(),
           "makeLegacySelectionRef must store the encoded stable id");

    QString diagnostic;
    const auto parsed = tsrebar::parseStableSelectionString(stableId, &diagnostic);
    expect(parsed.has_value(), diagnostic.toUtf8().constData());
    expect(parsed->partEntry == "0:1:1:1", "parsed entry mismatch");
    expect(!parsed->syntheticPart, "parsed synthetic flag mismatch");
    expect(parsed->syntheticOrdinal == 0, "parsed synthetic ordinal mismatch");
    expect(parsed->shapeKind == tsrebar::LegacyShapeKind::Face, "parsed shape kind mismatch");
    expect(parsed->subShapeIndex == 12, "parsed index mismatch");

    const auto transientPart =
        tsrebar::parseStableSelectionString(
            QStringLiteral("ref=selection-v1;part=7;entry=0:1;synthetic=0;"
                           "syntheticOrdinal=0;type=4;index=1"),
            &diagnostic);
    expect(!transientPart.has_value(), "stable selection ref must reject transient part id");

    const auto invalidSynthetic =
        tsrebar::parseStableSelectionString(
            QStringLiteral("ref=selection-v1;entry=0:1;synthetic=0;"
                           "syntheticOrdinal=1;type=4;index=1"),
            &diagnostic);
    expect(!invalidSynthetic.has_value(),
           "non-synthetic part must reject non-zero syntheticOrdinal");

    return 0;
}
