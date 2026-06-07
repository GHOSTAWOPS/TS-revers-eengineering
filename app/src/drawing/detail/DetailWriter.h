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

struct DetailDrawingViewOptions
{
    QString viewId;
    QString drawingName;
    QString modelFileName;
    QString drawingUnit;
    QString drawingScale;
    QString generalScale;
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
