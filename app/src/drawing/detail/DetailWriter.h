#pragma once

#include "domain/rebar/SteelData.h"

#include <QString>
#include <QStringList>
#include <QVector>

namespace tsrebar {

struct DetailDiagnostic
{
    QString code;
    QString file;
    QString message;
};

struct DetailLegacyLineGeometry
{
    double startX = 0.0;
    double startY = 0.0;
    double endX = 0.0;
    double endY = 0.0;
    QString zValue;
};

using DetailSectionLineGeometry = DetailLegacyLineGeometry;

struct DetailSectionArcGeometry
{
    double centerX = 0.0;
    double centerY = 0.0;
    double centerZ = 0.0;
    double radius = 0.0;
    double startAngle = 0.0;
    double endAngle = 0.0;
    QString zValue;
};

struct DetailDrawingViewOptions
{
    QString viewId;
    QString drawingName;
    QString modelFileName;
    QString drawingUnit;
    QString drawingScale;
    QString generalScale;
    QVector<DetailLegacyLineGeometry> continueLines;
    QVector<DetailLegacyLineGeometry> hiddenLines;
    QVector<DetailLegacyLineGeometry> centralLines;
    QVector<DetailSectionLineGeometry> sectionLines;
    QVector<DetailSectionArcGeometry> sectionArcs;
    QVector<DetailLegacyLineGeometry> hatchLines;
};

struct DetailWriteOptions
{
    QString runId = QStringLiteral("DW-RUN-001");
    QString drawingName = QStringLiteral("detail");
    QString modelFileName;
    QString drawingUnit = QStringLiteral("m");
    QString drawingScale = QStringLiteral("1");
    QVector<DetailDrawingViewOptions> views;
    bool testInjectInstallFailureAfterFirstCopy = false;
};

struct DetailWriteResult
{
    bool ok = false;
    QString decision = QStringLiteral("fail");
    QString l0 = QStringLiteral("not_run");
    QString l1 = QStringLiteral("not_run");
    QString l2 = QStringLiteral("not_run");
    QStringList files;
    QStringList warnings;
    QStringList errorCodes;
    QVector<DetailDiagnostic> diagnostics;
    bool dirtyAfter = true;
    bool oldPackagePreserved = false;
    QString candidatePackagePath;
};

class DetailWriter
{
public:
    [[nodiscard]] DetailWriteResult writePackage(
        const QString& outputDir,
        const SteelData& steelData,
        const DetailWriteOptions& options) const;
};

} // namespace tsrebar
