#include "drawing/detail/DetailWriter.h"

#include <QDir>
#include <QFile>
#include <QTemporaryDir>
#include <QXmlStreamReader>

#include <cstdlib>
#include <iostream>

namespace {

void expect(bool condition, const char* message)
{
    if (!condition) {
        std::cerr << message << '\n';
        std::exit(1);
    }
}

QString readText(const QString& path)
{
    QFile file(path);
    expect(file.open(QIODevice::ReadOnly | QIODevice::Text), "file must open");
    return QString::fromUtf8(file.readAll());
}

void writeText(const QString& path, const QByteArray& text)
{
    QFile file(path);
    expect(file.open(QIODevice::WriteOnly | QIODevice::Text), "file must open for write");
    file.write(text);
}

QXmlStreamAttributes findElementAttrs(const QString& path, const QString& name)
{
    QFile file(path);
    expect(file.open(QIODevice::ReadOnly | QIODevice::Text), "xml file must open");
    QXmlStreamReader reader(&file);
    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.isStartElement() && reader.name() == name) {
            return reader.attributes();
        }
    }
    expect(false, qPrintable(QStringLiteral("missing xml element %1").arg(name)));
    return {};
}

QString rootName(const QString& path)
{
    QFile file(path);
    expect(file.open(QIODevice::ReadOnly | QIODevice::Text), "xml file must open");
    QXmlStreamReader reader(&file);
    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.isStartElement()) {
            return reader.name().toString();
        }
    }
    expect(false, "xml root missing");
    return {};
}

tsrebar::SteelData steelDataWithMixedGroup()
{
    tsrebar::SteelBarSegment line;
    line.segmentId = "segment-line-001";
    line.barId = "bar-001";
    line.sequenceNo = 1;
    line.shapeType = tsrebar::SteelBarSegmentShape::Line;
    line.startPoint = {0.0, 0.0, 0.0};
    line.endPoint = {10.0, 0.0, 0.0};
    line.middlePoint = {5.0, 0.0, 0.0};
    line.length = 10.0;
    line.evidence.push_back({"E-IDA-018", "StbGeo write chain"});

    tsrebar::SteelBarSegment arc;
    arc.segmentId = "segment-arc-001";
    arc.barId = "bar-001";
    arc.sequenceNo = 2;
    arc.shapeType = tsrebar::SteelBarSegmentShape::Arc;
    arc.startPoint = {10.0, 0.0, 0.0};
    arc.middlePoint = {12.0, 2.0, 0.0};
    arc.endPoint = {10.0, 5.0, 0.0};
    arc.startRadius = 3.0;
    arc.endRadius = 3.0;
    arc.length = 5.0;
    arc.evidence.push_back({"E-IDA-018", "StbGeo write chain"});

    tsrebar::SteelBar bar;
    bar.barId = "bar-001";
    bar.groupId = "group-001";
    bar.sequenceNo = 1;
    bar.displayNumber = "12A";
    bar.diameter = 25.0;
    bar.steelLevel = "HRB400";
    bar.length = 15.0;
    bar.shapeType = "lineStb";
    bar.segmentIds = {line.segmentId, arc.segmentId};
    bar.evidence.push_back({"E-IDA-017", "steelbar evidence"});

    tsrebar::SteelBarGroup group;
    group.groupId = "group-001";
    group.rsdId = "Y12";
    group.displayNumber = "12A";
    group.actualNumber = "12";
    group.componentName = "pier";
    group.projectSteelName = "main bar";
    group.createCommand = "Rebar.Create.LineGroup";
    group.legacyCommand = "sgroupbarline";
    group.steelDataId = "steel-data-001";
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
    group.evidence.push_back({"E-IDA-018", "StbGroup write chain"});
    group.evidence.push_back({"E-IDA-019", "StbTable write chain"});

    tsrebar::SteelData steelData;
    steelData.steelDataId = "steel-data-001";
    steelData.level = "HRB400";
    steelData.gradeName = "HRB400";
    steelData.diameterSet.push_back(25.0);
    steelData.evidence.push_back({"E-DETAIL-001", "Detail package evidence"});
    steelData.groups.push_back(group);
    steelData.bars.push_back(bar);
    steelData.segments.push_back(line);
    steelData.segments.push_back(arc);
    return steelData;
}

