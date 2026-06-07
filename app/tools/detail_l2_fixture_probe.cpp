#include "drawing/detail/DetailWriter.h"

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTextStream>
#include <QXmlStreamReader>

#include <cstdlib>
#include <string>

namespace {

tsrebar::RebarEvidenceRef evidenceRef(const std::string& id, const std::string& note)
{
    tsrebar::RebarEvidenceRef evidence;
    evidence.evidenceId = id;
    evidence.note = note;
    return evidence;
}

tsrebar::SteelData makeProbeSteelData()
{
    tsrebar::SteelBarSegment line;
    line.segmentId = "todo033-segment-line-001";
    line.barId = "todo033-bar-001";
    line.sequenceNo = 1;
    line.shapeType = tsrebar::SteelBarSegmentShape::Line;
    line.startPoint = {0.0, 0.0, 0.0};
    line.middlePoint = {5.0, 0.0, 0.0};
    line.endPoint = {10.0, 0.0, 0.0};
    line.length = 10.0;
    line.evidence.push_back(evidenceRef("E-IDA-018", "StbGeo write chain"));

    tsrebar::SteelBarSegment arc;
    arc.segmentId = "todo033-segment-arc-001";
    arc.barId = "todo033-bar-001";
    arc.sequenceNo = 2;
    arc.shapeType = tsrebar::SteelBarSegmentShape::Arc;
    arc.startPoint = {10.0, 0.0, 0.0};
    arc.middlePoint = {12.0, 2.0, 0.0};
    arc.endPoint = {10.0, 5.0, 0.0};
    arc.startRadius = 3.0;
    arc.endRadius = 3.0;
    arc.length = 5.0;
    arc.evidence.push_back(evidenceRef("E-IDA-018", "StbGeo write chain"));

    tsrebar::SteelBar bar;
    bar.barId = "todo033-bar-001";
    bar.groupId = "todo033-group-001";
    bar.sequenceNo = 1;
    bar.displayNumber = "12A";
    bar.diameter = 25.0;
    bar.steelLevel = "HRB400";
    bar.length = 15.0;
    bar.shapeType = "lineStb";
    bar.segmentIds = {line.segmentId, arc.segmentId};
    bar.evidence.push_back(evidenceRef("E-IDA-017", "steelbar evidence"));

    tsrebar::SteelBarGroup group;
    group.groupId = "todo033-group-001";
    group.rsdId = "Y12";
    group.displayNumber = "12A";
    group.actualNumber = "12";
    group.componentName = "todo033-pier";
    group.projectSteelName = "todo033-main-bar";
    group.createCommand = "Rebar.Create.LineGroup";
    group.legacyCommand = "sgroupbarline";
    group.steelDataId = "todo033-steel-data-001";
    group.diameter = 25.0;
    group.interval = 200.0;
    group.barCount = 3;
    group.segmentCount = 2;
    group.steelLevel = "HRB400";
    group.layer = "inside";
    group.profile = "default-profile";
    group.use = "main";
    group.rangeLess180 = true;
    group.steelWay = "LA";
    group.rebarType = "lineStb";
    group.barIds.push_back(bar.barId);
    group.evidence.push_back(evidenceRef("E-IDA-018", "StbGroup write chain"));
    group.evidence.push_back(evidenceRef("E-IDA-019", "StbTable write chain"));

    tsrebar::SteelData steelData;
    steelData.steelDataId = "todo033-steel-data-001";
    steelData.level = "HRB400";
    steelData.gradeName = "HRB400";
    steelData.diameterSet.push_back(25.0);
    steelData.evidence.push_back(evidenceRef("E-DETAIL-001", "Detail package evidence"));
    steelData.groups.push_back(group);
    steelData.bars.push_back(bar);
    steelData.segments.push_back(line);
    steelData.segments.push_back(arc);
    return steelData;
}

QString rootName(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QStringLiteral("open_failed");
    }

    QXmlStreamReader reader(&file);
    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.isStartElement()) {
            return reader.name().toString();
        }
    }
    return QStringLiteral("missing");
}

QJsonObject fileSummary(const QString& outputDir, const QString& fileName)
{
    const QString path = QDir(outputDir).filePath(fileName);
    QFile file(path);
    QJsonObject item;
    item.insert(QStringLiteral("name"), fileName);
    item.insert(QStringLiteral("path"), QFileInfo(path).absoluteFilePath());
    item.insert(QStringLiteral("exists"), file.exists());
    if (!file.open(QIODevice::ReadOnly)) {
        item.insert(QStringLiteral("sha256"), QString());
        item.insert(QStringLiteral("bytes"), 0);
        item.insert(QStringLiteral("root"), QStringLiteral("open_failed"));
        return item;
    }

    const QByteArray bytes = file.readAll();
    item.insert(QStringLiteral("sha256"),
                QString::fromLatin1(QCryptographicHash::hash(bytes, QCryptographicHash::Sha256).toHex()));
    item.insert(QStringLiteral("bytes"), bytes.size());
    item.insert(QStringLiteral("root"), rootName(path));
    return item;
}

