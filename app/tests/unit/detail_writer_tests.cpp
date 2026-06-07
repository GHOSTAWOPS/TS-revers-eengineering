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

QXmlStreamAttributes findElementAttrsUnderParent(const QString& path,
                                                 const QString& parentName,
                                                 const QString& name)
{
    QFile file(path);
    expect(file.open(QIODevice::ReadOnly | QIODevice::Text), "xml file must open");
    QXmlStreamReader reader(&file);
    bool inParent = false;
    int depth = 0;
    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.isStartElement()) {
            const QString currentName = reader.name().toString();
            if (!inParent && currentName == parentName) {
                inParent = true;
                depth = 1;
                continue;
            }
            if (inParent) {
                if (currentName == name) {
                    return reader.attributes();
                }
                ++depth;
            }
        } else if (reader.isEndElement() && inParent) {
            --depth;
            if (depth == 0) {
                break;
            }
        }
    }
    expect(false,
           qPrintable(QStringLiteral("missing xml element %1 under %2")
                          .arg(name, parentName)));
    return {};
}

QStringList directChildElementNames(const QString& path, const QString& parentName)
{
    QFile file(path);
    expect(file.open(QIODevice::ReadOnly | QIODevice::Text), "xml file must open");
    QXmlStreamReader reader(&file);
    QStringList children;
    bool inParent = false;
    int depth = 0;
    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.isStartElement()) {
            const QString name = reader.name().toString();
            if (!inParent && name == parentName) {
                inParent = true;
                depth = 1;
                continue;
            }
            if (inParent) {
                if (depth == 1) {
                    children.append(name);
                }
                ++depth;
            }
        } else if (reader.isEndElement() && inParent) {
            --depth;
            if (depth == 0) {
                break;
            }
        }
    }
    expect(inParent, qPrintable(QStringLiteral("missing parent element %1").arg(parentName)));
    return children;
}