void testDetailWriterMapsDomainRebarToDetailPackage()
{
    QTemporaryDir temp;
    expect(temp.isValid(), "temporary dir must be valid");
    const QString outputDir = QDir(temp.path()).filePath("drawings");

    tsrebar::DetailWriteOptions options;
    options.runId = "DW-UNIT-001";
    options.drawingName = "unit-detail";
    options.modelFileName = "unit-model.step";

    const tsrebar::DetailWriter writer;
    const auto result = writer.writePackage(outputDir, steelDataWithMixedGroup(), options);

    if (!result.ok) {
        std::cerr << "detail writer failed: " << result.decision.toStdString() << '\n';
        for (const auto& diagnostic : result.diagnostics) {
            std::cerr << diagnostic.code.toStdString() << " "
                      << diagnostic.file.toStdString() << " "
                      << diagnostic.message.toStdString() << '\n';
        }
    }
    expect(result.ok, "Detail writer must succeed for valid domain rebar");
    expect(result.decision == "l0-l1-pass", "Detail writer decision mismatch");
    expect(result.l0 == "passed", "Detail writer L0 must pass");
    expect(result.l1 == "passed", "Detail writer L1 must pass");
    expect(result.l2 == "not_run", "Detail writer must not claim AutoCAD L2");
    expect(result.files.contains("Detail.xml"), "Detail.xml must be reported");
    expect(result.files.contains("Detail01.stl"), "Detail01.stl must be reported");
    expect(result.warnings.contains("DW-WARN-MATERIAL_MASS_FORMULA_DEFERRED"),
           "MaterialTable mass formula must be explicitly deferred");
    expect(!result.dirtyAfter, "successful Detail write can clear drawing dirty");

    const QString detailXml = QDir(outputDir).filePath("Detail.xml");
    const QString detailStl = QDir(outputDir).filePath("Detail01.stl");
    expect(rootName(detailXml) == "StyleRoot", "Detail.xml root must be StyleRoot");
    expect(rootName(detailStl) == "DrawingRoot", "Detail01.stl root must be DrawingRoot XML");

    const auto group = findElementAttrs(detailStl, "StbGroup1");
    expect(group.value("rsdID") == "Y12", "StbGroup.rsdID must map from group.rsdId");
    expect(group.value("groupID") == "group-001", "StbGroup.groupID must map from group.groupId");
    expect(group.value("stbNum") == "12A", "StbGroup.stbNum must map from displayNumber");
    expect(group.value("stbNumAct") == "12", "StbGroup.stbNumAct must map from actualNumber");
    expect(group.value("barcount") == "3", "StbGroup.barcount mismatch");
    expect(group.value("segcount") == "2", "StbGroup.segcount mismatch");
    expect(group.value("SteelWay") == "LA", "StbGroup.SteelWay mismatch");

    const auto std = findElementAttrs(detailStl, "Std1");
    expect(std.value("segCount") == "2", "Std.segCount must equal StbGeo count");

    const auto geo1 = findElementAttrs(detailStl, "StbGeo1");
    expect(geo1.value("segID") == "segment-line-001", "StbGeo.segID mismatch");
    expect(geo1.value("stbSeqNum") == "1", "StbGeo.stbSeqNum mismatch");
    expect(geo1.value("shapeType") == "L", "line segment must map to StbGeo shape L");
    expect(geo1.value("start_x") == "0", "StbGeo line start_x mismatch");
    expect(geo1.value("end_x") == "10", "StbGeo line end_x mismatch");

    const auto geo2 = findElementAttrs(detailStl, "StbGeo2");
    expect(geo2.value("segID") == "segment-arc-001", "arc StbGeo.segID mismatch");
    expect(geo2.value("shapeType") == "A", "arc segment must map to StbGeo shape A");
    expect(geo2.value("middle_x") == "12", "arc StbGeo middle_x mismatch");

    const auto row = findElementAttrs(detailStl, "StbRow1");
    expect(row.value("rsdID") == "Y12", "StbRow.rsdID must share StbGroup.rsdID");
    expect(row.value("diameter") == "25", "StbRow.diameter mismatch");
    expect(row.value("length") == "15", "StbRow.length must be one bar total length");
    expect(row.value("segNum") == "2", "StbRow.segNum mismatch");
    expect(row.value("stbNumSum") == "3", "StbRow.stbNumSum mismatch");
    expect(row.value("lenSum") == "45", "StbRow.lenSum mismatch");

    const auto mat = findElementAttrs(detailStl, "MaterialTable");
    expect(mat.value("rowCount") == "1", "MaterialTable.rowCount mismatch");
    expect(mat.value("Mass") == "0", "MaterialTable.Mass must be deferred zero");
    expect(mat.value("Volume722") == "0", "MaterialTable.Volume722 must be deferred zero");

    const auto matRow = findElementAttrs(detailStl, "MatRow1");
    expect(matRow.value("diameter") == "25", "MatRow.diameter mismatch");
    expect(matRow.value("lenSum") == "45", "MatRow.lenSum mismatch");
    expect(matRow.value("countSum") == "3", "MatRow.countSum mismatch");
}

