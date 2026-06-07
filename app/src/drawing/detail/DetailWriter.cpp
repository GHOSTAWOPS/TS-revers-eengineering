#include "drawing/detail/DetailWriter.h"

#include "domain/rebar/RebarScheduleService.h"

#include <QCryptographicHash>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QSaveFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include <algorithm>
#include <map>
#include <cmath>
#include <functional>
#include <set>
#include <stdexcept>

namespace tsrebar {
namespace {

constexpr auto kRequiredFieldMissing = "DW004_REQUIRED_FIELD_MISSING";
constexpr auto kXmlWriteFailed = "DW005_XML_WRITE_FAILED";
constexpr auto kXmlParseFailed = "DW006_XML_PARSE_FAILED";
constexpr auto kPackageValidationFailed = "DW007_PACKAGE_VALIDATION_FAILED";
constexpr auto kReplaceFailed = "DW008_REPLACE_FAILED";
constexpr auto kCrossReferenceFailed = "DW003_ID_CROSS_REFERENCE_FAILED";
constexpr auto kMaterialDeferred = "DW-WARN-MATERIAL_MASS_FORMULA_DEFERRED";

QString qstr(const std::string& value)
{
    return QString::fromStdString(value);
}

QString stableIdOrAlias(const std::string& primary, const std::string& alias)
{
    return qstr(primary.empty() ? alias : primary);
}

QString formatNumber(double value)
{
    if (std::abs(value) < 1.0e-12) {
        value = 0.0;
    }
    return QString::number(value, 'g', 15);
}

QString slashPath(const QString& path)
{
    return QDir::fromNativeSeparators(path);
}

QString directoryHash(const QString& path)
{
    QFileInfo info(path);
    if (!info.exists()) {
        return QStringLiteral("sha256:missing");
    }
    if (info.isFile()) {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly)) {
            return QStringLiteral("sha256:unreadable");
        }
        return QStringLiteral("sha256:") +
            QString::fromLatin1(QCryptographicHash::hash(file.readAll(),
                                                         QCryptographicHash::Sha256)
                                    .toHex());
    }

    QStringList files;
    QDirIterator iterator(path, QDir::Files, QDirIterator::Subdirectories);
    const QDir root(path);
    while (iterator.hasNext()) {
        const QString item = iterator.next();
        files.append(slashPath(root.relativeFilePath(item)));
    }
    std::sort(files.begin(), files.end());

    QCryptographicHash hash(QCryptographicHash::Sha256);
    for (const QString& rel : files) {
        QFile file(root.filePath(rel));
        if (!file.open(QIODevice::ReadOnly)) {
            return QStringLiteral("sha256:unreadable");
        }
        hash.addData(rel.toUtf8());
        hash.addData("\0", 1);
        hash.addData(file.readAll());
        hash.addData("\0", 1);
    }
    return QStringLiteral("sha256:") + QString::fromLatin1(hash.result().toHex());
}

bool removeDirIfExists(const QString& path)
{
    QFileInfo info(path);
    if (!info.exists()) {
        return true;
    }
    if (!info.isDir()) {
        return QFile::remove(path);
    }
    return QDir(path).removeRecursively();
}

void ensureParentDir(const QString& path)
{
    const QString parent = QFileInfo(path).absolutePath();
    if (!QDir().mkpath(parent)) {
        throw std::runtime_error(QStringLiteral("cannot create parent dir: %1")
                                     .arg(parent)
                                     .toStdString());
    }
}

void writeXmlFile(const QString& path, const std::function<void(QXmlStreamWriter&)>& write)
{
    ensureParentDir(path);
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        throw std::runtime_error(QStringLiteral("cannot write xml: %1")
                                     .arg(path)
                                     .toStdString());
    }
    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.writeStartDocument(QStringLiteral("1.0"));
    write(writer);
    writer.writeEndDocument();
    if (!file.commit()) {
        throw std::runtime_error(QStringLiteral("cannot commit xml: %1")
                                     .arg(path)
                                     .toStdString());
    }
}

std::map<QString, const SteelBar*> barsById(const SteelData& steelData)
{
    std::map<QString, const SteelBar*> result;
    for (const SteelBar& bar : steelData.bars) {
        result.emplace(stableIdOrAlias(bar.barId, bar.id), &bar);
    }
    return result;
}

std::map<QString, const SteelBarSegment*> segmentsById(const SteelData& steelData)
{
    std::map<QString, const SteelBarSegment*> result;
    for (const SteelBarSegment& segment : steelData.segments) {
        result.emplace(stableIdOrAlias(segment.segmentId, segment.id), &segment);
    }
    return result;
}

std::vector<const SteelBar*> groupBars(const SteelBarGroup& group,
                                       const std::map<QString, const SteelBar*>& byId)
{
    std::vector<const SteelBar*> result;
    for (const std::string& barId : group.barIds) {
        const auto it = byId.find(qstr(barId));
        if (it != byId.end()) {
            result.push_back(it->second);
        }
    }
    return result;
}

std::vector<QString> missingGroupBarIds(const SteelBarGroup& group,
                                        const std::map<QString, const SteelBar*>& byId)
{
    std::vector<QString> result;
    for (const std::string& barId : group.barIds) {
        const QString id = qstr(barId);
        if (byId.find(id) == byId.end()) {
            result.push_back(id);
        }
    }
    return result;
}

std::vector<const SteelBarSegment*> barSegments(
    const SteelBar& bar,
    const std::map<QString, const SteelBarSegment*>& byId)
{
    std::vector<const SteelBarSegment*> result;
    for (const std::string& segmentId : bar.segmentIds) {
        const auto it = byId.find(qstr(segmentId));
        if (it != byId.end()) {
            result.push_back(it->second);
        }
    }
    return result;
}

std::vector<QString> missingBarSegmentIds(const SteelBar& bar,
                                          const std::map<QString, const SteelBarSegment*>& byId)
{
    std::vector<QString> result;
    for (const std::string& segmentId : bar.segmentIds) {
        const QString id = qstr(segmentId);
        if (byId.find(id) == byId.end()) {
            result.push_back(id);
        }
    }
    return result;
}