void expectDirectChildren(const QString& path,
                          const QString& parentName,
                          const QStringList& expectedChildren)
{
    const QStringList actual = directChildElementNames(path, parentName);
    for (const QString& expected : expectedChildren) {
        expect(actual.contains(expected),
               qPrintable(QStringLiteral("%1 must contain child %2")
                               .arg(parentName, expected)));
    }
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

tsrebar::SteelData steelDataWithPointGroup()
{
    tsrebar::SteelBarSegment point;
    point.segmentId = "segment-point-001";
    point.barId = "bar-point-001";
    point.sequenceNo = 1;
    point.shapeType = tsrebar::SteelBarSegmentShape::Point;
    point.startPoint = {1.0, 2.0, 3.0};
    point.offset = {0.1, 0.2, 0.3};
    point.length = 0.0;
    point.evidence.push_back({"E-DETAIL-003", "pointStb StbGeo field skeleton"});

    tsrebar::SteelBar bar;
    bar.barId = "bar-point-001";
    bar.groupId = "group-point-001";
    bar.sequenceNo = 1;
    bar.displayNumber = "P1";
    bar.diameter = 20.0;
    bar.steelLevel = "HRB400";
    bar.length = 0.0;
    bar.shapeType = "pointStb";
    bar.segmentIds = {point.segmentId};
    bar.evidence.push_back({"E-DETAIL-003", "pointStb sample evidence"});

    tsrebar::SteelBarGroup group;
    group.groupId = "group-point-001";
    group.rsdId = "P1";
    group.displayNumber = "P1";
    group.actualNumber = "1";
    group.componentName = "pier";
    group.projectSteelName = "point bar";
    group.createCommand = "Rebar.Create.PointGroup";
    group.legacyCommand = "sgroupbarpoint";
    group.steelDataId = "steel-data-point-001";
    group.diameter = 20.0;
    group.interval = 200.0;
    group.barCount = 1;
    group.segmentCount = 1;
    group.steelLevel = "HRB400";
    group.layer = "inside";
    group.profile = "default-profile";
    group.use = "main";
    group.rangeLess180 = true;
    group.steelWay = "OTHER";
    group.rebarType = "pointStb";
    group.barIds.push_back(bar.barId);
    group.evidence.push_back({"E-DETAIL-003", "pointStb / FaceEdge static field evidence"});

    tsrebar::SteelData steelData;
    steelData.steelDataId = "steel-data-point-001";
    steelData.level = "HRB400";
    steelData.gradeName = "HRB400";
    steelData.diameterSet.push_back(20.0);
    steelData.evidence.push_back({"E-DETAIL-003", "Detail complex field evidence"});
    steelData.groups.push_back(group);
    steelData.bars.push_back(bar);
    steelData.segments.push_back(point);
    return steelData;
}

tsrebar::SteelData steelDataWithPointFaceEdgeGroups()
{
    tsrebar::SteelBarSegment pointLineEdge;
    pointLineEdge.segmentId = "segment-point-line-edge-001";
    pointLineEdge.barId = "bar-point-line-edge-001";
    pointLineEdge.sequenceNo = 1;
    pointLineEdge.shapeType = tsrebar::SteelBarSegmentShape::Point;
    pointLineEdge.startPoint = {4.94, -0.08, 0.0};
    pointLineEdge.offset = {0.0, 1.0, 0.0};
    pointLineEdge.evidence.push_back({"E-DETAIL-003", "pointStb StbGeo line FaceEdge skeleton"});

    tsrebar::SteelBarSegment pointArcEdge;
    pointArcEdge.segmentId = "segment-point-arc-edge-001";
    pointArcEdge.barId = "bar-point-arc-edge-001";
    pointArcEdge.sequenceNo = 1;
    pointArcEdge.shapeType = tsrebar::SteelBarSegmentShape::Point;
    pointArcEdge.startPoint = {4.94, 12.2694153788145, 0.0};
    pointArcEdge.offset = {-0.859628770301123, -0.510919149445956, 0.0};
    pointArcEdge.evidence.push_back({"E-DETAIL-003", "pointStb StbGeo arc FaceEdge skeleton"});

    tsrebar::SteelBar lineEdgeBar;
    lineEdgeBar.barId = "bar-point-line-edge-001";
    lineEdgeBar.groupId = "group-point-line-edge-001";
    lineEdgeBar.sequenceNo = 1;
    lineEdgeBar.displayNumber = "P-L";
    lineEdgeBar.diameter = 20.0;
    lineEdgeBar.steelLevel = "HRB400";
    lineEdgeBar.shapeType = "pointStb";
    lineEdgeBar.segmentIds = {pointLineEdge.segmentId};

    tsrebar::SteelBar arcEdgeBar;
    arcEdgeBar.barId = "bar-point-arc-edge-001";
    arcEdgeBar.groupId = "group-point-arc-edge-001";
    arcEdgeBar.sequenceNo = 1;
    arcEdgeBar.displayNumber = "P-A";
    arcEdgeBar.diameter = 20.0;
    arcEdgeBar.steelLevel = "HRB400";
    arcEdgeBar.shapeType = "pointStb";
    arcEdgeBar.segmentIds = {pointArcEdge.segmentId};

    tsrebar::RebarFaceEdgeGeometry lineEdge;
    lineEdge.shapeType = tsrebar::RebarFaceEdgeShape::Line;
    lineEdge.startPoint = {4.94, 0.0, 0.0};
    lineEdge.endPoint = {-4.99, 0.0, 0.0};
    lineEdge.evidence.push_back({"E-DETAIL-003", "FaceEdge shapeType=L field skeleton"});
    lineEdge.unresolvedLegacyFields.push_back({
        "FaceEdge.line.generationRule",
        "TODO-037 writes explicit fixture fields only; old generation rule remains a gap.",
        "GAP-DRAW-002"});

    tsrebar::RebarFaceEdgeGeometry arcEdge;
    arcEdge.shapeType = tsrebar::RebarFaceEdgeShape::Arc;
    arcEdge.arcDotReverse = false;
    arcEdge.startPoint = {4.87122969837591, 12.2285418468588, 0.0};
    arcEdge.middlePoint = {4.28372389791187, 13.0428913498069, 0.0};
    arcEdge.endPoint = {-4.92053364269146, 12.1439310957802, 0.0};
    arcEdge.evidence.push_back({"E-DETAIL-003", "FaceEdge shapeType=A field skeleton"});
    arcEdge.unresolvedLegacyFields.push_back({
        "FaceEdge.arc.generationRule",
        "TODO-037 writes explicit fixture fields only; m_ArcDotReverse source remains a gap.",
        "GAP-DRAW-002"});

    tsrebar::SteelBarGroup lineGroup;
    lineGroup.groupId = "group-point-line-edge-001";
    lineGroup.rsdId = "PFE-L";
    lineGroup.displayNumber = "P-L";
    lineGroup.actualNumber = "2";
    lineGroup.componentName = "pier";
    lineGroup.projectSteelName = "point line edge";
    lineGroup.createCommand = "Rebar.Create.PointGroup";
    lineGroup.legacyCommand = "sgroupbarpoint";
    lineGroup.steelDataId = "steel-data-point-face-edge-001";
    lineGroup.diameter = 20.0;
    lineGroup.secondaryDiameter = 25.0;
    lineGroup.interval = 20.0;
    lineGroup.barCount = 2;
    lineGroup.segmentCount = 1;
    lineGroup.steelLevel = "HRB400";
    lineGroup.layer = "inside";
    lineGroup.profile = "default-profile";
    lineGroup.use = "main";
    lineGroup.rangeLess180 = true;
    lineGroup.steelWay = "OTHER";
    lineGroup.rebarType = "pointStb";
    lineGroup.barIds.push_back(lineEdgeBar.barId);
    lineGroup.faceEdges.push_back(lineEdge);
    lineGroup.evidence.push_back({"E-DETAIL-003", "pointStb FaceEdge line static evidence"});

    tsrebar::SteelBarGroup arcGroup = lineGroup;
    arcGroup.groupId = "group-point-arc-edge-001";
    arcGroup.rsdId = "PFE-A";
    arcGroup.displayNumber = "P-A";
    arcGroup.projectSteelName = "point arc edge";
    arcGroup.barIds = {arcEdgeBar.barId};
    arcGroup.faceEdges = {arcEdge};
    arcGroup.evidence = {{"E-DETAIL-003", "pointStb FaceEdge arc static evidence"}};

    tsrebar::SteelData steelData;
    steelData.steelDataId = "steel-data-point-face-edge-001";
    steelData.level = "HRB400";
    steelData.gradeName = "HRB400";
    steelData.diameterSet.push_back(20.0);
    steelData.evidence.push_back({"E-DETAIL-003", "pointStb / FaceEdge static field evidence"});
    steelData.groups.push_back(lineGroup);
    steelData.groups.push_back(arcGroup);
    steelData.bars.push_back(lineEdgeBar);
    steelData.bars.push_back(arcEdgeBar);
    steelData.segments.push_back(pointLineEdge);
    steelData.segments.push_back(pointArcEdge);
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

void testDetailWriterWritesComplexPartDrawingSkeletonAndGeneralInfoDefaults()
{
    QTemporaryDir temp;
    expect(temp.isValid(), "temporary dir must be valid");
    const QString outputDir = QDir(temp.path()).filePath("drawings");

    tsrebar::DetailWriteOptions options;
    options.runId = "DW-UNIT-COMPLEX-001";
    options.drawingName = "complex-detail";
    options.modelFileName = "complex-model.step";

    const tsrebar::DetailWriter writer;
    const auto result = writer.writePackage(outputDir, steelDataWithMixedGroup(), options);

    expect(result.ok, "complex skeleton Detail writer must succeed");
    expect(result.l2 == "not_run", "complex skeleton must not claim AutoCAD L2");

    const QString detailStl = QDir(outputDir).filePath("Detail01.stl");
    const auto part = findElementAttrs(detailStl, "PartDetailDrawing");
    expect(part.value("num") == "8", "PartDetailDrawing.num must keep old sample skeleton value");

    const auto info = findElementAttrs(detailStl, "General-Info");
    expect(info.hasAttribute("CompanyName"), "General-Info.CompanyName must exist");
    expect(info.value("ExportYesNo") == "T", "General-Info.ExportYesNo default mismatch");
    expect(info.value("ExpSteelYesNo") == "T", "General-Info.ExpSteelYesNo default mismatch");
    expect(info.value("ExpSteelMark") == "T", "General-Info.ExpSteelMark default mismatch");
    expect(info.value("DimensionChicunB") == "T", "DimensionChicunB default mismatch");
    expect(info.value("DimensionChicunT") == "F", "DimensionChicunT default mismatch");
    expect(info.hasAttribute("DimensionPointBarB"), "DimensionPointBarB must exist");
    expect(info.hasAttribute("DimensionLineBarB"), "DimensionLineBarB must exist");
    expect(info.hasAttribute("DimensionLLineBarB"), "DimensionLLineBarB must exist");
    expect(info.value("DimensionBDist") == "15", "DimensionBDist default mismatch");
    expect(info.value("DispCuttedSymb") == "T", "DispCuttedSymb default mismatch");
    expect(info.hasAttribute("Range_XMLMin_X"), "Range_XMLMin_X must exist");
    expect(info.hasAttribute("Range_XMLMax_Y"), "Range_XMLMax_Y must exist");
    expect(info.value("CutPlaneDirY0") == "-1", "CutPlaneDirY0 default mismatch");
    expect(info.value("TopDirZ") == "1", "TopDirZ default mismatch");
    expect(info.value("DrawingType") == "0", "DrawingType default mismatch");
    expect(info.value("LevelDrawing") == "0", "LevelDrawing default mismatch");
    expect(info.value("DrawTaoTong") == "F", "DrawTaoTong default mismatch");

    expectDirectChildren(detailStl,
                         "continue-line",
                         {"lines", "circles", "Arcs", "Ellipses", "EllipseArcs", "Splines"});
    expectDirectChildren(detailStl,
                         "hidden-line",
                         {"lines", "circles", "Arcs", "Ellipses", "EllipseArcs", "Splines"});
    expectDirectChildren(detailStl, "central-line", {"lines"});
    expectDirectChildren(detailStl,
                         "section-line",
                         {"lines", "circles", "Arcs", "Ellipses", "EllipseArcs", "Splines"});
    expectDirectChildren(detailStl, "hatch-line", {"lines"});
    expectDirectChildren(detailStl, "steeljoint-line", {"joints"});
    findElementAttrs(detailStl, "Others");
}

void testDetailWriterWritesPointStbGeoFieldSkeleton()
{
    QTemporaryDir temp;
    expect(temp.isValid(), "temporary dir must be valid");
    const QString outputDir = QDir(temp.path()).filePath("drawings");

    const tsrebar::DetailWriter writer;
    const auto result = writer.writePackage(outputDir, steelDataWithPointGroup(), {});

    expect(result.ok, "pointStb field skeleton Detail writer must succeed");
    expect(result.l2 == "not_run", "pointStb skeleton must not claim AutoCAD L2");

    const QString detailStl = QDir(outputDir).filePath("Detail01.stl");
    const auto group = findElementAttrs(detailStl, "StbGroup1");
    expect(group.value("stbType") == "pointStb", "point group must keep pointStb type");

    const auto geo = findElementAttrs(detailStl, "StbGeo1");
    expect(geo.value("shapeType") == "C", "point StbGeo shape must be C");
    expect(geo.value("point_x") == "1", "point StbGeo point_x mismatch");
    expect(geo.value("point_y") == "2", "point StbGeo point_y mismatch");
    expect(geo.value("point_z") == "3", "point StbGeo point_z mismatch");
    expect(geo.value("offset_x") == "0.1", "point StbGeo offset_x mismatch");
    expect(geo.value("offset_y") == "0.2", "point StbGeo offset_y mismatch");
    expect(geo.value("offset_z") == "0.3", "point StbGeo offset_z mismatch");
    expect(geo.value("offset_x2") == "0", "point StbGeo offset_x2 default mismatch");
    expect(geo.value("offset_y2") == "0", "point StbGeo offset_y2 default mismatch");
    expect(geo.value("offset_z2") == "0", "point StbGeo offset_z2 default mismatch");
    expect(geo.value("start_x").isEmpty(), "point StbGeo must not emit line start_x");
    expect(geo.value("end_x").isEmpty(), "point StbGeo must not emit line end_x");
}

void testDetailWriterWritesPointStbFaceEdgeFieldSkeleton()
{
    QTemporaryDir temp;
    expect(temp.isValid(), "temporary dir must be valid");
    const QString outputDir = QDir(temp.path()).filePath("drawings");

    const tsrebar::DetailWriter writer;
    const auto result = writer.writePackage(outputDir, steelDataWithPointFaceEdgeGroups(), {});

    expect(result.ok, "pointStb FaceEdge field skeleton Detail writer must succeed");
    expect(result.l2 == "not_run", "pointStb FaceEdge skeleton must not claim AutoCAD L2");

    const QString detailStl = QDir(outputDir).filePath("Detail01.stl");
    const auto group1 = findElementAttrs(detailStl, "StbGroup1");
    expect(group1.value("stbType") == "pointStb", "line FaceEdge point group must keep pointStb type");
    const auto geo1 = findElementAttrs(detailStl, "StbGeo1");
    expect(geo1.value("shapeType") == "C", "line FaceEdge point StbGeo shape must be C");
    const auto lineFaceEdge = findElementAttrs(detailStl, "FaceEdge");
    expect(lineFaceEdge.value("shapeType") == "L", "FaceEdge line shapeType mismatch");
    expect(lineFaceEdge.value("start_x") == "4.94", "FaceEdge line start_x mismatch");
    expect(lineFaceEdge.value("start_y") == "0", "FaceEdge line start_y mismatch");
    expect(lineFaceEdge.value("end_x") == "-4.99", "FaceEdge line end_x mismatch");
    expect(lineFaceEdge.value("end_y") == "0", "FaceEdge line end_y mismatch");
    expect(lineFaceEdge.value("middle_x").isEmpty(), "FaceEdge line must not emit middle_x");
    expect(lineFaceEdge.value("m_ArcDotReverse").isEmpty(), "FaceEdge line must not emit m_ArcDotReverse");

    const auto group2 = findElementAttrs(detailStl, "StbGroup2");
    expect(group2.value("stbType") == "pointStb", "arc FaceEdge point group must keep pointStb type");

    int faceEdgeCount = 0;
    bool sawArcFaceEdge = false;
    QFile file(detailStl);
    expect(file.open(QIODevice::ReadOnly | QIODevice::Text), "detail stl must open");
    QXmlStreamReader reader(&file);
    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.isStartElement() && reader.name() == "FaceEdge") {
            ++faceEdgeCount;
            const auto attrs = reader.attributes();
            if (attrs.value("shapeType") == "A") {
                sawArcFaceEdge = true;
                expect(attrs.value("m_ArcDotReverse") == "F", "FaceEdge arc reverse flag mismatch");
                expect(attrs.value("start_x") == "4.87122969837591", "FaceEdge arc start_x mismatch");
                expect(attrs.value("middle_y") == "13.0428913498069", "FaceEdge arc middle_y mismatch");
                expect(attrs.value("end_x") == "-4.92053364269146", "FaceEdge arc end_x mismatch");
            }
        }
    }
    expect(faceEdgeCount == 2, "pointStb fixture must write one FaceEdge per group");
    expect(sawArcFaceEdge, "pointStb fixture must write arc FaceEdge skeleton");
}