void testDetailWriterFailurePreservesExistingPackage()
{
    QTemporaryDir temp;
    expect(temp.isValid(), "temporary dir must be valid");
    const QString outputDir = QDir(temp.path()).filePath("drawings");
    expect(QDir().mkpath(outputDir), "output dir must be created");
    const QString oldXml = QDir(outputDir).filePath("Detail.xml");
    const QString oldStl = QDir(outputDir).filePath("Detail01.stl");
    writeText(oldXml, "<StyleRoot><Old value=\"style\" /></StyleRoot>");
    writeText(oldStl, "<DrawingRoot><Old value=\"drawing\" /></DrawingRoot>");
    const QString oldXmlText = readText(oldXml);
    const QString oldStlText = readText(oldStl);

    tsrebar::SteelData broken = steelDataWithMixedGroup();
    broken.groups.front().rsdId.clear();

    const tsrebar::DetailWriter writer;
    const auto result = writer.writePackage(outputDir, broken, {});

    expect(!result.ok, "missing rsdID must fail");
    expect(result.decision == "fail", "failed Detail writer decision mismatch");
    expect(result.errorCodes.contains("DW004_REQUIRED_FIELD_MISSING"),
           "missing rsdID must emit DW004_REQUIRED_FIELD_MISSING");
    expect(result.oldPackagePreserved, "failed Detail write must preserve old package");
    expect(result.dirtyAfter, "failed Detail write must keep drawing dirty");
    expect(readText(oldXml) == oldXmlText, "failed write must not replace old Detail.xml");
    expect(readText(oldStl) == oldStlText, "failed write must not replace old Detail01.stl");
}

void testDetailWriterRejectsBrokenRebarReferencesBeforeWriting()
{
    QTemporaryDir temp;
    expect(temp.isValid(), "temporary dir must be valid");
    const QString outputDir = QDir(temp.path()).filePath("drawings");

    tsrebar::SteelData broken = steelDataWithMixedGroup();
    broken.groups.front().barIds = {"missing-bar"};

    const tsrebar::DetailWriter writer;
    const auto result = writer.writePackage(outputDir, broken, {});

    expect(!result.ok, "missing bar reference must fail");
    expect(result.errorCodes.contains("DW003_ID_CROSS_REFERENCE_FAILED"),
           "missing bar reference must emit DW003");
    expect(!QFile::exists(QDir(outputDir).filePath("Detail.xml")),
           "missing bar reference must not create Detail.xml");
    expect(!QFile::exists(QDir(outputDir).filePath("Detail01.stl")),
           "missing bar reference must not create Detail01.stl");

    tsrebar::SteelData missingSegment = steelDataWithMixedGroup();
    missingSegment.bars.front().segmentIds = {"missing-segment"};

    const auto segmentResult = writer.writePackage(outputDir, missingSegment, {});
    expect(!segmentResult.ok, "missing segment reference must fail");
    expect(segmentResult.errorCodes.contains("DW003_ID_CROSS_REFERENCE_FAILED"),
           "missing segment reference must emit DW003");
}