int segmentCountFor(const SteelBarGroup& group,
                    const std::vector<const SteelBar*>& bars,
                    const std::map<QString, const SteelBarSegment*>& segmentIndex)
{
    if (group.segmentCount > 0) {
        return group.segmentCount;
    }
    if (bars.empty()) {
        return 0;
    }
    return static_cast<int>(barSegments(*bars.front(), segmentIndex).size());
}

int barCountFor(const SteelBarGroup& group, int actualBarRefs)
{
    return group.barCount > 0 ? group.barCount : actualBarRefs;
}

const SteelBarSegment* findSegmentById(
    const std::map<QString, const SteelBarSegment*>& segmentIndex,
    const std::string& segmentId)
{
    const auto it = segmentIndex.find(qstr(segmentId));
    return it == segmentIndex.end() ? nullptr : it->second;
}

QString detailShapeCode(const SteelBarSegment& segment)
{
    return qstr(segment.detailShapeTypeCode());
}

void appendDiagnostic(DetailWriteResult& result,
                      const QString& code,
                      const QString& file,
                      const QString& message)
{
    result.diagnostics.push_back({code, file, message});
    if (!result.errorCodes.contains(code)) {
        result.errorCodes.append(code);
    }
}

bool numberedNodeName(const QString& name, const QString& prefix)
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

QString detailDrawingFileName(int oneBasedIndex)
{
    return oneBasedIndex < 10
        ? QStringLiteral("Detail0%1.stl").arg(oneBasedIndex)
        : QStringLiteral("Detail%1.stl").arg(oneBasedIndex);
}

DetailDrawingViewOptions fallbackViewOptions(const DetailWriteOptions& options,
                                             int oneBasedIndex)
{
    DetailDrawingViewOptions view;
    view.viewId = QStringLiteral("view_%1").arg(oneBasedIndex, 6, 10, QLatin1Char('0'));
    view.drawingName = options.drawingName;
    view.modelFileName = options.modelFileName;
    view.drawingUnit = options.drawingUnit;
    view.drawingScale = options.drawingScale;
    view.generalScale = options.drawingScale;
    return view;
}

QVector<DetailDrawingViewOptions> effectiveViews(const DetailWriteOptions& options)
{
    if (options.views.empty()) {
        return {fallbackViewOptions(options, 1)};
    }

    QVector<DetailDrawingViewOptions> result;
    result.reserve(options.views.size());
    for (int index = 0; index < options.views.size(); ++index) {
        DetailDrawingViewOptions view = options.views.at(index);
        const DetailDrawingViewOptions fallback = fallbackViewOptions(options, index + 1);
        if (view.viewId.isEmpty()) {
            view.viewId = fallback.viewId;
        }
        if (view.drawingName.isEmpty()) {
            view.drawingName = fallback.drawingName;
        }
        if (view.modelFileName.isEmpty()) {
            view.modelFileName = fallback.modelFileName;
        }
        if (view.drawingUnit.isEmpty()) {
            view.drawingUnit = fallback.drawingUnit;
        }
        if (view.drawingScale.isEmpty()) {
            view.drawingScale = fallback.drawingScale;
        }
        if (view.generalScale.isEmpty()) {
            view.generalScale = view.drawingScale;
        }
        result.push_back(view);
    }
    return result;
}

QStringList detailPackageFilesForViews(int viewCount)
{
    QStringList files{QStringLiteral("Detail.xml")};
    for (int index = 1; index <= viewCount; ++index) {
        files.append(detailDrawingFileName(index));
    }
    return files;
}

DetailWriteResult validateInput(const SteelData& steelData)
{
    DetailWriteResult result;
    const auto barIndex = barsById(steelData);
    const auto segmentIndex = segmentsById(steelData);
    if (steelData.groups.empty()) {
        appendDiagnostic(result,
                         QString::fromLatin1(kRequiredFieldMissing),
                         QStringLiteral("Detail01.stl"),
                         QStringLiteral("SteelData.groups is empty"));
        return result;
    }

    for (const SteelBarGroup& group : steelData.groups) {
        const QString groupId = stableIdOrAlias(group.groupId, group.id);
        if (groupId.isEmpty()) {
            appendDiagnostic(result,
                             QString::fromLatin1(kRequiredFieldMissing),
                             QStringLiteral("Detail01.stl"),
                             QStringLiteral("StbGroup.groupID is required"));
        }
        if (group.rsdId.empty()) {
            appendDiagnostic(result,
                             QString::fromLatin1(kRequiredFieldMissing),
                             QStringLiteral("Detail01.stl"),
                             QStringLiteral("StbGroup.rsdID is required"));
        }
        if (group.diameter <= 0.0) {
            appendDiagnostic(result,
                             QString::fromLatin1(kRequiredFieldMissing),
                             QStringLiteral("Detail01.stl"),
                             QStringLiteral("StbGroup.diameter is required"));
        }

        const std::vector<const SteelBar*> bars = groupBars(group, barIndex);
        for (const QString& missingBarId : missingGroupBarIds(group, barIndex)) {
            appendDiagnostic(result,
                             QString::fromLatin1(kCrossReferenceFailed),
                             QStringLiteral("Detail01.stl"),
                             QStringLiteral("StbGroup references missing SteelBar %1")
                                 .arg(missingBarId));
        }
        if (bars.empty()) {
            appendDiagnostic(result,
                             QString::fromLatin1(kRequiredFieldMissing),
                             QStringLiteral("Detail01.stl"),
                             QStringLiteral("StbGroup must reference at least one SteelBar"));
            continue;
        }

        for (const SteelBar* bar : bars) {
            for (const QString& missingSegmentId : missingBarSegmentIds(*bar, segmentIndex)) {
                appendDiagnostic(result,
                                 QString::fromLatin1(kCrossReferenceFailed),
                                 QStringLiteral("Detail01.stl"),
                                 QStringLiteral("SteelBar references missing segment %1")
                                     .arg(missingSegmentId));
            }
            const std::vector<const SteelBarSegment*> segments = barSegments(*bar, segmentIndex);
            if (segments.empty()) {
                appendDiagnostic(result,
                                 QString::fromLatin1(kRequiredFieldMissing),
                                 QStringLiteral("Detail01.stl"),
                                 QStringLiteral("SteelBar must reference at least one segment"));
            }
            for (const SteelBarSegment* segment : segments) {
                const QString segmentId = stableIdOrAlias(segment->segmentId, segment->id);
                if (segmentId.isEmpty()) {
                    appendDiagnostic(result,
                                     QString::fromLatin1(kRequiredFieldMissing),
                                     QStringLiteral("Detail01.stl"),
                                     QStringLiteral("StbGeo.segID is required"));
                }
                if (detailShapeCode(*segment).isEmpty()) {
                    appendDiagnostic(result,
                                     QString::fromLatin1(kRequiredFieldMissing),
                                     QStringLiteral("Detail01.stl"),
                                     QStringLiteral("StbGeo.shapeType is required"));
                }
            }
        }
    }
    return result;
}

