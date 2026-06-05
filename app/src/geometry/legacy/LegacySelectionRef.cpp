#include "geometry/legacy/LegacySelectionRef.h"

#include <QMap>
#include <QStringList>

namespace {

QMap<QString, QString> parseFields(const QString& value)
{
    QMap<QString, QString> fields;
    const QStringList tokens = value.split(QStringLiteral(";"), Qt::SkipEmptyParts);
    for (const QString& token : tokens) {
        const int separator = token.indexOf(QLatin1Char('='));
        if (separator <= 0) {
            continue;
        }
        fields.insert(token.left(separator), token.mid(separator + 1));
    }
    return fields;
}

void setDiagnostic(QString* diagnostic, const QString& value)
{
    if (diagnostic != nullptr) {
        *diagnostic = value;
    }
}

bool parseIntegerField(const QMap<QString, QString>& fields,
                       const QString& key,
                       int* output)
{
    bool ok = false;
    const int value = fields.value(key).toInt(&ok);
    if (!ok) {
        return false;
    }
    *output = value;
    return true;
}

bool validSyntheticIdentity(bool syntheticPart, int syntheticOrdinal)
{
    return syntheticPart ? syntheticOrdinal > 0 : syntheticOrdinal == 0;
}

} // namespace

namespace tsrebar {

int stableShapeType(LegacyShapeKind kind)
{
    switch (kind) {
    case LegacyShapeKind::Solid:
        return 2;
    case LegacyShapeKind::Face:
        return 4;
    case LegacyShapeKind::Edge:
        return 6;
    case LegacyShapeKind::Vertex:
        return 7;
    case LegacyShapeKind::Whole:
        return 8;
    }
    return -1;
}

std::optional<LegacyShapeKind> legacyShapeKindFromStableType(int stableType)
{
    switch (stableType) {
    case 2:
        return LegacyShapeKind::Solid;
    case 4:
        return LegacyShapeKind::Face;
    case 6:
        return LegacyShapeKind::Edge;
    case 7:
        return LegacyShapeKind::Vertex;
    case 8:
        return LegacyShapeKind::Whole;
    default:
        return std::nullopt;
    }
}

bool isSelectableShapeKind(LegacyShapeKind kind)
{
    return kind == LegacyShapeKind::Solid
        || kind == LegacyShapeKind::Face
        || kind == LegacyShapeKind::Edge
        || kind == LegacyShapeKind::Vertex;
}

LegacySelectionRef makeLegacySelectionRef(const QString& partEntry,
                                          bool syntheticPart,
                                          int syntheticOrdinal,
                                          LegacyShapeKind shapeKind,
                                          int subShapeIndex)
{
    LegacySelectionRef ref;
    ref.partEntry = partEntry.toStdString();
    ref.syntheticPart = syntheticPart;
    ref.syntheticOrdinal = syntheticOrdinal;
    ref.shapeKind = shapeKind;
    ref.subShapeIndex = subShapeIndex;
    ref.stableId = toStableSelectionString(ref).toStdString();
    return ref;
}

QString toStableSelectionString(const LegacySelectionRef& ref)
{
    if (ref.partEntry.empty() ||
        !validSyntheticIdentity(ref.syntheticPart, ref.syntheticOrdinal) ||
        !isSelectableShapeKind(ref.shapeKind) ||
        ref.subShapeIndex <= 0) {
        return {};
    }

    return QStringLiteral("ref=selection-v1;entry=%1;synthetic=%2;syntheticOrdinal=%3;"
                          "type=%4;index=%5")
        .arg(QString::fromStdString(ref.partEntry))
        .arg(ref.syntheticPart ? 1 : 0)
        .arg(ref.syntheticOrdinal)
        .arg(stableShapeType(ref.shapeKind))
        .arg(ref.subShapeIndex);
}

std::optional<LegacySelectionRef> parseStableSelectionString(const QString& stableId,
                                                             QString* diagnostic)
{
    const QMap<QString, QString> fields = parseFields(stableId);
    if (fields.value(QStringLiteral("ref")) != QStringLiteral("selection-v1")) {
        setDiagnostic(diagnostic, QStringLiteral("selection ref is missing selection-v1 version"));
        return std::nullopt;
    }
    if (fields.contains(QStringLiteral("part"))) {
        setDiagnostic(diagnostic, QStringLiteral("selection ref must not contain transient part id"));
        return std::nullopt;
    }

    const QString entry = fields.value(QStringLiteral("entry"));
    if (entry.isEmpty()) {
        setDiagnostic(diagnostic, QStringLiteral("selection ref is missing entry"));
        return std::nullopt;
    }

    int syntheticValue = 0;
    if (!parseIntegerField(fields, QStringLiteral("synthetic"), &syntheticValue) ||
        (syntheticValue != 0 && syntheticValue != 1)) {
        setDiagnostic(diagnostic, QStringLiteral("selection ref has invalid synthetic flag"));
        return std::nullopt;
    }

    int syntheticOrdinal = 0;
    if (!parseIntegerField(fields, QStringLiteral("syntheticOrdinal"), &syntheticOrdinal) ||
        syntheticOrdinal < 0) {
        setDiagnostic(diagnostic, QStringLiteral("selection ref has invalid synthetic ordinal"));
        return std::nullopt;
    }

    const bool syntheticPart = syntheticValue == 1;
    if (!validSyntheticIdentity(syntheticPart, syntheticOrdinal)) {
        setDiagnostic(diagnostic,
                      QStringLiteral("synthetic ordinal does not match part kind"));
        return std::nullopt;
    }

    int stableType = -1;
    int index = 0;
    if (!parseIntegerField(fields, QStringLiteral("type"), &stableType) ||
        !parseIntegerField(fields, QStringLiteral("index"), &index)) {
        setDiagnostic(diagnostic, QStringLiteral("selection ref is missing type or index"));
        return std::nullopt;
    }

    const std::optional<LegacyShapeKind> shapeKind =
        legacyShapeKindFromStableType(stableType);
    if (!shapeKind.has_value() || !isSelectableShapeKind(*shapeKind) || index <= 0) {
        setDiagnostic(diagnostic, QStringLiteral("selection ref has invalid sub-shape"));
        return std::nullopt;
    }

    LegacySelectionRef ref = makeLegacySelectionRef(entry,
                                                    syntheticPart,
                                                    syntheticOrdinal,
                                                    *shapeKind,
                                                    index);
    setDiagnostic(diagnostic, {});
    return ref;
}

} // namespace tsrebar
