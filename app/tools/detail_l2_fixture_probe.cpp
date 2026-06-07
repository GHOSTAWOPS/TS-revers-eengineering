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
#include <QSettings>
#include <QStandardPaths>
#include <QTextStream>
#include <QXmlStreamAttributes>
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
    line.segmentId = "todo036-segment-line-001";
    line.barId = "todo036-bar-001";
    line.sequenceNo = 1;
    line.shapeType = tsrebar::SteelBarSegmentShape::Line;
    line.startPoint = {0.0, 0.0, 0.0};
    line.middlePoint = {5.0, 0.0, 0.0};
    line.endPoint = {10.0, 0.0, 0.0};
    line.length = 10.0;
    line.evidence.push_back(evidenceRef("E-IDA-018", "StbGeo write chain"));

    tsrebar::SteelBarSegment arc;
    arc.segmentId = "todo036-segment-arc-001";
    arc.barId = "todo036-bar-001";
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
    bar.barId = "todo036-bar-001";
    bar.groupId = "todo036-group-001";
    bar.sequenceNo = 1;
    bar.displayNumber = "12A";
    bar.diameter = 25.0;
    bar.steelLevel = "HRB400";
    bar.length = 15.0;
    bar.shapeType = "lineStb";
    bar.segmentIds = {line.segmentId, arc.segmentId};
    bar.evidence.push_back(evidenceRef("E-IDA-017", "steelbar evidence"));

    tsrebar::SteelBarGroup group;
    group.groupId = "todo036-group-001";
    group.rsdId = "Y12";
    group.displayNumber = "12A";
    group.actualNumber = "12";
    group.componentName = "todo036-pier";
    group.projectSteelName = "todo036-main-bar";
    group.createCommand = "Rebar.Create.LineGroup";
    group.legacyCommand = "sgroupbarline";
    group.steelDataId = "todo036-steel-data-001";
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
    steelData.steelDataId = "todo036-steel-data-001";
    steelData.level = "HRB400";
    steelData.gradeName = "HRB400";
    steelData.diameterSet.push_back(25.0);
    steelData.evidence.push_back(evidenceRef("E-DETAIL-001", "Detail package evidence"));
    steelData.evidence.push_back(evidenceRef("E-DETAIL-003", "Detail complex field evidence"));
    steelData.groups.push_back(group);
    steelData.bars.push_back(bar);
    steelData.segments.push_back(line);
    steelData.segments.push_back(arc);
    return steelData;
}