void writeStyleXml(const QString& path, const SteelData& steelData)
{
    writeXmlFile(path, [&steelData](QXmlStreamWriter& writer) {
        writer.writeStartElement(QStringLiteral("StyleRoot"));
        writer.writeAttribute(QStringLiteral("CurrPos"), QStringLiteral("1"));
        writer.writeStartElement(QStringLiteral("Styles"));
        writer.writeStartElement(QStringLiteral("Style1"));
        writer.writeAttribute(QStringLiteral("Name"),
                              steelData.gradeName.empty()
                                  ? QStringLiteral("default")
                                  : qstr(steelData.gradeName));
        const double diameter = steelData.diameterSet.empty()
            ? (!steelData.groups.empty() ? steelData.groups.front().diameter : 0.0)
            : steelData.diameterSet.front();
        writer.writeAttribute(QStringLiteral("dia"), formatNumber(diameter));
        writer.writeAttribute(QStringLiteral("type"),
                              steelData.level.empty() ? QStringLiteral("HRB") : qstr(steelData.level));
        writer.writeAttribute(QStringLiteral("source"), QStringLiteral("E-DETAIL-001"));
        writer.writeEndElement();
        writer.writeEndElement();
        writer.writeEndElement();
    });
}

void writePointAttributes(QXmlStreamWriter& writer,
                          const QString& prefix,
                          const DomainPoint3d& point)
{
    writer.writeAttribute(prefix + QStringLiteral("_x"), formatNumber(point.x));
    writer.writeAttribute(prefix + QStringLiteral("_y"), formatNumber(point.y));
    writer.writeAttribute(prefix + QStringLiteral("_z"), formatNumber(point.z));
}

void writeZeroSecondaryOffsetAttributes(QXmlStreamWriter& writer)
{
    writer.writeAttribute(QStringLiteral("offset_x2"), QStringLiteral("0"));
    writer.writeAttribute(QStringLiteral("offset_y2"), QStringLiteral("0"));
    writer.writeAttribute(QStringLiteral("offset_z2"), QStringLiteral("0"));
}

void writeSegmentGeo(QXmlStreamWriter& writer,
                     const SteelBarSegment& segment,
                     int sequence)
{
    writer.writeStartElement(QStringLiteral("StbGeo%1").arg(sequence));
    writer.writeAttribute(QStringLiteral("segID"), stableIdOrAlias(segment.segmentId, segment.id));
    writer.writeAttribute(QStringLiteral("stbSeqNum"),
                          QString::number(segment.sequenceNo > 0 ? segment.sequenceNo : sequence));
    writer.writeAttribute(QStringLiteral("shapeType"), detailShapeCode(segment));
    if (segment.shapeType == SteelBarSegmentShape::Point) {
        // P0 field skeleton: the existing domain point is mapped to old pointStb fields.
        // Full point-bar generation and secondary offset semantics remain evidence gaps.
        writePointAttributes(writer, QStringLiteral("point"), segment.startPoint);
        writePointAttributes(writer, QStringLiteral("offset"), segment.offset);
        writeZeroSecondaryOffsetAttributes(writer);
    } else {
        writePointAttributes(writer, QStringLiteral("start"), segment.startPoint);
        writePointAttributes(writer, QStringLiteral("middle"), segment.middlePoint);
        writePointAttributes(writer, QStringLiteral("end"), segment.endPoint);
        writer.writeAttribute(QStringLiteral("start_r"), formatNumber(segment.startRadius));
        writer.writeAttribute(QStringLiteral("end_r"), formatNumber(segment.endRadius));
        writePointAttributes(writer, QStringLiteral("offset"), segment.offset);
    }
    writer.writeAttribute(QStringLiteral("length"), formatNumber(segment.length));
    writer.writeEndElement();
}

void writeFaceEdge(QXmlStreamWriter& writer, const RebarFaceEdgeGeometry& faceEdge)
{
    const QString shapeType = qstr(faceEdge.detailShapeTypeCode());
    if (shapeType.isEmpty()) {
        return;
    }

    writer.writeEmptyElement(QStringLiteral("FaceEdge"));
    writer.writeAttribute(QStringLiteral("shapeType"), shapeType);
    if (faceEdge.shapeType == RebarFaceEdgeShape::Arc) {
        writer.writeAttribute(QStringLiteral("m_ArcDotReverse"),
                              faceEdge.arcDotReverse ? QStringLiteral("T") : QStringLiteral("F"));
        writer.writeAttribute(QStringLiteral("start_x"), formatNumber(faceEdge.startPoint.x));
        writer.writeAttribute(QStringLiteral("start_y"), formatNumber(faceEdge.startPoint.y));
        writer.writeAttribute(QStringLiteral("middle_x"), formatNumber(faceEdge.middlePoint.x));
        writer.writeAttribute(QStringLiteral("middle_y"), formatNumber(faceEdge.middlePoint.y));
        writer.writeAttribute(QStringLiteral("end_x"), formatNumber(faceEdge.endPoint.x));
        writer.writeAttribute(QStringLiteral("end_y"), formatNumber(faceEdge.endPoint.y));
    } else {
        writer.writeAttribute(QStringLiteral("start_x"), formatNumber(faceEdge.startPoint.x));
        writer.writeAttribute(QStringLiteral("start_y"), formatNumber(faceEdge.startPoint.y));
        writer.writeAttribute(QStringLiteral("end_x"), formatNumber(faceEdge.endPoint.x));
        writer.writeAttribute(QStringLiteral("end_y"), formatNumber(faceEdge.endPoint.y));
    }
}