void testDetailWriterInstallFailureRestoresExistingPackage()
{
    QTemporaryDir temp;
    expect(temp.isValid(), "temporary dir must be valid");
    const QString outputDir = QDir(temp.path()).filePath("drawings");
    expect(QDir().mkpath(outputDir), "output dir must be created");
    const QString oldXml = QDir(outputDir).filePath("Detail.xml");
    const QString oldStl = QDir(outputDir).filePath("Detail01.stl");
    const QString oldStl2 = QDir(outputDir).filePath("Detail02.stl");
    writeText(oldXml, "<StyleRoot><Old value=\"style\" /></StyleRoot>");
    writeText(oldStl, "<DrawingRoot><Old value=\"drawing1\" /></DrawingRoot>");
    writeText(oldStl2, "<DrawingRoot><Old value=\"drawing2\" /></DrawingRoot>");
    const QString oldXmlText = readText(oldXml);
    const QString oldStlText = readText(oldStl);
    const QString oldStl2Text = readText(oldStl2);

    tsrebar::DetailWriteOptions options;
    options.testInjectInstallFailureAfterFirstCopy = true;

    const tsrebar::DetailWriter writer;
    const auto result = writer.writePackage(outputDir, steelDataWithMixedGroup(), options);

    expect(!result.ok, "injected install failure must fail");
    expect(result.errorCodes.contains("DW008_REPLACE_FAILED"),
           "install failure must emit DW008_REPLACE_FAILED");
    expect(result.oldPackagePreserved, "install failure must preserve old package hash");
    expect(result.dirtyAfter, "install failure must keep drawing dirty");
    expect(readText(oldXml) == oldXmlText, "install failure must restore old Detail.xml");
    expect(readText(oldStl) == oldStlText, "install failure must restore old Detail01.stl");
    expect(readText(oldStl2) == oldStl2Text, "install failure must restore stale Detail02.stl");
}

void testDetailWriterWritesMultipleDetailViewsWithLegacyNames()
{
    QTemporaryDir temp;
    expect(temp.isValid(), "temporary dir must be valid");
    const QString outputDir = QDir(temp.path()).filePath("drawings");

    tsrebar::DetailWriteOptions options;
    options.runId = "DW-UNIT-MULTI-001";
    options.drawingUnit = "m";
    options.drawingScale = "1";
    for (int index = 1; index <= 100; ++index) {
        tsrebar::DetailDrawingViewOptions view;
        view.viewId = QStringLiteral("view-%1").arg(index, 3, 10, QLatin1Char('0'));
        view.drawingName = QStringLiteral("view-name-%1").arg(index);
        view.modelFileName = QStringLiteral("model-%1.step").arg(index);
        view.drawingUnit = QStringLiteral("mm");
        view.drawingScale = QStringLiteral("1:%1").arg(index);
        view.generalScale = QStringLiteral("general-%1").arg(index);
        options.views.push_back(view);
    }

    const tsrebar::DetailWriter writer;
    const auto result = writer.writePackage(outputDir, steelDataWithMixedGroup(), options);

    expect(result.ok, "multi-view Detail writer must succeed");
    expect(result.files.contains("Detail.xml"), "multi-view result must report Detail.xml");
    expect(result.files.contains("Detail01.stl"), "multi-view result must report Detail01.stl");
    expect(result.files.contains("Detail02.stl"), "multi-view result must report Detail02.stl");
    expect(result.files.contains("Detail09.stl"), "multi-view result must report Detail09.stl");
    expect(result.files.contains("Detail10.stl"), "multi-view result must report Detail10.stl");
    expect(result.files.contains("Detail100.stl"), "multi-view result must report Detail100.stl");
    expect(!result.files.contains("Detail010.stl"), "legacy naming must not use Detail010.stl");
    expect(!result.files.contains("Detail0100.stl"), "legacy naming must not use Detail0100.stl");
    expect(rootName(QDir(outputDir).filePath("Detail01.stl")) == "DrawingRoot",
           "Detail01.stl root must be DrawingRoot");
    expect(rootName(QDir(outputDir).filePath("Detail10.stl")) == "DrawingRoot",
           "Detail10.stl root must be DrawingRoot");
    expect(rootName(QDir(outputDir).filePath("Detail100.stl")) == "DrawingRoot",
           "Detail100.stl root must be DrawingRoot");

    const auto view2 = findElementAttrs(QDir(outputDir).filePath("Detail02.stl"), "ViewPort");
    expect(view2.value("id") == "view-002", "Detail02 ViewPort id mismatch");
    const auto info2 = findElementAttrs(QDir(outputDir).filePath("Detail02.stl"), "General-Info");
    expect(info2.value("DrawingName") == "view-name-2",
           "Detail02 General-Info DrawingName mismatch");
    expect(info2.value("Model_FileName") == "model-2.step",
           "Detail02 General-Info Model_FileName mismatch");
    expect(info2.value("DrawingUnit") == "mm", "Detail02 DrawingUnit mismatch");
    expect(info2.value("DrawingScale") == "1:2", "Detail02 DrawingScale mismatch");
    expect(info2.value("GeneralScale") == "general-2", "Detail02 GeneralScale mismatch");

    const auto view10 = findElementAttrs(QDir(outputDir).filePath("Detail10.stl"), "ViewPort");
    expect(view10.value("id") == "view-010", "Detail10 ViewPort id mismatch");
    const auto info10 = findElementAttrs(QDir(outputDir).filePath("Detail10.stl"), "General-Info");
    expect(info10.value("DrawingName") == "view-name-10",
           "Detail10 General-Info DrawingName mismatch");

    const auto view100 = findElementAttrs(QDir(outputDir).filePath("Detail100.stl"), "ViewPort");
    expect(view100.value("id") == "view-100", "Detail100 ViewPort id mismatch");
    const auto info100 = findElementAttrs(QDir(outputDir).filePath("Detail100.stl"), "General-Info");
    expect(info100.value("DrawingName") == "view-name-100",
           "Detail100 General-Info DrawingName mismatch");
}