tsrebar::SteelData makePointFaceEdgeProbeSteelData()
{
    tsrebar::SteelBarSegment linePoint;
    linePoint.segmentId = "todo037-segment-point-line-edge-001";
    linePoint.barId = "todo037-bar-point-line-edge-001";
    linePoint.sequenceNo = 1;
    linePoint.shapeType = tsrebar::SteelBarSegmentShape::Point;
    linePoint.startPoint = {4.94, -0.08, 0.0};
    linePoint.offset = {0.0, 1.0, 0.0};
    linePoint.evidence.push_back(evidenceRef("E-DETAIL-003", "pointStb StbGeo line FaceEdge skeleton"));

    tsrebar::SteelBarSegment arcPoint;
    arcPoint.segmentId = "todo037-segment-point-arc-edge-001";
    arcPoint.barId = "todo037-bar-point-arc-edge-001";
    arcPoint.sequenceNo = 1;
    arcPoint.shapeType = tsrebar::SteelBarSegmentShape::Point;
    arcPoint.startPoint = {4.94, 12.2694153788145, 0.0};
    arcPoint.offset = {-0.859628770301123, -0.510919149445956, 0.0};
    arcPoint.evidence.push_back(evidenceRef("E-DETAIL-003", "pointStb StbGeo arc FaceEdge skeleton"));

    tsrebar::SteelBar lineBar;
    lineBar.barId = "todo037-bar-point-line-edge-001";
    lineBar.groupId = "todo037-group-point-line-edge-001";
    lineBar.sequenceNo = 1;
    lineBar.displayNumber = "P-L";
    lineBar.diameter = 20.0;
    lineBar.steelLevel = "HRB400";
    lineBar.shapeType = "pointStb";
    lineBar.segmentIds = {linePoint.segmentId};
    lineBar.evidence.push_back(evidenceRef("E-DETAIL-003", "pointStb line FaceEdge sample evidence"));

    tsrebar::SteelBar arcBar;
    arcBar.barId = "todo037-bar-point-arc-edge-001";
    arcBar.groupId = "todo037-group-point-arc-edge-001";
    arcBar.sequenceNo = 1;
    arcBar.displayNumber = "P-A";
    arcBar.diameter = 20.0;
    arcBar.steelLevel = "HRB400";
    arcBar.shapeType = "pointStb";
    arcBar.segmentIds = {arcPoint.segmentId};
    arcBar.evidence.push_back(evidenceRef("E-DETAIL-003", "pointStb arc FaceEdge sample evidence"));

    tsrebar::RebarFaceEdgeGeometry lineEdge;
    lineEdge.shapeType = tsrebar::RebarFaceEdgeShape::Line;
    lineEdge.startPoint = {4.94, 0.0, 0.0};
    lineEdge.endPoint = {-4.99, 0.0, 0.0};
    lineEdge.evidence.push_back(evidenceRef("E-DETAIL-003", "FaceEdge shapeType=L field skeleton"));
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
    arcEdge.evidence.push_back(evidenceRef("E-DETAIL-003", "FaceEdge shapeType=A field skeleton"));
    arcEdge.unresolvedLegacyFields.push_back({
        "FaceEdge.arc.generationRule",
        "TODO-037 writes explicit fixture fields only; m_ArcDotReverse source remains a gap.",
        "GAP-DRAW-002"});

    tsrebar::SteelBarGroup lineGroup;
    lineGroup.groupId = "todo037-group-point-line-edge-001";
    lineGroup.rsdId = "PFE-L";
    lineGroup.displayNumber = "P-L";
    lineGroup.actualNumber = "2";
    lineGroup.componentName = "todo037-pier";
    lineGroup.projectSteelName = "todo037-point-line-edge";
    lineGroup.createCommand = "Rebar.Create.PointGroup";
    lineGroup.legacyCommand = "sgroupbarpoint";
    lineGroup.steelDataId = "todo037-steel-data-point-face-edge-001";
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
    lineGroup.barIds.push_back(lineBar.barId);
    lineGroup.faceEdges.push_back(lineEdge);
    lineGroup.evidence.push_back(evidenceRef("E-DETAIL-003", "pointStb FaceEdge line static evidence"));

    tsrebar::SteelBarGroup arcGroup = lineGroup;
    arcGroup.groupId = "todo037-group-point-arc-edge-001";
    arcGroup.rsdId = "PFE-A";
    arcGroup.displayNumber = "P-A";
    arcGroup.projectSteelName = "todo037-point-arc-edge";
    arcGroup.barIds = {arcBar.barId};
    arcGroup.faceEdges = {arcEdge};
    arcGroup.evidence = {evidenceRef("E-DETAIL-003", "pointStb FaceEdge arc static evidence")};

    tsrebar::SteelData steelData;
    steelData.steelDataId = "todo037-steel-data-point-face-edge-001";
    steelData.level = "HRB400";
    steelData.gradeName = "HRB400";
    steelData.diameterSet.push_back(20.0);
    steelData.evidence.push_back(evidenceRef("E-DETAIL-003", "pointStb / FaceEdge static field evidence"));
    steelData.groups.push_back(lineGroup);
    steelData.groups.push_back(arcGroup);
    steelData.bars.push_back(lineBar);
    steelData.bars.push_back(arcBar);
    steelData.segments.push_back(linePoint);
    steelData.segments.push_back(arcPoint);
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

QJsonObject fileSummaryForPath(const QString& path, const QString& displayName = {});

QJsonObject fileSummary(const QString& outputDir, const QString& fileName)
{
    const QString path = QDir(outputDir).filePath(fileName);
    return fileSummaryForPath(path, fileName);
}

QJsonObject fileSummaryForPath(const QString& path, const QString& displayName)
{
    QFile file(path);
    QJsonObject item;
    item.insert(QStringLiteral("name"),
                displayName.isEmpty() ? QFileInfo(path).fileName() : displayName);
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

QStringList directChildElementNames(const QString& path, const QString& parentName)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return {};
    }

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
    return children;
}

QXmlStreamAttributes firstElementAttributes(const QString& path, const QString& elementName)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return {};
    }

    QXmlStreamReader reader(&file);
    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.isStartElement() && reader.name() == elementName) {
            return reader.attributes();
        }
    }
    return {};
}