void writeScheduleSegment(QXmlStreamWriter& writer,
                          const SteelBarSegment& segment,
                          int sequence)
{
    writer.writeStartElement(QStringLiteral("StbSeg%1").arg(sequence));
    writer.writeAttribute(QStringLiteral("lenRange"), formatNumber(segment.length));
    writer.writeAttribute(QStringLiteral("deltaLen"), QStringLiteral("0"));
    writer.writeAttribute(QStringLiteral("shapeType"), detailShapeCode(segment));
    if (segment.shapeType == SteelBarSegmentShape::Arc) {
        writer.writeEmptyElement(QStringLiteral("Arc"));
        writer.writeAttribute(QStringLiteral("start_x"), formatNumber(segment.startPoint.x));
        writer.writeAttribute(QStringLiteral("start_y"), formatNumber(segment.startPoint.y));
        writer.writeAttribute(QStringLiteral("middle_x"), formatNumber(segment.middlePoint.x));
        writer.writeAttribute(QStringLiteral("middle_y"), formatNumber(segment.middlePoint.y));
        writer.writeAttribute(QStringLiteral("end_x"), formatNumber(segment.endPoint.x));
        writer.writeAttribute(QStringLiteral("end_y"), formatNumber(segment.endPoint.y));
        writer.writeAttribute(QStringLiteral("start_rad"), formatNumber(segment.startRadius));
        writer.writeAttribute(QStringLiteral("end_rad"), formatNumber(segment.endRadius));
    } else {
        writer.writeEmptyElement(QStringLiteral("Line"));
        writer.writeAttribute(QStringLiteral("start_x"), formatNumber(segment.startPoint.x));
        writer.writeAttribute(QStringLiteral("start_y"), formatNumber(segment.startPoint.y));
        writer.writeAttribute(QStringLiteral("end_x"), formatNumber(segment.endPoint.x));
        writer.writeAttribute(QStringLiteral("end_y"), formatNumber(segment.endPoint.y));
    }
    writer.writeEndElement();
}

void writeLegacyGeneralInfo(QXmlStreamWriter& writer, const DetailDrawingViewOptions& view)
{
    writer.writeStartElement(QStringLiteral("General-Info"));
    writer.writeAttribute(QStringLiteral("CompanyName"), QStringLiteral("design-company"));
    writer.writeAttribute(QStringLiteral("ExportYesNo"), QStringLiteral("T"));
    writer.writeAttribute(QStringLiteral("ExpSteelYesNo"), QStringLiteral("T"));
    writer.writeAttribute(QStringLiteral("ExpSteelMark"), QStringLiteral("T"));
    writer.writeAttribute(QStringLiteral("DimensionChicunB"), QStringLiteral("T"));
    writer.writeAttribute(QStringLiteral("DimensionChicunT"), QStringLiteral("F"));
    writer.writeAttribute(QStringLiteral("DimensionChicunL"), QStringLiteral("T"));
    writer.writeAttribute(QStringLiteral("DimensionChicunR"), QStringLiteral("F"));
    writer.writeAttribute(QStringLiteral("DimensionPointBarB"), QStringLiteral("F"));
    writer.writeAttribute(QStringLiteral("DimensionPointBarT"), QStringLiteral("F"));
    writer.writeAttribute(QStringLiteral("DimensionPointBarL"), QStringLiteral("F"));
    writer.writeAttribute(QStringLiteral("DimensionPointBarR"), QStringLiteral("F"));
    writer.writeAttribute(QStringLiteral("DimensionLineBarB"), QStringLiteral("F"));
    writer.writeAttribute(QStringLiteral("DimensionLineBarT"), QStringLiteral("F"));
    writer.writeAttribute(QStringLiteral("DimensionLineBarL"), QStringLiteral("F"));
    writer.writeAttribute(QStringLiteral("DimensionLineBarR"), QStringLiteral("F"));
    writer.writeAttribute(QStringLiteral("DimensionLLineBarB"), QStringLiteral("F"));
    writer.writeAttribute(QStringLiteral("DimensionLLineBarT"), QStringLiteral("F"));
    writer.writeAttribute(QStringLiteral("DimensionLLineBarL"), QStringLiteral("F"));
    writer.writeAttribute(QStringLiteral("DimensionLLineBarR"), QStringLiteral("F"));
    writer.writeAttribute(QStringLiteral("DimensionBDist"), QStringLiteral("15"));
    writer.writeAttribute(QStringLiteral("DimensionTDist"), QStringLiteral("15"));
    writer.writeAttribute(QStringLiteral("DimensionLDist"), QStringLiteral("15"));
    writer.writeAttribute(QStringLiteral("DimensionRDist"), QStringLiteral("15"));
    writer.writeAttribute(QStringLiteral("Detail"), QString());
    writer.writeAttribute(QStringLiteral("Model_FileName"), view.modelFileName);
    writer.writeAttribute(QStringLiteral("DispCuttedSymb"), QStringLiteral("T"));
    writer.writeAttribute(QStringLiteral("HalfViewH"), QStringLiteral("F"));
    writer.writeAttribute(QStringLiteral("HalfViewW"), QStringLiteral("F"));
    writer.writeAttribute(QStringLiteral("BasePoint_X"), QStringLiteral("0"));
    writer.writeAttribute(QStringLiteral("BasePoint_Y"), QStringLiteral("0"));
    writer.writeAttribute(QStringLiteral("Range_Min_X"), QStringLiteral("0"));
    writer.writeAttribute(QStringLiteral("Range_Max_X"), QStringLiteral("0"));
    writer.writeAttribute(QStringLiteral("Range_Min_Y"), QStringLiteral("0"));
    writer.writeAttribute(QStringLiteral("Range_Max_Y"), QStringLiteral("0"));
    writer.writeAttribute(QStringLiteral("Range_XMLMin_X"), QStringLiteral("0"));
    writer.writeAttribute(QStringLiteral("Range_XMLMax_X"), QStringLiteral("0"));
    writer.writeAttribute(QStringLiteral("Range_XMLMin_Y"), QStringLiteral("0"));
    writer.writeAttribute(QStringLiteral("Range_XMLMax_Y"), QStringLiteral("0"));
    writer.writeAttribute(QStringLiteral("CutPlaneDirX0"), QStringLiteral("0"));
    writer.writeAttribute(QStringLiteral("CutPlaneDirY0"), QStringLiteral("-1"));
    writer.writeAttribute(QStringLiteral("CutPlaneDirZ0"), QStringLiteral("0"));
    writer.writeAttribute(QStringLiteral("CutPlaneDirX"), QStringLiteral("0"));
    writer.writeAttribute(QStringLiteral("CutPlaneDirY"), QStringLiteral("-1"));
    writer.writeAttribute(QStringLiteral("CutPlaneDirZ"), QStringLiteral("0"));
    writer.writeAttribute(QStringLiteral("TopDirX"), QStringLiteral("0"));
    writer.writeAttribute(QStringLiteral("TopDirY"), QStringLiteral("0"));
    writer.writeAttribute(QStringLiteral("TopDirZ"), QStringLiteral("1"));
    writer.writeAttribute(QStringLiteral("DrawingName"), view.drawingName);
    writer.writeAttribute(QStringLiteral("DrawingUnit"), view.drawingUnit);
    writer.writeAttribute(QStringLiteral("DrawingScale"), view.drawingScale);
    writer.writeAttribute(QStringLiteral("GeneralScale"), view.generalScale);
    writer.writeAttribute(QStringLiteral("DrawingType"), QStringLiteral("0"));
    writer.writeAttribute(QStringLiteral("LevelDrawing"), QStringLiteral("0"));
    writer.writeAttribute(QStringLiteral("CutPlanePosX"), QStringLiteral("0"));
    writer.writeAttribute(QStringLiteral("CutPlanePosY"), QStringLiteral("0"));
    writer.writeAttribute(QStringLiteral("CutPlanePosZ"), QStringLiteral("0"));
    writer.writeAttribute(QStringLiteral("DrawTaoTong"), QStringLiteral("F"));
    writer.writeEndElement();
}