QJsonArray diagnosticsToJson(const QVector<tsrebar::DetailDiagnostic>& diagnostics)
{
    QJsonArray result;
    for (const auto& diagnostic : diagnostics) {
        QJsonObject item;
        item.insert(QStringLiteral("code"), diagnostic.code);
        item.insert(QStringLiteral("file"), diagnostic.file);
        item.insert(QStringLiteral("message"), diagnostic.message);
        result.append(item);
    }
    return result;
}

QJsonArray stringListToJson(const QStringList& values)
{
    QJsonArray result;
    for (const auto& value : values) {
        result.append(value);
    }
    return result;
}

} // namespace

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("detail_l2_fixture_probe"));

    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Generate a reproducible Detail package for AutoCAD L2 import probing."));
    parser.addHelpOption();
    const QCommandLineOption outputOption(
        {QStringLiteral("o"), QStringLiteral("output")},
        QStringLiteral("Output directory for Detail.xml and DetailNN.stl."),
        QStringLiteral("dir"));
    const QCommandLineOption viewsOption(
        {QStringLiteral("v"), QStringLiteral("views")},
        QStringLiteral("Number of drawing views to generate."),
        QStringLiteral("count"),
        QStringLiteral("3"));
    parser.addOption(outputOption);
    parser.addOption(viewsOption);
    parser.addPositionalArgument(QStringLiteral("outputDir"), QStringLiteral("Output directory fallback."));
    parser.process(app);

    QString outputDir = parser.value(outputOption);
    if (outputDir.isEmpty() && !parser.positionalArguments().isEmpty()) {
        outputDir = parser.positionalArguments().front();
    }
    if (outputDir.isEmpty()) {
        QTextStream(stderr) << "missing output directory\n";
        return EXIT_FAILURE;
    }

    bool ok = false;
    int viewCount = parser.value(viewsOption).toInt(&ok);
    if (!ok || viewCount < 1 || viewCount > 100) {
        QTextStream(stderr) << "views must be an integer in [1, 100]\n";
        return EXIT_FAILURE;
    }

    tsrebar::DetailWriteOptions options;
    options.runId = QStringLiteral("DW-L2-TODO033-001");
    options.drawingName = QStringLiteral("todo033-autocad-l2");
    options.modelFileName = QStringLiteral("todo033-model.step");
    options.drawingUnit = QStringLiteral("m");
    options.drawingScale = QStringLiteral("1");
    for (int index = 1; index <= viewCount; ++index) {
        tsrebar::DetailDrawingViewOptions view;
        view.viewId = QStringLiteral("todo033-view-%1").arg(index, 3, 10, QLatin1Char('0'));
        view.drawingName = QStringLiteral("todo033-view-%1").arg(index);
        view.modelFileName = QStringLiteral("todo033-model-%1.step").arg(index);
        view.drawingUnit = QStringLiteral("mm");
        view.drawingScale = QStringLiteral("1:%1").arg(index);
        view.generalScale = QStringLiteral("todo033-general-%1").arg(index);
        options.views.push_back(view);
    }

    const tsrebar::DetailWriter writer;
    const tsrebar::DetailWriteResult result =
        writer.writePackage(outputDir, makeProbeSteelData(), options);

    QJsonObject root;
    root.insert(QStringLiteral("schemaVersion"), QStringLiteral("detail-l2-fixture-probe/v1"));
    root.insert(QStringLiteral("runId"), options.runId);
    root.insert(QStringLiteral("outputDir"), QFileInfo(outputDir).absoluteFilePath());
    root.insert(QStringLiteral("viewCount"), viewCount);
    root.insert(QStringLiteral("ok"), result.ok);
    root.insert(QStringLiteral("decision"), result.decision);
    root.insert(QStringLiteral("l0"), result.l0);
    root.insert(QStringLiteral("l1"), result.l1);
    root.insert(QStringLiteral("l2"), result.l2);
    root.insert(QStringLiteral("warnings"), stringListToJson(result.warnings));
    root.insert(QStringLiteral("errorCodes"), stringListToJson(result.errorCodes));
    root.insert(QStringLiteral("diagnostics"), diagnosticsToJson(result.diagnostics));
    root.insert(QStringLiteral("autocadL2"), QStringLiteral("not_run"));
    root.insert(QStringLiteral("evidence"),
                QJsonArray{QStringLiteral("E-DEV-054"), QStringLiteral("E-DEV-055")});
    root.insert(QStringLiteral("gaps"), QJsonArray{QStringLiteral("GAP-DRAW-001"), QStringLiteral("GAP-DRAW-002")});

    QJsonArray files;
    for (const auto& fileName : result.files) {
        files.append(fileSummary(outputDir, fileName));
    }
    root.insert(QStringLiteral("files"), files);

    QTextStream(stdout) << QString::fromUtf8(QJsonDocument(root).toJson(QJsonDocument::Indented));
    return result.ok ? EXIT_SUCCESS : 2;
}