void testDetailWriterWritesSectionLineFieldSkeleton()
{
    QTemporaryDir temp;
    expect(temp.isValid(), "temporary dir must be valid");
    const QString outputDir = QDir(temp.path()).filePath("drawings");

    tsrebar::DetailWriteOptions options;
    tsrebar::DetailDrawingViewOptions view;
    view.viewId = "section-line-view-001";
    view.drawingName = "todo038-section-line";
    view.modelFileName = "todo038-model.step";
    view.drawingUnit = "mm";
    view.drawingScale = "1:50";
    view.generalScale = "50";
    view.sectionLines.push_back({
        6.00000000005,
        -1.0,
        -6.0,
        -1.0,
        "-1.000000:-1.000000:12.000000"});
    view.sectionArcs.push_back({
        1.38015820994e-13,
        9.33333333333,
        0.5,
        6.66666666667,
        0.451026811805728,
        2.69056584178407,
        "12.239266:9.333333:12.239266"});
    options.views.push_back(view);

    const tsrebar::DetailWriter writer;
    const auto result = writer.writePackage(outputDir, steelDataWithMixedGroup(), options);

    expect(result.ok, "section-line field skeleton Detail writer must succeed");
    expect(result.l2 == "not_run", "section-line skeleton must not claim AutoCAD L2");

    const QString detailStl = QDir(outputDir).filePath("Detail01.stl");
    expectDirectChildren(detailStl,
                         "section-line",
                         {"lines", "circles", "Arcs", "Ellipses", "EllipseArcs", "Splines"});

    const auto line = findElementAttrs(detailStl, "Line1");
    expect(line.value("start_x") == "6.00000000005", "section Line1 start_x mismatch");
    expect(line.value("start_y") == "-1", "section Line1 start_y mismatch");
    expect(line.value("end_x") == "-6", "section Line1 end_x mismatch");
    expect(line.value("end_y") == "-1", "section Line1 end_y mismatch");
    expect(line.value("ZValue") == "-1.000000:-1.000000:12.000000",
           "section Line1 ZValue mismatch");

    const auto arc = findElementAttrs(detailStl, "Arc1");
    expect(arc.hasAttribute("center_x"), "section Arc1 center_x must exist");
    expect(arc.value("center_y") == "9.33333333333", "section Arc1 center_y mismatch");
    expect(arc.value("center_z") == "0.5", "section Arc1 center_z mismatch");
    expect(arc.value("radius") == "6.66666666667", "section Arc1 radius mismatch");
    expect(arc.value("start_angle") == "0.451026811805728",
           "section Arc1 start_angle mismatch");
    expect(arc.value("end_angle") == "2.69056584178407",
           "section Arc1 end_angle mismatch");
    expect(arc.value("ZValue") == "12.239266:9.333333:12.239266",
           "section Arc1 ZValue mismatch");
}