void writeLegacyLineContainer(QXmlStreamWriter& writer,
                              const QVector<DetailLegacyLineGeometry>* lines = nullptr)
{
    writer.writeStartElement(QStringLiteral("lines"));
    if (lines) {
        int sequence = 0;
        for (const DetailLegacyLineGeometry& line : *lines) {
            writer.writeEmptyElement(QStringLiteral("Line%1").arg(++sequence));
            writer.writeAttribute(QStringLiteral("start_x"), formatNumber(line.startX));
            writer.writeAttribute(QStringLiteral("start_y"), formatNumber(line.startY));
            writer.writeAttribute(QStringLiteral("end_x"), formatNumber(line.endX));
            writer.writeAttribute(QStringLiteral("end_y"), formatNumber(line.endY));
            writer.writeAttribute(QStringLiteral("ZValue"), line.zValue);
        }
    }
    writer.writeEndElement();
}

void writeLegacyArcContainer(QXmlStreamWriter& writer,
                             const QVector<DetailSectionArcGeometry>* sectionArcs = nullptr)
{
    writer.writeStartElement(QStringLiteral("Arcs"));
    if (sectionArcs) {
        int sequence = 0;
        for (const DetailSectionArcGeometry& arc : *sectionArcs) {
            writer.writeEmptyElement(QStringLiteral("Arc%1").arg(++sequence));
            writer.writeAttribute(QStringLiteral("center_x"), formatNumber(arc.centerX));
            writer.writeAttribute(QStringLiteral("center_y"), formatNumber(arc.centerY));
            writer.writeAttribute(QStringLiteral("center_z"), formatNumber(arc.centerZ));
            writer.writeAttribute(QStringLiteral("radius"), formatNumber(arc.radius));
            writer.writeAttribute(QStringLiteral("start_angle"), formatNumber(arc.startAngle));
            writer.writeAttribute(QStringLiteral("end_angle"), formatNumber(arc.endAngle));
            writer.writeAttribute(QStringLiteral("ZValue"), arc.zValue);
        }
    }
    writer.writeEndElement();
}

void writeLegacyCurveGeometryContainers(QXmlStreamWriter& writer,
                                        const QVector<DetailLegacyLineGeometry>* lines = nullptr)
{
    writeLegacyLineContainer(writer, lines);
    writer.writeEmptyElement(QStringLiteral("circles"));
    writeLegacyArcContainer(writer);
    writer.writeEmptyElement(QStringLiteral("Ellipses"));
    writer.writeEmptyElement(QStringLiteral("EllipseArcs"));
    writer.writeEmptyElement(QStringLiteral("Splines"));
}

void writeSectionLineGeometryContainers(QXmlStreamWriter& writer, const DetailDrawingViewOptions& view)
{
    writeLegacyLineContainer(writer, &view.sectionLines);
    writer.writeEmptyElement(QStringLiteral("circles"));
    writeLegacyArcContainer(writer, &view.sectionArcs);
    writer.writeEmptyElement(QStringLiteral("Ellipses"));
    writer.writeEmptyElement(QStringLiteral("EllipseArcs"));
    writer.writeEmptyElement(QStringLiteral("Splines"));
}

void writePartDetailDrawingSkeleton(QXmlStreamWriter& writer, const DetailDrawingViewOptions& view)
{
    writer.writeStartElement(QStringLiteral("PartDetailDrawing"));
    writer.writeAttribute(QStringLiteral("num"), QStringLiteral("8"));
    writeLegacyGeneralInfo(writer, view);

    writer.writeStartElement(QStringLiteral("continue-line"));
    writeLegacyCurveGeometryContainers(writer, &view.continueLines);
    writer.writeEndElement();

    writer.writeStartElement(QStringLiteral("hidden-line"));
    writeLegacyCurveGeometryContainers(writer, &view.hiddenLines);
    writer.writeEndElement();

    writer.writeStartElement(QStringLiteral("central-line"));
    writeLegacyLineContainer(writer, &view.centralLines);
    writer.writeEndElement();

    writer.writeStartElement(QStringLiteral("section-line"));
    writeSectionLineGeometryContainers(writer, view);
    writer.writeEndElement();

    writer.writeStartElement(QStringLiteral("hatch-line"));
    writeLegacyLineContainer(writer, &view.hatchLines);
    writer.writeEndElement();

    writer.writeEmptyElement(QStringLiteral("Others"));

    writer.writeStartElement(QStringLiteral("steeljoint-line"));
    writer.writeEmptyElement(QStringLiteral("joints"));
    writer.writeEndElement();

    writer.writeEndElement();
}