QJsonArray toJsonArray(const QStringList& values)
{
    QJsonArray result;
    for (const QString& value : values) {
        result.append(value);
    }
    return result;
}

QStringList missingItems(const QStringList& actual, const QStringList& expected)
{
    QStringList missing;
    for (const QString& item : expected) {
        if (!actual.contains(item)) {
            missing.append(item);
        }
    }
    return missing;
}

QStringList missingAttributes(const QXmlStreamAttributes& attrs, const QStringList& expected)
{
    QStringList missing;
    for (const QString& item : expected) {
        if (!attrs.hasAttribute(item)) {
            missing.append(item);
        }
    }
    return missing;
}

bool numberedElementName(const QString& name, const QString& prefix)
{
    if (!name.startsWith(prefix) || name.size() == prefix.size()) {
        return false;
    }
    for (int index = prefix.size(); index < name.size(); ++index) {
        if (!name.at(index).isDigit()) {
            return false;
        }
    }
    return true;
}

QJsonObject attributesToJson(const QXmlStreamAttributes& attrs)
{
    QJsonObject result;
    for (const QXmlStreamAttribute& attr : attrs) {
        result.insert(attr.name().toString(), attr.value().toString());
    }
    return result;
}

QJsonObject childContainerCheck(const QString& path,
                                const QString& parentName,
                                const QStringList& expectedChildren)
{
    const QStringList actual = directChildElementNames(path, parentName);
    const QStringList missing = missingItems(actual, expectedChildren);
    QJsonObject result;
    result.insert(QStringLiteral("parent"), parentName);
    result.insert(QStringLiteral("actualChildren"), toJsonArray(actual));
    result.insert(QStringLiteral("requiredChildren"), toJsonArray(expectedChildren));
    result.insert(QStringLiteral("missingChildren"), toJsonArray(missing));
    result.insert(QStringLiteral("passed"), missing.isEmpty());
    return result;
}