void testDetailWriterWritesLineContainerFieldSkeleton()
{
    QTemporaryDir temp;
    expect(temp.isValid(), "temporary dir must be valid");
    const QString outputDir = QDir(temp.path()).filePath("drawings");

    tsrebar::DetailWriteOptions options;
    tsrebar::DetailDrawingViewOptions view;
    view.viewId = "line-containers-view-001";
    view.drawingName = "todo040-line-containers";
    view.modelFileName = "todo040-model.step";
    view.drawingUnit = "mm";
    view.drawingScale = "1:100";
    view.generalScale = "100";
    view.continueLines.push_back({1.0, 2.0, 3.0, 4.0, "continue-z"});
    view.hiddenLines.push_back({5.0, 6.0, 7.0, 8.0, "hidden-z"});
    view.centralLines.push_back({9.0, 10.0, 11.0, 12.0, "central-z"});
    view.hatchLines.push_back({13.0, 14.0, 15.0, 16.0, "hatch-z"});
    options.views.push_back(view);

    const tsrebar::DetailWriter writer;
    const auto result = writer.writePackage(outputDir, steelDataWithMixedGroup(), options);

    expect(result.ok, "line container field skeleton Detail writer must succeed");
    expect(result.l2 == "not_run", "line container skeleton must not claim AutoCAD L2");

    const QString detailStl = QDir(outputDir).filePath("Detail01.stl");
    expectDirectChildren(detailStl,
                         "continue-line",
                         {"lines", "circles", "Arcs", "Ellipses", "EllipseArcs", "Splines"});
    expectDirectChildren(detailStl,
                         "hidden-line",
                         {"lines", "circles", "Arcs", "Ellipses", "EllipseArcs", "Splines"});
    expectDirectChildren(detailStl, "central-line", {"lines"});
    expectDirectChildren(detailStl, "hatch-line", {"lines"});

    const auto continueLine = findElementAttrsUnderParent(detailStl, "continue-line", "Line1");
    expect(continueLine.value("start_x") == "1", "continue-line Line1 start_x mismatch");
    expect(continueLine.value("start_y") == "2", "continue-line Line1 start_y mismatch");
    expect(continueLine.value("end_x") == "3", "continue-line Line1 end_x mismatch");
    expect(continueLine.value("end_y") == "4", "continue-line Line1 end_y mismatch");
    expect(continueLine.value("ZValue") == "continue-z", "continue-line Line1 ZValue mismatch");

    const auto hiddenLine = findElementAttrsUnderParent(detailStl, "hidden-line", "Line1");
    expect(hiddenLine.value("start_x") == "5", "hidden-line Line1 start_x mismatch");
    expect(hiddenLine.value("start_y") == "6", "hidden-line Line1 start_y mismatch");
    expect(hiddenLine.value("end_x") == "7", "hidden-line Line1 end_x mismatch");
    expect(hiddenLine.value("end_y") == "8", "hidden-line Line1 end_y mismatch");
    expect(hiddenLine.value("ZValue") == "hidden-z", "hidden-line Line1 ZValue mismatch");

    const auto centralLine = findElementAttrsUnderParent(detailStl, "central-line", "Line1");
    expect(centralLine.value("start_x") == "9", "central-line Line1 start_x mismatch");
    expect(centralLine.value("end_y") == "12", "central-line Line1 end_y mismatch");
    expect(centralLine.value("ZValue") == "central-z", "central-line Line1 ZValue mismatch");

    const auto hatchLine = findElementAttrsUnderParent(detailStl, "hatch-line", "Line1");
    expect(hatchLine.value("start_x") == "13", "hatch-line Line1 start_x mismatch");
    expect(hatchLine.value("end_y") == "16", "hatch-line Line1 end_y mismatch");
    expect(hatchLine.value("ZValue") == "hatch-z", "hatch-line Line1 ZValue mismatch");
}