void writeDrawingXml(const QString& path,
                     const SteelData& steelData,
                     const DetailDrawingViewOptions& view)
{
    const auto barIndex = barsById(steelData);
    const auto segmentIndex = segmentsById(steelData);
    const RebarSchedule schedule = RebarScheduleService{}.buildSchedule(steelData);

    writeXmlFile(path, [&](QXmlStreamWriter& writer) {
        writer.writeStartElement(QStringLiteral("DrawingRoot"));

        writer.writeStartElement(QStringLiteral("StbTables"));
        writer.writeStartElement(QStringLiteral("StbTable"));
        writer.writeAttribute(QStringLiteral("count"), QString::number(schedule.scheduleRows.size()));

        int rowSequence = 0;
        for (const RebarScheduleRow& row : schedule.scheduleRows) {
            writer.writeStartElement(QStringLiteral("StbRow%1").arg(++rowSequence));
            writer.writeAttribute(QStringLiteral("rsdID"), qstr(row.rsdId));
            writer.writeAttribute(QStringLiteral("ComponentName"), qstr(row.componentName));
            writer.writeAttribute(QStringLiteral("SteelWay"), qstr(row.steelWay));
            writer.writeAttribute(QStringLiteral("diameter"), formatNumber(row.diameter));
            writer.writeAttribute(QStringLiteral("length"), formatNumber(row.length));
            writer.writeAttribute(QStringLiteral("segNum"), QString::number(row.segmentCount));
            writer.writeAttribute(QStringLiteral("sameGrpNum"), QString::number(row.sameGroupCount));
            writer.writeAttribute(QStringLiteral("stbNumSum"), QString::number(row.barNumberSum));
            writer.writeAttribute(QStringLiteral("lenSum"), formatNumber(row.lengthSum));
            writer.writeAttribute(QStringLiteral("stbLevel"), qstr(row.steelLevel));
            writer.writeAttribute(QStringLiteral("stbLayer"), qstr(row.layer));
            writer.writeAttribute(QStringLiteral("stbProfile"), qstr(row.profile));
            writer.writeAttribute(QStringLiteral("stbUse"), qstr(row.use));
            int scheduleSegSequence = 0;
            for (const RebarScheduleSegmentRef& segmentRef : row.segments) {
                const SteelBarSegment* segment = findSegmentById(segmentIndex, segmentRef.segmentId);
                if (segment) {
                    writeScheduleSegment(writer, *segment, ++scheduleSegSequence);
                }
            }
            writer.writeEndElement();
        }
        writer.writeEndElement();

        writer.writeStartElement(QStringLiteral("MaterialTable"));
        writer.writeAttribute(QStringLiteral("rowCount"), QString::number(schedule.materialRows.size()));
        writer.writeAttribute(QStringLiteral("Mass"), formatNumber(schedule.totalMass));
        writer.writeAttribute(QStringLiteral("Volume722"), formatNumber(schedule.volume722));
        writer.writeAttribute(QStringLiteral("MassNum"), QString::number(schedule.massNum));
        int materialSequence = 0;
        for (const RebarMaterialRow& material : schedule.materialRows) {
            writer.writeStartElement(QStringLiteral("MatRow%1").arg(++materialSequence));
            writer.writeAttribute(QStringLiteral("diameter"), formatNumber(material.diameter));
            writer.writeAttribute(QStringLiteral("lenSum"), formatNumber(material.lengthSum));
            writer.writeAttribute(QStringLiteral("countSum"), QString::number(material.countSum));
            writer.writeAttribute(QStringLiteral("singleMass"), formatNumber(material.singleMass));
            writer.writeAttribute(QStringLiteral("massSum"), formatNumber(material.massSum));
            writer.writeAttribute(QStringLiteral("stbLevel"), qstr(material.steelLevel));
            writer.writeEndElement();
        }
        writer.writeEndElement();
        writer.writeEndElement();

        writer.writeStartElement(QStringLiteral("HViewPorts"));
        writer.writeStartElement(QStringLiteral("ViewPort"));
        writer.writeAttribute(QStringLiteral("id"), view.viewId);
        writePartDetailDrawingSkeleton(writer, view);

        writer.writeStartElement(QStringLiteral("StbDetailDrawing"));
        writer.writeStartElement(QStringLiteral("StbGroups"));
        writer.writeAttribute(QStringLiteral("stbGroupCount"), QString::number(steelData.groups.size()));

        int groupSequence = 0;
        for (const SteelBarGroup& group : steelData.groups) {
            const std::vector<const SteelBar*> bars = groupBars(group, barIndex);
            const std::vector<const SteelBarSegment*> segments = barSegments(*bars.front(), segmentIndex);
            const int barCount = barCountFor(group, static_cast<int>(bars.size()));
            const int segmentCount = segmentCountFor(group, bars, segmentIndex);

            writer.writeStartElement(QStringLiteral("StbGroup%1").arg(++groupSequence));
            writer.writeAttribute(QStringLiteral("rsdID"), qstr(group.rsdId));
            writer.writeAttribute(QStringLiteral("groupID"), stableIdOrAlias(group.groupId, group.id));
            writer.writeAttribute(QStringLiteral("diameter"), formatNumber(group.diameter));
            writer.writeAttribute(QStringLiteral("diameter2"), formatNumber(group.secondaryDiameter));
            writer.writeAttribute(QStringLiteral("interval"), formatNumber(group.interval));
            writer.writeAttribute(QStringLiteral("barcount"), QString::number(barCount));
            writer.writeAttribute(QStringLiteral("segcount"), QString::number(segmentCount));
            writer.writeAttribute(QStringLiteral("stbNum"), qstr(group.displayNumber));
            writer.writeAttribute(QStringLiteral("stbNumAct"), qstr(group.actualNumber));
            writer.writeAttribute(QStringLiteral("stbLevel"), qstr(group.steelLevel));
            writer.writeAttribute(QStringLiteral("stbLayer"), qstr(group.layer));
            writer.writeAttribute(QStringLiteral("stbProfile"), qstr(group.profile));
            writer.writeAttribute(QStringLiteral("stbUse"), qstr(group.use));
            writer.writeAttribute(QStringLiteral("RangeLess180"), group.rangeLess180 ? QStringLiteral("T") : QStringLiteral("F"));
            writer.writeAttribute(QStringLiteral("ComponentName"), qstr(group.componentName));
            writer.writeAttribute(QStringLiteral("PJSteelName"), qstr(group.projectSteelName));
            writer.writeAttribute(QStringLiteral("SteelWay"), qstr(group.steelWay));
            writer.writeAttribute(QStringLiteral("stbType"), qstr(group.rebarType));
            writer.writeAttribute(QStringLiteral("stbOffsetInOut"), formatNumber(group.offsetInOut));

            writer.writeStartElement(QStringLiteral("Std1"));
            writer.writeAttribute(QStringLiteral("segCount"), QString::number(segments.size()));
            int segmentSequence = 0;
            for (const SteelBarSegment* segment : segments) {
                writeSegmentGeo(writer, *segment, ++segmentSequence);
            }
            writer.writeEndElement();
            if (group.rebarType == "pointStb") {
                for (const RebarFaceEdgeGeometry& faceEdge : group.faceEdges) {
                    writeFaceEdge(writer, faceEdge);
                }
            }
            writer.writeEndElement();
        }

        writer.writeEndElement();
        writer.writeEndElement();
        writer.writeEndElement();
        writer.writeEndElement();

        writer.writeEndElement();
    });
}