QJsonObject complexSkeletonProbe(const QString& detailStlPath)
{
    const QStringList partChildren{
        QStringLiteral("General-Info"),
        QStringLiteral("continue-line"),
        QStringLiteral("hidden-line"),
        QStringLiteral("central-line"),
        QStringLiteral("section-line"),
        QStringLiteral("hatch-line"),
        QStringLiteral("Others"),
        QStringLiteral("steeljoint-line"),
    };
    const QStringList curveChildren{
        QStringLiteral("lines"),
        QStringLiteral("circles"),
        QStringLiteral("Arcs"),
        QStringLiteral("Ellipses"),
        QStringLiteral("EllipseArcs"),
        QStringLiteral("Splines"),
    };
    const QStringList generalAttrs{
        QStringLiteral("CompanyName"),
        QStringLiteral("ExportYesNo"),
        QStringLiteral("ExpSteelYesNo"),
        QStringLiteral("ExpSteelMark"),
        QStringLiteral("DimensionChicunB"),
        QStringLiteral("DimensionPointBarB"),
        QStringLiteral("DimensionLineBarB"),
        QStringLiteral("DimensionLLineBarB"),
        QStringLiteral("DimensionBDist"),
        QStringLiteral("DispCuttedSymb"),
        QStringLiteral("HalfViewH"),
        QStringLiteral("HalfViewW"),
        QStringLiteral("Range_XMLMin_X"),
        QStringLiteral("Range_XMLMax_Y"),
        QStringLiteral("CutPlaneDirX0"),
        QStringLiteral("CutPlaneDirY0"),
        QStringLiteral("CutPlaneDirZ0"),
        QStringLiteral("TopDirZ"),
        QStringLiteral("DrawingType"),
        QStringLiteral("LevelDrawing"),
        QStringLiteral("DrawTaoTong"),
    };
    QJsonObject result;
    result.insert(QStringLiteral("file"), QFileInfo(detailStlPath).absoluteFilePath());

    const QXmlStreamAttributes partAttrs =
        firstElementAttributes(detailStlPath, QStringLiteral("PartDetailDrawing"));
    const QStringList actualPartChildren =
        directChildElementNames(detailStlPath, QStringLiteral("PartDetailDrawing"));
    const QStringList missingPartChildren = missingItems(actualPartChildren, partChildren);
    result.insert(QStringLiteral("partDetailDrawingNum"),
                  partAttrs.value(QStringLiteral("num")).toString());
    result.insert(QStringLiteral("partDetailDrawingRequiredChildren"), toJsonArray(partChildren));
    result.insert(QStringLiteral("partDetailDrawingActualChildren"), toJsonArray(actualPartChildren));
    result.insert(QStringLiteral("partDetailDrawingMissingChildren"), toJsonArray(missingPartChildren));

    QJsonArray containerChecks;
    containerChecks.append(childContainerCheck(detailStlPath, QStringLiteral("continue-line"), curveChildren));
    containerChecks.append(childContainerCheck(detailStlPath, QStringLiteral("hidden-line"), curveChildren));
    containerChecks.append(childContainerCheck(detailStlPath, QStringLiteral("central-line"), {QStringLiteral("lines")}));
    containerChecks.append(childContainerCheck(detailStlPath, QStringLiteral("section-line"), curveChildren));
    containerChecks.append(childContainerCheck(detailStlPath, QStringLiteral("hatch-line"), {QStringLiteral("lines")}));
    containerChecks.append(childContainerCheck(detailStlPath, QStringLiteral("steeljoint-line"), {QStringLiteral("joints")}));
    result.insert(QStringLiteral("containerChecks"), containerChecks);

    bool containersPassed = true;
    for (const auto item : containerChecks) {
        if (!item.toObject().value(QStringLiteral("passed")).toBool()) {
            containersPassed = false;
        }
    }

    const QXmlStreamAttributes general =
        firstElementAttributes(detailStlPath, QStringLiteral("General-Info"));
    const QStringList missingGeneralAttrs = missingAttributes(general, generalAttrs);
    QJsonObject generalInfo;
    generalInfo.insert(QStringLiteral("requiredAttributes"), toJsonArray(generalAttrs));
    generalInfo.insert(QStringLiteral("missingAttributes"), toJsonArray(missingGeneralAttrs));
    generalInfo.insert(QStringLiteral("CompanyName"), general.value(QStringLiteral("CompanyName")).toString());
    generalInfo.insert(QStringLiteral("DrawingName"), general.value(QStringLiteral("DrawingName")).toString());
    generalInfo.insert(QStringLiteral("DrawingUnit"), general.value(QStringLiteral("DrawingUnit")).toString());
    generalInfo.insert(QStringLiteral("DrawingScale"), general.value(QStringLiteral("DrawingScale")).toString());
    generalInfo.insert(QStringLiteral("passed"), missingGeneralAttrs.isEmpty());
    result.insert(QStringLiteral("generalInfo"), generalInfo);

    const bool passed = partAttrs.value(QStringLiteral("num")).toString() == QStringLiteral("8") &&
        missingPartChildren.isEmpty() &&
        containersPassed &&
        missingGeneralAttrs.isEmpty();
    result.insert(QStringLiteral("passed"), passed);
    result.insert(QStringLiteral("scope"),
                  QStringLiteral("complex containers + General-Info only; pointStb L2 is deferred"));
    return result;
}