void testDetailWriterRemovesStaleDetailViewsOnSuccessfulInstall()
{
    QTemporaryDir temp;
    expect(temp.isValid(), "temporary dir must be valid");
    const QString outputDir = QDir(temp.path()).filePath("drawings");
    expect(QDir().mkpath(outputDir), "output dir must be created");

    writeText(QDir(outputDir).filePath("Detail.xml"),
              "<StyleRoot><Old value=\"style\" /></StyleRoot>");
    writeText(QDir(outputDir).filePath("Detail01.stl"),
              "<DrawingRoot><Old value=\"drawing1\" /></DrawingRoot>");
    writeText(QDir(outputDir).filePath("Detail02.stl"),
              "<DrawingRoot><Old value=\"drawing2\" /></DrawingRoot>");
    writeText(QDir(outputDir).filePath("Detail03.stl"),
              "<DrawingRoot><Old value=\"drawing3\" /></DrawingRoot>");
    writeText(QDir(outputDir).filePath("keep.txt"), "must stay");

    tsrebar::DetailWriteOptions options;
    for (int index = 1; index <= 2; ++index) {
        tsrebar::DetailDrawingViewOptions view;
        view.viewId = QStringLiteral("fresh-view-%1").arg(index);
        view.drawingName = QStringLiteral("fresh-view-name-%1").arg(index);
        options.views.push_back(view);
    }

    const tsrebar::DetailWriter writer;
    const auto result = writer.writePackage(outputDir, steelDataWithMixedGroup(), options);

    expect(result.ok, "stale cleanup write must succeed");
    expect(QFile::exists(QDir(outputDir).filePath("Detail01.stl")),
           "Detail01.stl must remain after two-view write");
    expect(QFile::exists(QDir(outputDir).filePath("Detail02.stl")),
           "Detail02.stl must remain after two-view write");
    expect(!QFile::exists(QDir(outputDir).filePath("Detail03.stl")),
           "stale Detail03.stl must be removed after successful install");
    expect(QFile::exists(QDir(outputDir).filePath("keep.txt")),
           "non-Detail files must be preserved");
}

} // namespace

int main()
{
    testDetailWriterMapsDomainRebarToDetailPackage();
    testDetailWriterFailurePreservesExistingPackage();
    testDetailWriterRejectsBrokenRebarReferencesBeforeWriting();
    testDetailWriterInstallFailureRestoresExistingPackage();
    testDetailWriterWritesMultipleDetailViewsWithLegacyNames();
    testDetailWriterRemovesStaleDetailViewsOnSuccessfulInstall();
    return 0;
}