QString firstRootName(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return {};
    }
    QXmlStreamReader reader(&file);
    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.isStartElement()) {
            return reader.name().toString();
        }
    }
    return {};
}

void validateL0(const QString& dir, const QStringList& packageFiles, DetailWriteResult& result)
{
    for (const QString& fileName : packageFiles) {
        const QString expectedRoot = fileName == QStringLiteral("Detail.xml")
            ? QStringLiteral("StyleRoot")
            : QStringLiteral("DrawingRoot");
        const QString path = QDir(dir).filePath(fileName);
        if (!QFileInfo::exists(path)) {
            appendDiagnostic(result,
                             QString::fromLatin1(kRequiredFieldMissing),
                             fileName,
                             QStringLiteral("required Detail file missing"));
            continue;
        }
        const QString root = firstRootName(path);
        if (root != expectedRoot) {
            appendDiagnostic(result,
                             QString::fromLatin1(kXmlParseFailed),
                             fileName,
                             QStringLiteral("root is %1, expected %2")
                                 .arg(root, expectedRoot));
        }
    }
}

void validateL1File(const QString& dir, const QString& fileName, DetailWriteResult& result)
{
    QFile file(QDir(dir).filePath(fileName));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        appendDiagnostic(result,
                         QString::fromLatin1(kXmlParseFailed),
                         fileName,
                         QStringLiteral("cannot open generated %1").arg(fileName));
        return;
    }

    QXmlStreamReader reader(&file);
    std::set<QString> groupRsdIds;
    std::set<QString> rowRsdIds;
    std::set<QString> segmentIds;
    std::map<QString, int> stdExpected;
    std::map<QString, int> stdActual;
    QString currentStd;

    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.isStartElement()) {
            const QString name = reader.name().toString();
            const auto attrs = reader.attributes();
            if (numberedNodeName(name, QStringLiteral("StbGroup"))) {
                groupRsdIds.insert(attrs.value(QStringLiteral("rsdID")).toString());
            } else if (numberedNodeName(name, QStringLiteral("StbRow"))) {
                rowRsdIds.insert(attrs.value(QStringLiteral("rsdID")).toString());
            } else if (numberedNodeName(name, QStringLiteral("Std"))) {
                currentStd = name;
                stdExpected[currentStd] = attrs.value(QStringLiteral("segCount")).toInt();
                stdActual[currentStd] = 0;
            } else if (numberedNodeName(name, QStringLiteral("StbGeo"))) {
                const QString segmentId = attrs.value(QStringLiteral("segID")).toString();
                if (segmentIds.count(segmentId) != 0) {
                    appendDiagnostic(result,
                                     QString::fromLatin1(kPackageValidationFailed),
                                     fileName,
                                     QStringLiteral("duplicate StbGeo.segID %1").arg(segmentId));
                }
                segmentIds.insert(segmentId);
                if (!currentStd.isEmpty()) {
                    ++stdActual[currentStd];
                }
            }
        } else if (reader.isEndElement() &&
                   numberedNodeName(reader.name().toString(), QStringLiteral("Std"))) {
            currentStd.clear();
        }
    }

    if (reader.hasError()) {
        appendDiagnostic(result,
                         QString::fromLatin1(kXmlParseFailed),
                         fileName,
                         reader.errorString());
    }
    if (groupRsdIds != rowRsdIds) {
        appendDiagnostic(result,
                         QString::fromLatin1(kPackageValidationFailed),
                         fileName,
                         QStringLiteral("StbGroup.rsdID and StbRow.rsdID mismatch"));
    }
    for (const auto& item : stdExpected) {
        if (item.second != stdActual[item.first]) {
            appendDiagnostic(result,
                             QString::fromLatin1(kPackageValidationFailed),
                             fileName,
                             QStringLiteral("%1 segCount mismatch").arg(item.first));
        }
    }
}

void validateL1(const QString& dir, const QStringList& packageFiles, DetailWriteResult& result)
{
    for (const QString& fileName : packageFiles) {
        if (fileName.endsWith(QStringLiteral(".stl"))) {
            validateL1File(dir, fileName, result);
        }
    }
}

void copyFileReplacing(const QString& src, const QString& dst)
{
    if (QFileInfo::exists(dst) && !QFile::remove(dst)) {
        throw std::runtime_error(QStringLiteral("cannot remove target before restore/copy: %1")
                                     .arg(dst)
                                     .toStdString());
    }
    if (!QFile::copy(src, dst)) {
        throw std::runtime_error(QStringLiteral("cannot copy %1 to %2")
                                     .arg(src, dst)
                                     .toStdString());
    }
}