QJsonObject pointFaceEdgeProbe(const QString& detailStlPath)
{
    QJsonObject result;
    result.insert(QStringLiteral("file"), QFileInfo(detailStlPath).absoluteFilePath());
    result.insert(QStringLiteral("scope"),
                  QStringLiteral("pointStb StbGeo shapeType=C + FaceEdge L/A field skeleton only"));

    int pointGroupCount = 0;
    int pointGeoCount = 0;
    int faceEdgeCount = 0;
    bool lineFaceEdgePassed = false;
    bool arcFaceEdgePassed = false;
    bool allPointGeoFieldsPresent = true;
    QJsonArray faceEdges;

    QFile file(detailStlPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        result.insert(QStringLiteral("passed"), false);
        result.insert(QStringLiteral("diagnostic"), QStringLiteral("detail file open failed"));
        return result;
    }

    QXmlStreamReader reader(&file);
    while (!reader.atEnd()) {
        reader.readNext();
        if (!reader.isStartElement()) {
            continue;
        }

        const QString name = reader.name().toString();
        const QXmlStreamAttributes attrs = reader.attributes();
        if (name.startsWith(QStringLiteral("StbGroup")) &&
            attrs.value(QStringLiteral("stbType")).toString() == QStringLiteral("pointStb")) {
            ++pointGroupCount;
        }
        if (name.startsWith(QStringLiteral("StbGeo")) &&
            attrs.value(QStringLiteral("shapeType")).toString() == QStringLiteral("C")) {
            ++pointGeoCount;
            const QStringList required{
                QStringLiteral("point_x"),
                QStringLiteral("point_y"),
                QStringLiteral("point_z"),
                QStringLiteral("offset_x"),
                QStringLiteral("offset_y"),
                QStringLiteral("offset_z"),
                QStringLiteral("offset_x2"),
                QStringLiteral("offset_y2"),
                QStringLiteral("offset_z2"),
            };
            if (!missingAttributes(attrs, required).isEmpty()) {
                allPointGeoFieldsPresent = false;
            }
        }
        if (name == QStringLiteral("FaceEdge")) {
            ++faceEdgeCount;
            QJsonObject item;
            const QString shapeType = attrs.value(QStringLiteral("shapeType")).toString();
            item.insert(QStringLiteral("shapeType"), shapeType);
            item.insert(QStringLiteral("attributes"), attributesToJson(attrs));
            if (shapeType == QStringLiteral("L")) {
                const QStringList required{
                    QStringLiteral("start_x"),
                    QStringLiteral("start_y"),
                    QStringLiteral("end_x"),
                    QStringLiteral("end_y"),
                };
                const QStringList missing = missingAttributes(attrs, required);
                item.insert(QStringLiteral("missingAttributes"), toJsonArray(missing));
                lineFaceEdgePassed = missing.isEmpty() &&
                    !attrs.hasAttribute(QStringLiteral("middle_x")) &&
                    !attrs.hasAttribute(QStringLiteral("m_ArcDotReverse"));
            } else if (shapeType == QStringLiteral("A")) {
                const QStringList required{
                    QStringLiteral("m_ArcDotReverse"),
                    QStringLiteral("start_x"),
                    QStringLiteral("start_y"),
                    QStringLiteral("middle_x"),
                    QStringLiteral("middle_y"),
                    QStringLiteral("end_x"),
                    QStringLiteral("end_y"),
                };
                const QStringList missing = missingAttributes(attrs, required);
                item.insert(QStringLiteral("missingAttributes"), toJsonArray(missing));
                arcFaceEdgePassed = missing.isEmpty();
            }
            faceEdges.append(item);
        }
    }

    const bool passed = pointGroupCount >= 2 &&
        pointGeoCount >= 2 &&
        allPointGeoFieldsPresent &&
        faceEdgeCount == 2 &&
        lineFaceEdgePassed &&
        arcFaceEdgePassed;
    result.insert(QStringLiteral("pointGroupCount"), pointGroupCount);
    result.insert(QStringLiteral("pointGeoCount"), pointGeoCount);
    result.insert(QStringLiteral("allPointGeoFieldsPresent"), allPointGeoFieldsPresent);
    result.insert(QStringLiteral("faceEdgeCount"), faceEdgeCount);
    result.insert(QStringLiteral("lineFaceEdgePassed"), lineFaceEdgePassed);
    result.insert(QStringLiteral("arcFaceEdgePassed"), arcFaceEdgePassed);
    result.insert(QStringLiteral("faceEdges"), faceEdges);
    result.insert(QStringLiteral("passed"), passed);
    return result;
}