void testDetailWriterWritesOthersAndSteeljointContainerSkeleton()
{
    QTemporaryDir temp;
    expect(temp.isValid(), "temporary dir must be valid");
    const QString outputDir = QDir(temp.path()).filePath("drawings");

    tsrebar::DetailWriteOptions options;
    tsrebar::DetailDrawingViewOptions view;
    view.viewId = "others-steeljoint-view-001";
    view.drawingName = "todo043-others-steeljoint";
    view.modelFileName = "todo043-model.step";
    options.views.push_back(view);

    const tsrebar::DetailWriter writer;
    const auto result = writer.writePackage(outputDir, steelDataWithMixedGroup(), options);

    expect(result.ok, "Others / steeljoint-line field skeleton Detail writer must succeed");
    expect(result.l2 == "not_run", "Others / steeljoint-line skeleton must not claim AutoCAD L2");

    const QString detailStl = QDir(outputDir).filePath("Detail01.stl");
    const QStringList partChildren = directChildElementNames(detailStl, "PartDetailDrawing");
    expect(partChildren.contains("Others"), "PartDetailDrawing must contain Others");
    expect(partChildren.contains("steeljoint-line"), "PartDetailDrawing must contain steeljoint-line");
    expect(directChildElementNames(detailStl, "Others").isEmpty(),
           "Others skeleton must stay empty until old semantics are confirmed");
    expectDirectChildren(detailStl, "steeljoint-line", {"joints"});
    expect(directChildElementNames(detailStl, "joints").isEmpty(),
           "steeljoint-line joints skeleton must stay empty until old joint algorithm is confirmed");
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
    testDetailWriterWritesComplexPartDrawingSkeletonAndGeneralInfoDefaults();
    testDetailWriterWritesPointStbGeoFieldSkeleton();
    testDetailWriterWritesPointStbFaceEdgeFieldSkeleton();
    testDetailWriterWritesSectionLineFieldSkeleton();
    testDetailWriterWritesLineContainerFieldSkeleton();
    testDetailWriterWritesOthersAndSteeljointContainerSkeleton();
    testDetailWriterFailurePreservesExistingPackage();
    testDetailWriterRejectsBrokenRebarReferencesBeforeWriting();
    testDetailWriterInstallFailureRestoresExistingPackage();
    testDetailWriterWritesMultipleDetailViewsWithLegacyNames();
    testDetailWriterRemovesStaleDetailViewsOnSuccessfulInstall();
    return 0;
}