void removeTargetDetailFiles(const QString& outputDir)
{
    QDir out(outputDir);
    const QFileInfoList detailFiles =
        out.entryInfoList(QStringList{QStringLiteral("Detail.xml"), QStringLiteral("Detail*.stl")},
                          QDir::Files);
    for (const QFileInfo& info : detailFiles) {
        QFile::remove(info.absoluteFilePath());
    }
}

void replacePackage(const QString& outputDir,
                    const QString& candidateDir,
                    const QStringList& packageFiles,
                    const DetailWriteOptions& options)
{
    QDir().mkpath(outputDir);
    QDir out(outputDir);
    const QString backupDir = outputDir + QStringLiteral(".detail_backup_tmp");
    removeDirIfExists(backupDir);
    QDir().mkpath(backupDir);

    const QFileInfoList existing =
        out.entryInfoList(QStringList{QStringLiteral("Detail.xml"), QStringLiteral("Detail*.stl")},
                          QDir::Files);
    QStringList backedUpNames;
    try {
        for (const QFileInfo& info : existing) {
            const QString backupPath = QDir(backupDir).filePath(info.fileName());
            copyFileReplacing(info.absoluteFilePath(), backupPath);
            backedUpNames.append(info.fileName());
        }
    } catch (...) {
        removeDirIfExists(backupDir);
        throw;
    }

    try {
        int copiedCount = 0;
        for (const QString& fileName : packageFiles) {
            const QString src = QDir(candidateDir).filePath(fileName);
            const QString dst = QDir(outputDir).filePath(fileName);
            copyFileReplacing(src, dst);
            ++copiedCount;
            if (options.testInjectInstallFailureAfterFirstCopy && copiedCount == 1) {
                throw std::runtime_error("test injected Detail package install failure");
            }
        }

        for (const QFileInfo& info : existing) {
            if (packageFiles.contains(info.fileName())) {
                continue;
            }
            if (!QFile::remove(info.absoluteFilePath())) {
                throw std::runtime_error(QStringLiteral("cannot remove stale Detail file: %1")
                                             .arg(info.absoluteFilePath())
                                             .toStdString());
            }
        }
        removeDirIfExists(backupDir);
    } catch (...) {
        removeTargetDetailFiles(outputDir);
        for (const QString& fileName : backedUpNames) {
            const QString backupPath = QDir(backupDir).filePath(fileName);
            const QString dst = QDir(outputDir).filePath(fileName);
            if (QFileInfo::exists(backupPath)) {
                copyFileReplacing(backupPath, dst);
            }
        }
        removeDirIfExists(backupDir);
        throw;
    }
}

} // namespace

DetailWriteResult DetailWriter::writePackage(
    const QString& outputDir,
    const SteelData& steelData,
    const DetailWriteOptions& options) const
{
    const QString root = QDir::cleanPath(QFileInfo(outputDir).absoluteFilePath());
    const QString candidate = root + QStringLiteral(".candidate_tmp");
    const QString oldHash = directoryHash(root);

    DetailWriteResult inputValidation = validateInput(steelData);
    if (!inputValidation.errorCodes.isEmpty()) {
        inputValidation.ok = false;
        inputValidation.decision = QStringLiteral("fail");
        inputValidation.l0 = QStringLiteral("not_run");
        inputValidation.l1 = QStringLiteral("not_run");
        inputValidation.oldPackagePreserved = directoryHash(root) == oldHash;
        inputValidation.dirtyAfter = true;
        return inputValidation;
    }

    DetailWriteResult result;
    result.candidatePackagePath = slashPath(candidate);
    const QVector<DetailDrawingViewOptions> views = effectiveViews(options);
    const QStringList packageFiles = detailPackageFilesForViews(views.size());
    const RebarSchedule schedulePreview = RebarScheduleService{}.buildSchedule(steelData);
    if (schedulePreview.massFormulaDeferred) {
        result.warnings.append(QString::fromLatin1(kMaterialDeferred));
    }

    try {
        removeDirIfExists(candidate);
        QDir().mkpath(candidate);
        writeStyleXml(QDir(candidate).filePath(QStringLiteral("Detail.xml")), steelData);
        for (int index = 0; index < views.size(); ++index) {
            writeDrawingXml(QDir(candidate).filePath(detailDrawingFileName(index + 1)),
                            steelData,
                            views.at(index));
        }

        validateL0(candidate, packageFiles, result);
        result.l0 = result.errorCodes.isEmpty() ? QStringLiteral("passed") : QStringLiteral("failed");
        if (result.errorCodes.isEmpty()) {
            validateL1(candidate, packageFiles, result);
        }
        result.l1 = result.errorCodes.isEmpty() ? QStringLiteral("passed") : QStringLiteral("failed");
        if (!result.errorCodes.isEmpty()) {
            result.ok = false;
            result.decision = QStringLiteral("fail");
            result.oldPackagePreserved = directoryHash(root) == oldHash;
            removeDirIfExists(candidate);
            return result;
        }

        replacePackage(root, candidate, packageFiles, options);
        removeDirIfExists(candidate);
        result.ok = true;
        result.decision = QStringLiteral("l0-l1-pass");
        result.l0 = QStringLiteral("passed");
        result.l1 = QStringLiteral("passed");
        result.l2 = QStringLiteral("not_run");
        result.files = packageFiles;
        result.dirtyAfter = false;
        result.oldPackagePreserved = true;
        return result;
    } catch (const std::exception& exc) {
        const QString code = result.l0 == QStringLiteral("passed") &&
                result.l1 == QStringLiteral("passed")
            ? QString::fromLatin1(kReplaceFailed)
            : QString::fromLatin1(kXmlWriteFailed);
        appendDiagnostic(result,
                         code,
                         QStringLiteral("Detail package"),
                         QString::fromStdString(exc.what()));
        removeDirIfExists(candidate);
        result.ok = false;
        result.decision = QStringLiteral("fail");
        result.l0 = result.l0 == QStringLiteral("not_run") ? QStringLiteral("failed") : result.l0;
        result.l1 = result.l1 == QStringLiteral("not_run") ? QStringLiteral("failed") : result.l1;
        result.oldPackagePreserved = directoryHash(root) == oldHash;
        result.dirtyAfter = true;
        return result;
    }
}

} // namespace tsrebar