QJsonObject sectionLineProbe(const QString& detailStlPath)
{
    QJsonObject result;
    result.insert(QStringLiteral("file"), QFileInfo(detailStlPath).absoluteFilePath());
    result.insert(QStringLiteral("scope"),
                  QStringLiteral("section-line LineN / ArcN / ZValue field skeleton only"));

    int lineCount = 0;
    int arcCount = 0;
    bool lineFieldsPassed = false;
    bool arcFieldsPassed = false;
    QJsonArray lines;
    QJsonArray arcs;

    QFile file(detailStlPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        result.insert(QStringLiteral("passed"), false);
        result.insert(QStringLiteral("diagnostic"), QStringLiteral("detail file open failed"));
        return result;
    }

    QXmlStreamReader reader(&file);
    bool inSectionLine = false;
    int sectionDepth = 0;
    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.isStartElement()) {
            const QString name = reader.name().toString();
            if (!inSectionLine && name == QStringLiteral("section-line")) {
                inSectionLine = true;
                sectionDepth = 1;
                continue;
            }
            if (!inSectionLine) {
                continue;
            }
            ++sectionDepth;
            const QXmlStreamAttributes attrs = reader.attributes();
            if (numberedElementName(name, QStringLiteral("Line"))) {
                ++lineCount;
                const QStringList required{
                    QStringLiteral("start_x"),
                    QStringLiteral("start_y"),
                    QStringLiteral("end_x"),
                    QStringLiteral("end_y"),
                    QStringLiteral("ZValue"),
                };
                const QStringList missing = missingAttributes(attrs, required);
                QJsonObject item;
                item.insert(QStringLiteral("name"), name);
                item.insert(QStringLiteral("attributes"), attributesToJson(attrs));
                item.insert(QStringLiteral("missingAttributes"), toJsonArray(missing));
                lineFieldsPassed = lineFieldsPassed || missing.isEmpty();
                lines.append(item);
            } else if (numberedElementName(name, QStringLiteral("Arc"))) {
                ++arcCount;
                const QStringList required{
                    QStringLiteral("center_x"),
                    QStringLiteral("center_y"),
                    QStringLiteral("center_z"),
                    QStringLiteral("radius"),
                    QStringLiteral("start_angle"),
                    QStringLiteral("end_angle"),
                    QStringLiteral("ZValue"),
                };
                const QStringList missing = missingAttributes(attrs, required);
                QJsonObject item;
                item.insert(QStringLiteral("name"), name);
                item.insert(QStringLiteral("attributes"), attributesToJson(attrs));
                item.insert(QStringLiteral("missingAttributes"), toJsonArray(missing));
                arcFieldsPassed = arcFieldsPassed || missing.isEmpty();
                arcs.append(item);
            }
        } else if (reader.isEndElement() && inSectionLine) {
            --sectionDepth;
            if (sectionDepth == 0) {
                break;
            }
        }
    }

    const bool passed = lineCount >= 1 && arcCount >= 1 && lineFieldsPassed && arcFieldsPassed;
    result.insert(QStringLiteral("lineCount"), lineCount);
    result.insert(QStringLiteral("arcCount"), arcCount);
    result.insert(QStringLiteral("lineFieldsPassed"), lineFieldsPassed);
    result.insert(QStringLiteral("arcFieldsPassed"), arcFieldsPassed);
    result.insert(QStringLiteral("lines"), lines);
    result.insert(QStringLiteral("arcs"), arcs);
    result.insert(QStringLiteral("passed"), passed);
    return result;
}

QJsonObject executableProbe(const QString& executable)
{
    QJsonObject result;
    result.insert(QStringLiteral("name"), executable);
    const QString path = QStandardPaths::findExecutable(executable);
    result.insert(QStringLiteral("found"), !path.isEmpty());
    result.insert(QStringLiteral("path"), path);
    return result;
}

QJsonObject registryKeyProbe(const QString& key)
{
    QJsonObject result;
    result.insert(QStringLiteral("key"), key);
#ifdef Q_OS_WIN
    QSettings settings(key, QSettings::NativeFormat);
    const QStringList childGroups = settings.childGroups();
    const QStringList childKeys = settings.childKeys();
    result.insert(QStringLiteral("exists"), !childGroups.isEmpty() || !childKeys.isEmpty());
    result.insert(QStringLiteral("childGroupCount"), childGroups.size());
#else
    result.insert(QStringLiteral("exists"), false);
    result.insert(QStringLiteral("childGroupCount"), 0);
#endif
    return result;
}

QJsonObject autocadEnvironmentProbe()
{
    QJsonArray executables;
    executables.append(executableProbe(QStringLiteral("acad.exe")));
    executables.append(executableProbe(QStringLiteral("accoreconsole.exe")));

    QJsonArray registry;
    registry.append(registryKeyProbe(QStringLiteral("HKEY_LOCAL_MACHINE\\SOFTWARE\\Autodesk\\AutoCAD")));
    registry.append(registryKeyProbe(QStringLiteral("HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Autodesk\\AutoCAD")));
    registry.append(registryKeyProbe(QStringLiteral("HKEY_CURRENT_USER\\SOFTWARE\\Autodesk\\AutoCAD")));

    bool anyExecutable = false;
    for (const auto item : executables) {
        if (item.toObject().value(QStringLiteral("found")).toBool()) {
            anyExecutable = true;
        }
    }

    bool anyRegistry = false;
    for (const auto item : registry) {
        if (item.toObject().value(QStringLiteral("exists")).toBool()) {
            anyRegistry = true;
        }
    }

    QJsonObject result;
    result.insert(QStringLiteral("executables"), executables);
    result.insert(QStringLiteral("registry"), registry);
    result.insert(QStringLiteral("autocadExecutableFound"), anyExecutable);
    result.insert(QStringLiteral("autocadRegistryFound"), anyRegistry);
    result.insert(QStringLiteral("automaticL2Possible"), anyExecutable);
    result.insert(QStringLiteral("status"), anyExecutable ? QStringLiteral("available") : QStringLiteral("not_found"));
    return result;
}

QJsonObject pluginProbe(const QString& pluginDir)
{
    QJsonObject result;
    result.insert(QStringLiteral("pluginDir"), pluginDir);
    if (pluginDir.isEmpty()) {
        result.insert(QStringLiteral("status"), QStringLiteral("not_configured"));
        result.insert(QStringLiteral("allRequiredPresent"), false);
        return result;
    }

    const QDir dir(pluginDir);
    QJsonArray files;
    files.append(fileSummaryForPath(dir.filePath(QStringLiteral("FDrawingObj.dbx"))));
    files.append(fileSummaryForPath(dir.filePath(QStringLiteral("FDrawing.arx"))));
    files.append(fileSummaryForPath(dir.filePath(QStringLiteral("Detail.xml")), QStringLiteral("old-sample/Detail.xml")));
    files.append(fileSummaryForPath(dir.filePath(QStringLiteral("Detail01.stl")), QStringLiteral("old-sample/Detail01.stl")));

    bool allRequiredPresent = true;
    for (int index = 0; index < 2; ++index) {
        if (!files.at(index).toObject().value(QStringLiteral("exists")).toBool()) {
            allRequiredPresent = false;
        }
    }

    result.insert(QStringLiteral("status"), allRequiredPresent ? QStringLiteral("ready") : QStringLiteral("missing_required"));
    result.insert(QStringLiteral("allRequiredPresent"), allRequiredPresent);
    result.insert(QStringLiteral("files"), files);
    return result;
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
    const QCommandLineOption runIdOption(
        QStringLiteral("run-id"),
        QStringLiteral("DetailWriter run id."),
        QStringLiteral("id"),
        QStringLiteral("DW-L2-TODO036-001"));
    const QCommandLineOption fixtureOption(
        QStringLiteral("fixture"),
        QStringLiteral("Fixture to generate: complex-skeleton, point-face-edge, or section-line."),
        QStringLiteral("name"),
        QStringLiteral("complex-skeleton"));
    const QCommandLineOption pluginDirOption(
        QStringLiteral("plugin-dir"),
        QStringLiteral("Directory containing FDrawingObj.dbx and FDrawing.arx."),
        QStringLiteral("dir"));
    parser.addOption(outputOption);
    parser.addOption(viewsOption);
    parser.addOption(runIdOption);
    parser.addOption(fixtureOption);
    parser.addOption(pluginDirOption);
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
    options.runId = parser.value(runIdOption);
    const QString fixture = parser.value(fixtureOption);
    if (fixture != QStringLiteral("complex-skeleton") &&
        fixture != QStringLiteral("point-face-edge") &&
        fixture != QStringLiteral("section-line")) {
        QTextStream(stderr) << "fixture must be complex-skeleton, point-face-edge, or section-line\n";
        return EXIT_FAILURE;
    }

    const bool pointFaceEdgeFixture = fixture == QStringLiteral("point-face-edge");
    const bool sectionLineFixture = fixture == QStringLiteral("section-line");
    options.drawingName = pointFaceEdgeFixture
        ? QStringLiteral("todo037-point-face-edge")
        : (sectionLineFixture
               ? QStringLiteral("todo038-section-line")
               : QStringLiteral("todo036-complex-skeleton-l2"));
    options.modelFileName = pointFaceEdgeFixture
        ? QStringLiteral("todo037-model.step")
        : (sectionLineFixture
               ? QStringLiteral("todo038-model.step")
               : QStringLiteral("todo036-model.step"));
    options.drawingUnit = QStringLiteral("m");
    options.drawingScale = QStringLiteral("1");
    for (int index = 1; index <= viewCount; ++index) {
        tsrebar::DetailDrawingViewOptions view;
        const QString prefix = pointFaceEdgeFixture
            ? QStringLiteral("todo037")
            : (sectionLineFixture ? QStringLiteral("todo038") : QStringLiteral("todo036"));
        view.viewId = QStringLiteral("%1-view-%2").arg(prefix).arg(index, 3, 10, QLatin1Char('0'));
        view.drawingName = QStringLiteral("%1-view-%2").arg(prefix).arg(index);
        view.modelFileName = QStringLiteral("%1-model-%2.step").arg(prefix).arg(index);
        view.drawingUnit = QStringLiteral("mm");
        view.drawingScale = QStringLiteral("1:%1").arg(index);
        view.generalScale = QStringLiteral("%1-general-%2").arg(prefix).arg(index);
        if (sectionLineFixture) {
            view.sectionLines.push_back({
                6.00000000005,
                -1.0,
                -6.0,
                -1.0,
                QStringLiteral("-1.000000:-1.000000:12.000000")});
            view.sectionArcs.push_back({
                1.38015820994e-13,
                9.33333333333,
                0.5,
                6.66666666667,
                0.451026811805728,
                2.69056584178407,
                QStringLiteral("12.239266:9.333333:12.239266")});
        }
        options.views.push_back(view);
    }

    const tsrebar::DetailWriter writer;
    const tsrebar::DetailWriteResult result =
        writer.writePackage(outputDir,
                            pointFaceEdgeFixture ? makePointFaceEdgeProbeSteelData() : makeProbeSteelData(),
                            options);

    QJsonObject root;
    root.insert(QStringLiteral("schemaVersion"), QStringLiteral("detail-l2-fixture-probe/v1"));
    root.insert(QStringLiteral("runId"), options.runId);
    root.insert(QStringLiteral("fixture"), fixture);
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
                pointFaceEdgeFixture
                    ? QJsonArray{QStringLiteral("E-DETAIL-003"),
                                 QStringLiteral("E-DEV-056"),
                                 QStringLiteral("E-DEV-057"),
                                 QStringLiteral("E-DEV-058"),
                                 QStringLiteral("E-DEV-059")}
                    : sectionLineFixture
                    ? QJsonArray{QStringLiteral("E-DETAIL-003"),
                                 QStringLiteral("E-DEV-056"),
                                 QStringLiteral("E-DEV-057"),
                                 QStringLiteral("E-DEV-059"),
                                 QStringLiteral("E-DEV-060")}
                    : QJsonArray{QStringLiteral("E-DEV-055"),
                                 QStringLiteral("E-DETAIL-003"),
                                 QStringLiteral("E-DEV-057"),
                                 QStringLiteral("E-DEV-058")});
    root.insert(QStringLiteral("gaps"), QJsonArray{QStringLiteral("GAP-DRAW-001"), QStringLiteral("GAP-DRAW-002")});
    root.insert(QStringLiteral("autocadEnvironment"), autocadEnvironmentProbe());
    root.insert(QStringLiteral("fdrawingPlugin"), pluginProbe(parser.value(pluginDirOption)));

    const QString firstDrawing = QDir(outputDir).filePath(QStringLiteral("Detail01.stl"));
    const QJsonObject complexSkeleton = complexSkeletonProbe(firstDrawing);
    root.insert(QStringLiteral("complexSkeleton"), complexSkeleton);
    if (pointFaceEdgeFixture) {
        root.insert(QStringLiteral("pointFaceEdge"), pointFaceEdgeProbe(firstDrawing));
    }
    if (sectionLineFixture) {
        root.insert(QStringLiteral("sectionLine"), sectionLineProbe(firstDrawing));
    }

    QJsonArray files;
    for (const auto& fileName : result.files) {
        files.append(fileSummary(outputDir, fileName));
    }
    root.insert(QStringLiteral("files"), files);

    QTextStream(stdout) << QString::fromUtf8(QJsonDocument(root).toJson(QJsonDocument::Indented));
    if (!result.ok) {
        return 2;
    }
    if (pointFaceEdgeFixture) {
        return root.value(QStringLiteral("pointFaceEdge")).toObject().value(QStringLiteral("passed")).toBool()
            ? EXIT_SUCCESS
            : 4;
    }
    if (sectionLineFixture) {
        return root.value(QStringLiteral("sectionLine")).toObject().value(QStringLiteral("passed")).toBool()
            ? EXIT_SUCCESS
            : 5;
    }
    return complexSkeleton.value(QStringLiteral("passed")).toBool() ? EXIT_SUCCESS : 3;
}
