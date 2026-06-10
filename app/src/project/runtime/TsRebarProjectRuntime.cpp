#include "project/runtime/TsRebarProjectRuntime.h"

#include <QCryptographicHash>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QSaveFile>

#include <algorithm>
#include <map>
#include <set>
#include <stdexcept>
#include <utility>

namespace tsrebar {
namespace {

constexpr auto kManifest = "manifest.json";
constexpr auto kProject = "project.json";
constexpr auto kSourceStep = "geometry/source_step.json";
constexpr auto kTopologyRefs = "geometry/topology_refs.json";
constexpr auto kSteelData = "rebar/steel_data.json";
constexpr auto kGroups = "rebar/groups.json";
constexpr auto kBars = "rebar/bars.json";
constexpr auto kSegments = "rebar/segments.json";
constexpr auto kDetailMapping = "drawing/detail_mapping.json";
constexpr auto kEvidenceIndex = "evidence/evidence_index.json";
constexpr auto kUnresolvedFields = "evidence/unresolved_fields.json";

const QStringList kRequiredFiles{
    QString::fromLatin1(kProject),
    QString::fromLatin1(kSourceStep),
    QString::fromLatin1(kTopologyRefs),
    QString::fromLatin1(kSteelData),
    QString::fromLatin1(kGroups),
    QString::fromLatin1(kBars),
    QString::fromLatin1(kSegments),
    QString::fromLatin1(kDetailMapping),
    QString::fromLatin1(kEvidenceIndex),
    QString::fromLatin1(kUnresolvedFields),
};

QString qstr(const std::string& value)
{
    return QString::fromStdString(value);
}

std::string stdstr(const QString& value)
{
    return value.toStdString();
}

QString slashPath(const QString& path)
{
    return QDir::fromNativeSeparators(path);
}

QString absolutePackagePath(const QString& packagePath)
{
    return QDir::cleanPath(QFileInfo(packagePath).absoluteFilePath());
}

bool removeDirIfExists(const QString& path)
{
    QFileInfo info(path);
    if (info.exists() && !info.isDir()) {
        return QFile::remove(path);
    }
    QDir dir(path);
    return !dir.exists() || dir.removeRecursively();
}

void ensureParentDir(const QString& path)
{
    const QString parent = QFileInfo(path).absolutePath();
    if (!QDir().mkpath(parent)) {
        throw std::runtime_error(QString("cannot create parent directory: %1")
                                     .arg(parent)
                                     .toStdString());
    }
}

void writeJson(const QString& path, const QJsonObject& payload)
{
    ensureParentDir(path);
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        throw std::runtime_error(QString("cannot write json: %1")
                                     .arg(path)
                                     .toStdString());
    }
    file.write(QJsonDocument(payload).toJson(QJsonDocument::Indented));
    if (!file.commit()) {
        throw std::runtime_error(QString("cannot commit json: %1")
                                     .arg(path)
                                     .toStdString());
    }
}

QJsonObject readJsonObject(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        throw std::runtime_error(QString("cannot open json: %1")
                                     .arg(path)
                                     .toStdString());
    }

    QJsonParseError error;
    const QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &error);
    if (error.error != QJsonParseError::NoError || !document.isObject()) {
        throw std::runtime_error(QString("json object parse failed: %1")
                                     .arg(path)
                                     .toStdString());
    }
    return document.object();
}

QJsonArray stringArray(const std::vector<std::string>& values)
{
    QJsonArray array;
    for (const std::string& value : values) {
        array.append(qstr(value));
    }
    return array;
}

QJsonArray stringArray(const QStringList& values)
{
    QJsonArray array;
    for (const QString& value : values) {
        array.append(value);
    }
    return array;
}

QString bindingStateToString(BindingState state)
{
    switch (state) {
    case BindingState::Resolved:
        return QStringLiteral("resolved");
    case BindingState::UnresolvedGeometry:
        return QStringLiteral("unresolvedGeometry");
    case BindingState::PendingLegacyEvidence:
        return QStringLiteral("pendingLegacyEvidence");
    }
    return QStringLiteral("pendingLegacyEvidence");
}

BindingState bindingStateFromString(const QString& state)
{
    if (state == QStringLiteral("resolved")) {
        return BindingState::Resolved;
    }
    if (state == QStringLiteral("unresolvedGeometry")) {
        return BindingState::UnresolvedGeometry;
    }
    return BindingState::PendingLegacyEvidence;
}

QString segmentShapeToString(SteelBarSegmentShape shape)
{
    switch (shape) {
    case SteelBarSegmentShape::Line:
        return QStringLiteral("line");
    case SteelBarSegmentShape::Arc:
        return QStringLiteral("arc");
    case SteelBarSegmentShape::Point:
        return QStringLiteral("point");
    case SteelBarSegmentShape::Unknown:
        return QStringLiteral("unknown");
    }
    return QStringLiteral("unknown");
}

SteelBarSegmentShape segmentShapeFromString(const QString& shape)
{
    if (shape == QStringLiteral("line")) {
        return SteelBarSegmentShape::Line;
    }
    if (shape == QStringLiteral("arc")) {
        return SteelBarSegmentShape::Arc;
    }
    if (shape == QStringLiteral("point")) {
        return SteelBarSegmentShape::Point;
    }
    return SteelBarSegmentShape::Unknown;
}

QJsonArray evidenceIds(const std::vector<RebarEvidenceRef>& evidence)
{
    QJsonArray array;
    for (const RebarEvidenceRef& item : evidence) {
        array.append(qstr(item.evidenceId));
    }
    return array;
}

QJsonObject pointObject(const DomainPoint3d& point)
{
    return QJsonObject{{"x", point.x}, {"y", point.y}, {"z", point.z}};
}

DomainPoint3d pointFromObject(const QJsonObject& object)
{
    return {
        object.value("x").toDouble(),
        object.value("y").toDouble(),
        object.value("z").toDouble(),
    };
}

QString legacySchemaFor(const QString& legacyType)
{
    if (legacyType == QStringLiteral("steelData")) {
        return QStringLiteral("legacy-steelData-v1");
    }
    if (legacyType == QStringLiteral("steelbargroup")) {
        return QStringLiteral("legacy-steelbargroup-v1");
    }
    if (legacyType == QStringLiteral("steelbar")) {
        return QStringLiteral("legacy-steelbar-v1");
    }
    if (legacyType == QStringLiteral("seg_steelbargroup")) {
        return QStringLiteral("legacy-seg-steelbargroup-v1");
    }
    return QStringLiteral("legacy-unknown-v1");
}

QJsonObject legacyRawObject(const LegacyRawBlock& raw, const QString& fallbackType)
{
    const QString legacyType = raw.legacyType.empty() ? fallbackType : qstr(raw.legacyType);
    QJsonArray writeOrder;
    for (const std::string& item : raw.writeOrder) {
        writeOrder.append(qstr(item));
    }

    QJsonObject fields;
    for (const LegacyRawField& field : raw.fields) {
        if (QString::fromStdString(field.evidenceId).startsWith(QStringLiteral("GAP-"))) {
            fields.insert(qstr(field.name),
                          QJsonObject{
                              {"value", qstr(field.value)},
                              {"confidence", "pending"},
                              {"evidence", QJsonArray{qstr(field.evidenceId)}}});
            continue;
        }
        fields.insert(qstr(field.name), qstr(field.value));
    }

    QJsonObject rawObject{{"schema", legacySchemaFor(legacyType)}, {"fields", fields}};
    if (!writeOrder.isEmpty()) {
        rawObject.insert("legacyWriteOrder", writeOrder);
    }
    return QJsonObject{{"type", legacyType}, {"raw", rawObject}};
}

LegacyRawBlock legacyRawFromObject(const QJsonObject& legacyObject)
{
    LegacyRawBlock raw;
    raw.legacyType = stdstr(legacyObject.value("type").toString());
    const QJsonObject rawObject = legacyObject.value("raw").toObject();
    const QJsonArray writeOrder = rawObject.value("legacyWriteOrder").toArray();
    for (const QJsonValue& value : writeOrder) {
        raw.writeOrder.push_back(stdstr(value.toString()));
    }
    const QJsonObject fields = rawObject.value("fields").toObject();
    for (const QString& key : fields.keys()) {
        const QJsonValue value = fields.value(key);
        LegacyRawField field;
        field.name = stdstr(key);
        if (value.isObject()) {
            const QJsonObject object = value.toObject();
            const QJsonValue rawValue = object.value("value");
            field.value = rawValue.isDouble()
                ? std::to_string(rawValue.toDouble())
                : stdstr(rawValue.toString());
            const QJsonArray evidence = object.value("evidence").toArray();
            if (!evidence.isEmpty()) {
                field.evidenceId = stdstr(evidence.at(0).toString());
            }
        } else {
            field.value = stdstr(value.toString());
        }
        raw.fields.push_back(std::move(field));
    }
    return raw;
}

QJsonObject geometryRefObject(const GeometryReference& ref, const QString& sourceStepId)
{
    QJsonObject object{
        {"kernel", "OCCT"},
        {"sourceStepId", sourceStepId},
        {"topologyRefs", stringArray(ref.faceStableIds)},
        {"edgeRefs", stringArray(ref.edgeStableIds)},
        {"curveRefs", stringArray(ref.curveStableIds)},
    };
    return object;
}

GeometryReference geometryRefFromObject(const QJsonObject& object)
{
    GeometryReference ref;
    for (const QJsonValue& value : object.value("topologyRefs").toArray()) {
        ref.faceStableIds.push_back(stdstr(value.toString()));
    }
    for (const QJsonValue& value : object.value("edgeRefs").toArray()) {
        ref.edgeStableIds.push_back(stdstr(value.toString()));
    }
    for (const QJsonValue& value : object.value("curveRefs").toArray()) {
        if (value.isString()) {
            ref.curveStableIds.push_back(stdstr(value.toString()));
        } else if (value.isObject()) {
            ref.curveStableIds.push_back(stdstr(value.toObject().value("curveId").toString()));
        }
    }
    return ref;
}

QJsonObject bindingObject(const BindingAnchor& anchor)
{
    QJsonArray items;
    for (const BindingItem& item : anchor.items) {
        items.append(QJsonObject{
            {"legacyPath", qstr(item.legacyPath)},
            {"geometryPath", qstr(item.geometryPath)},
            {"evidence", QJsonArray{qstr(item.evidenceId)}}});
    }
    return QJsonObject{{"state", bindingStateToString(anchor.state)}, {"items", items}};
}

BindingAnchor bindingFromObject(const QJsonObject& object)
{
    BindingAnchor anchor;
    anchor.state = bindingStateFromString(object.value("state").toString());
    for (const QJsonValue& value : object.value("items").toArray()) {
        const QJsonObject item = value.toObject();
        BindingItem bindingItem;
        bindingItem.legacyPath = stdstr(item.value("legacyPath").toString());
        bindingItem.geometryPath = stdstr(item.value("geometryPath").toString());
        const QJsonArray evidence = item.value("evidence").toArray();
        if (!evidence.isEmpty()) {
            bindingItem.evidenceId = stdstr(evidence.at(0).toString());
        }
        anchor.items.push_back(std::move(bindingItem));
    }
    return anchor;
}

QJsonArray unresolvedFieldsFor(const std::vector<UnresolvedLegacyField>& fields,
                               const QString& objectId,
                               const QString& file,
                               int index)
{
    QJsonArray array;
    for (const UnresolvedLegacyField& field : fields) {
        array.append(QJsonObject{
            {"objectId", objectId},
            {"jsonPath", QStringLiteral("%1#/items/%2/legacyObject/raw/fields/%3")
                             .arg(file)
                             .arg(index)
                             .arg(qstr(field.fieldName))},
            {"gapId", qstr(field.gapId)},
            {"reason", qstr(field.reason)}});
    }
    return array;
}

void appendEvidence(QStringList& ids, const QString& id)
{
    if (!id.isEmpty() && !ids.contains(id)) {
        ids.append(id);
    }
}

QStringList collectEvidenceIds(const TsRebarProjectSnapshot& snapshot)
{
    QStringList ids{
        QStringLiteral("E-DEV-002"),
        QStringLiteral("E-DEV-004"),
        QStringLiteral("E-DEV-009"),
    };
    for (const std::string& id : snapshot.evidenceIds) {
        appendEvidence(ids, qstr(id));
    }
    for (const TsRebarTopologyRef& ref : snapshot.topologyRefs) {
        for (const std::string& id : ref.evidence) {
            appendEvidence(ids, qstr(id));
        }
    }
    for (const RebarEvidenceRef& item : snapshot.steelData.evidence) {
        appendEvidence(ids, qstr(item.evidenceId));
    }
    for (const SteelBarGroup& item : snapshot.steelData.groups) {
        for (const RebarEvidenceRef& evidence : item.evidence) {
            appendEvidence(ids, qstr(evidence.evidenceId));
        }
        for (const BindingItem& binding : item.binding.items) {
            appendEvidence(ids, qstr(binding.evidenceId));
        }
        for (const UnresolvedLegacyField& field : item.unresolvedLegacyFields) {
            appendEvidence(ids, qstr(field.gapId));
        }
    }
    for (const SteelBar& item : snapshot.steelData.bars) {
        for (const RebarEvidenceRef& evidence : item.evidence) {
            appendEvidence(ids, qstr(evidence.evidenceId));
        }
        for (const BindingItem& binding : item.binding.items) {
            appendEvidence(ids, qstr(binding.evidenceId));
        }
    }
    for (const SteelBarSegment& item : snapshot.steelData.segments) {
        for (const RebarEvidenceRef& evidence : item.evidence) {
            appendEvidence(ids, qstr(evidence.evidenceId));
        }
        for (const BindingItem& binding : item.binding.items) {
            appendEvidence(ids, qstr(binding.evidenceId));
        }
        for (const UnresolvedLegacyField& field : item.unresolvedLegacyFields) {
            appendEvidence(ids, qstr(field.gapId));
        }
    }
    return ids;
}

QJsonObject steelDataDoc(const SteelData& steelData)
{
    QJsonArray items;
    items.append(QJsonObject{
        {"id", qstr(steelData.steelDataId)},
        {"kind", "RebarSteelData"},
        {"legacyObject", legacyRawObject(steelData.legacyRaw, QStringLiteral("steelData"))},
        {"evidence", evidenceIds(steelData.evidence)}});
    return QJsonObject{{"schemaVersion", "rebar-steel-data/v1"}, {"items", items}};
}

QJsonObject groupsDoc(const SteelData& steelData, const QString& sourceStepId)
{
    QJsonArray items;
    for (const SteelBarGroup& group : steelData.groups) {
        QJsonArray segmentIds;
        for (const SteelBar& bar : steelData.bars) {
            if (bar.groupId != group.groupId) {
                continue;
            }
            for (const std::string& segmentId : bar.segmentIds) {
                segmentIds.append(qstr(segmentId));
            }
        }

        items.append(QJsonObject{
            {"id", qstr(group.groupId.empty() ? group.id : group.groupId)},
            {"kind", "RebarGroup"},
            {"groupId", qstr(group.groupId)},
            {"rsdId", qstr(group.rsdId)},
            {"steelDataId", qstr(group.steelDataId)},
            {"createCommand", qstr(group.createCommand)},
            {"legacyCommand", qstr(group.legacyCommand)},
            {"diameter", group.diameter},
            {"interval", group.interval},
            {"barCount", group.barCount},
            {"segmentCount", group.segmentCount},
            {"barIds", stringArray(group.barIds)},
            {"segmentIds", segmentIds},
            {"sourceCurveIds", stringArray(group.sourceCurveIds)},
            {"legacyObject", legacyRawObject(group.legacyRaw, QStringLiteral("steelbargroup"))},
            {"geometryRef", geometryRefObject(group.geometryRef, sourceStepId)},
            {"binding", bindingObject(group.binding)},
            {"evidence", evidenceIds(group.evidence)}});
    }
    return QJsonObject{{"schemaVersion", "rebar-groups/v1"}, {"items", items}};
}

QJsonObject barsDoc(const SteelData& steelData, const QString& sourceStepId)
{
    QJsonArray items;
    for (const SteelBar& bar : steelData.bars) {
        items.append(QJsonObject{
            {"id", qstr(bar.barId.empty() ? bar.id : bar.barId)},
            {"kind", "Rebar"},
            {"groupId", qstr(bar.groupId)},
            {"sequenceNo", bar.sequenceNo},
            {"segmentIds", stringArray(bar.segmentIds)},
            {"length", bar.length},
            {"shapeType", qstr(bar.shapeType)},
            {"legacyObject", legacyRawObject(bar.legacyRaw, QStringLiteral("steelbar"))},
            {"geometryRef", geometryRefObject(bar.geometryRef, sourceStepId)},
            {"binding", bindingObject(bar.binding)},
            {"evidence", evidenceIds(bar.evidence)}});
    }
    return QJsonObject{{"schemaVersion", "rebar-bars/v1"}, {"items", items}};
}

QJsonObject segmentsDoc(const SteelData& steelData, const QString& sourceStepId)
{
    std::map<std::string, std::string> groupIdBySegmentId;
    for (const SteelBar& bar : steelData.bars) {
        for (const std::string& segmentId : bar.segmentIds) {
            groupIdBySegmentId[segmentId] = bar.groupId;
        }
    }

    QJsonArray items;
    for (const SteelBarSegment& segment : steelData.segments) {
        const std::string segmentId = segment.segmentId.empty() ? segment.id : segment.segmentId;
        items.append(QJsonObject{
            {"id", qstr(segmentId)},
            {"kind", "RebarSegment"},
            {"groupId", qstr(groupIdBySegmentId[segmentId])},
            {"barId", qstr(segment.barId)},
            {"sequenceNo", segment.sequenceNo},
            {"shapeType", segmentShapeToString(segment.shapeType)},
            {"startPoint", pointObject(segment.startPoint)},
            {"middlePoint", pointObject(segment.middlePoint)},
            {"endPoint", pointObject(segment.endPoint)},
            {"length", segment.length},
            {"legacyObject", legacyRawObject(segment.legacyRaw, QStringLiteral("seg_steelbargroup"))},
            {"geometryRef", geometryRefObject(segment.geometryRef, sourceStepId)},
            {"binding", bindingObject(segment.binding)},
            {"evidence", evidenceIds(segment.evidence)}});
    }
    return QJsonObject{{"schemaVersion", "rebar-segments/v1"}, {"items", items}};
}

QJsonObject topologyRefsDoc(const TsRebarProjectSnapshot& snapshot)
{
    QJsonArray topologyRefs;
    QJsonArray geometryRefs;
    for (const TsRebarTopologyRef& ref : snapshot.topologyRefs) {
        topologyRefs.append(QJsonObject{
            {"topologyId", qstr(ref.topologyId)},
            {"shapeType", qstr(ref.shapeType)},
            {"sourceStepId", qstr(ref.sourceStepId)},
            {"fingerprint", QJsonObject{{"stableId", qstr(ref.fingerprint)}}},
            {"evidence", stringArray(ref.evidence)}});
        geometryRefs.append(QJsonObject{
            {"geometryRefId", QStringLiteral("geom-%1").arg(qstr(ref.topologyId))},
            {"sourceStepId", qstr(ref.sourceStepId)},
            {"kind", qstr(ref.shapeType)},
            {"topologyId", qstr(ref.topologyId)},
            {"kernel", "OCCT"},
            {"fingerprint", qstr(ref.fingerprint)},
            {"state", "resolved"},
            {"evidence", stringArray(ref.evidence)}});
    }
    return QJsonObject{
        {"schemaVersion", "geometry-topology-refs/v1"},
        {"sourceStepId", qstr(snapshot.sourceStep.sourceStepId)},
        {"geometryRefs", geometryRefs},
        {"topologyRefs", topologyRefs},
        {"unresolvedGeometry", QJsonArray{}}};
}

QJsonObject detailMappingDoc(const SteelData& steelData)
{
    QJsonArray groups;
    QJsonArray segments;
    QJsonArray tables;
    for (const SteelBarGroup& group : steelData.groups) {
        const QString groupId = qstr(group.groupId.empty() ? group.id : group.groupId);
        groups.append(QJsonObject{
            {"rebarGroupId", groupId},
            {"stbGroupId", groupId},
            {"groupId", groupId},
            {"rsdId", qstr(group.rsdId)}});
        tables.append(QJsonObject{{"sourceGroupId", groupId}, {"rsdId", qstr(group.rsdId)}});
    }
    for (const SteelBarSegment& segment : steelData.segments) {
        const QString segmentId = qstr(segment.segmentId.empty() ? segment.id : segment.segmentId);
        segments.append(QJsonObject{
            {"rebarSegmentId", segmentId},
            {"stbGeoSegId", segmentId},
            {"sourceCurveRef", segment.geometryRef.curveStableIds.empty()
                                   ? QString{}
                                   : qstr(segment.geometryRef.curveStableIds.front())}});
    }
    return QJsonObject{
        {"schemaVersion", "drawing-detail-mapping/v1"},
        {"groups", groups},
        {"segments", segments},
        {"tables", tables},
        {"evidence", QJsonArray{"E-DEV-004"}}};
}

QJsonObject evidenceIndexDoc(const QStringList& ids)
{
    QJsonArray items;
    for (const QString& id : ids) {
        items.append(QJsonObject{
            {"id", id},
            {"type", id.startsWith(QStringLiteral("GAP-")) ? "gap" : "actual-or-contract"},
            {"source", "runtime snapshot"},
            {"summary", QStringLiteral("runtime-preserved evidence %1").arg(id)}});
    }
    return QJsonObject{{"schemaVersion", "evidence-index/v1"}, {"items", items}};
}

QJsonObject unresolvedFieldsDoc(const TsRebarProjectSnapshot& snapshot)
{
    QJsonArray items;
    int index = 0;
    for (const SteelBarGroup& group : snapshot.steelData.groups) {
        const QJsonArray groupFields = unresolvedFieldsFor(
            group.unresolvedLegacyFields,
            qstr(group.groupId.empty() ? group.id : group.groupId),
            QString::fromLatin1(kGroups),
            index++);
        for (const QJsonValue& value : groupFields) {
            items.append(value);
        }
    }
    index = 0;
    for (const SteelBar& bar : snapshot.steelData.bars) {
        const QJsonArray barFields = unresolvedFieldsFor(
            bar.unresolvedLegacyFields,
            qstr(bar.barId.empty() ? bar.id : bar.barId),
            QString::fromLatin1(kBars),
            index++);
        for (const QJsonValue& value : barFields) {
            items.append(value);
        }
    }
    index = 0;
    for (const SteelBarSegment& segment : snapshot.steelData.segments) {
        const QJsonArray segmentFields = unresolvedFieldsFor(
            segment.unresolvedLegacyFields,
            qstr(segment.segmentId.empty() ? segment.id : segment.segmentId),
            QString::fromLatin1(kSegments),
            index++);
        for (const QJsonValue& value : segmentFields) {
            items.append(value);
        }
    }
    return QJsonObject{{"schemaVersion", "unresolved-fields/v1"}, {"items", items}};
}

void writeSnapshotPackage(const QString& packagePath, const TsRebarProjectSnapshot& snapshot)
{
    if (!removeDirIfExists(packagePath)) {
        throw std::runtime_error(QString("cannot remove candidate package: %1")
                                     .arg(packagePath)
                                     .toStdString());
    }
    QDir().mkpath(packagePath);

    QJsonArray requiredFiles;
    for (const QString& file : kRequiredFiles) {
        requiredFiles.append(file);
    }

    writeJson(QDir(packagePath).filePath(kManifest),
              QJsonObject{
                  {"format", "tsrebar"},
                  {"formatVersion", 1},
                  {"schemaVersion", "2026-06-05-draft"},
                  {"createdBy", "TushiRebarClone"},
                  {"requiredFiles", requiredFiles},
                  {"saveTransaction", QJsonObject{{"state", "complete"}, {"dirtyCleared", true}}},
                  {"evidence", stringArray(collectEvidenceIds(snapshot))}});

    writeJson(QDir(packagePath).filePath(kProject),
              QJsonObject{
                  {"schemaVersion", "project/v1"},
                  {"projectId", qstr(snapshot.projectId)},
                  {"name", qstr(snapshot.projectName)},
                  {"sourceModelId", qstr(snapshot.sourceStep.sourceStepId)},
                  {"dirtyFlags", QJsonObject{
                                     {"projectDirty", false},
                                     {"geometryDirty", false},
                                     {"rebarDirty", false},
                                     {"drawingDirty", false}}},
                  {"models", QJsonObject{
                                 {"structure", kSourceStep},
                                 {"topologyRefs", kTopologyRefs},
                                 {"steelData", kSteelData},
                                 {"groups", kGroups},
                                 {"bars", kBars},
                                 {"segments", kSegments},
                                 {"detailMapping", kDetailMapping},
                                 {"evidenceIndex", kEvidenceIndex},
                                 {"unresolvedFields", kUnresolvedFields}}}});

    writeJson(QDir(packagePath).filePath(kSourceStep),
              QJsonObject{
                  {"schemaVersion", "geometry-source-step/v1"},
                  {"sourceStepId", qstr(snapshot.sourceStep.sourceStepId)},
                  {"kind", "STEP"},
                  {"path", qstr(snapshot.sourceStep.path)},
                  {"originalPath", qstr(snapshot.sourceStep.originalPath)},
                  {"sha256", qstr(snapshot.sourceStep.sha256)},
                  {"importKernel", "OCCT"},
                  {"importStatus", "not-run"},
                  {"stepSelectionDiff", QJsonValue(QJsonValue::Null)},
                  {"evidence", QJsonArray{"E-DEV-009", "GAP-DEV-002"}}});

    writeJson(QDir(packagePath).filePath(kTopologyRefs), topologyRefsDoc(snapshot));
    writeJson(QDir(packagePath).filePath(kSteelData), steelDataDoc(snapshot.steelData));
    const QString sourceStepId = qstr(snapshot.sourceStep.sourceStepId);
    writeJson(QDir(packagePath).filePath(kGroups), groupsDoc(snapshot.steelData, sourceStepId));
    writeJson(QDir(packagePath).filePath(kBars), barsDoc(snapshot.steelData, sourceStepId));
    writeJson(QDir(packagePath).filePath(kSegments), segmentsDoc(snapshot.steelData, sourceStepId));
    writeJson(QDir(packagePath).filePath(kDetailMapping), detailMappingDoc(snapshot.steelData));
    writeJson(QDir(packagePath).filePath(kEvidenceIndex), evidenceIndexDoc(collectEvidenceIds(snapshot)));
    writeJson(QDir(packagePath).filePath(kUnresolvedFields), unresolvedFieldsDoc(snapshot));
}

QJsonValue resolvePointer(QJsonValue value, QString pointer)
{
    if (pointer.startsWith('/')) {
        pointer.remove(0, 1);
    }
    if (pointer.isEmpty()) {
        return value;
    }
    for (QString token : pointer.split('/', Qt::SkipEmptyParts)) {
        token.replace(QStringLiteral("~1"), QStringLiteral("/"));
        token.replace(QStringLiteral("~0"), QStringLiteral("~"));
        if (value.isObject()) {
            value = value.toObject().value(token);
        } else if (value.isArray()) {
            bool ok = false;
            const int index = token.toInt(&ok);
            const QJsonArray array = value.toArray();
            value = (ok && index >= 0 && index < array.size())
                ? array.at(index)
                : QJsonValue(QJsonValue::Undefined);
        } else {
            return QJsonValue(QJsonValue::Undefined);
        }
    }
    return value;
}

bool jsonPathExists(const QString& packagePath, const QString& jsonPath)
{
    const QStringList parts = jsonPath.split('#');
    const QString rel = parts.value(0);
    const QString path = QDir(packagePath).filePath(rel);
    if (!QFileInfo::exists(path)) {
        return false;
    }
    if (parts.size() == 1) {
        return true;
    }
    const QJsonObject object = readJsonObject(path);
    return !resolvePointer(QJsonValue(object), parts.value(1)).isUndefined();
}

QJsonArray itemsIn(const QString& packagePath, const QString& rel)
{
    const QString path = QDir(packagePath).filePath(rel);
    if (!QFileInfo::exists(path)) {
        return {};
    }
    return readJsonObject(path).value("items").toArray();
}

std::set<QString> idSet(const QJsonArray& items)
{
    std::set<QString> ids;
    for (const QJsonValue& value : items) {
        const QString id = value.toObject().value("id").toString();
        if (!id.isEmpty()) {
            ids.insert(id);
        }
    }
    return ids;
}

bool allStringRefsExist(const QJsonArray& refs, const std::set<QString>& ids)
{
    for (const QJsonValue& value : refs) {
        if (ids.find(value.toString()) == ids.end()) {
            return false;
        }
    }
    return true;
}

void appendUnique(QStringList& codes, const QString& code)
{
    if (!codes.contains(code)) {
        codes.append(code);
    }
}

TsRebarValidationSummary validatePackage(const QString& packagePath)
{
    TsRebarValidationSummary summary;

    const QString manifestPath = QDir(packagePath).filePath(kManifest);
    if (!QFileInfo::exists(manifestPath)) {
        summary.errorCodes.append(QStringLiteral("PV001_REQUIRED_FILE_MISSING"));
    } else {
        const QJsonObject manifest = readJsonObject(manifestPath);
        const QJsonArray required = manifest.value("requiredFiles").toArray();
        for (const QJsonValue& value : required) {
            if (!QFileInfo::exists(QDir(packagePath).filePath(value.toString()))) {
                summary.errorCodes.append(QStringLiteral("PV001_REQUIRED_FILE_MISSING"));
                break;
            }
        }
        if (manifest.value("saveTransaction").toObject().value("state").toString() !=
            QStringLiteral("complete")) {
            summary.errorCodes.append(QStringLiteral("PV004_SAVE_TRANSACTION_INCOMPLETE"));
        }
    }

    const QString unresolvedPath = QDir(packagePath).filePath(kUnresolvedFields);
    if (QFileInfo::exists(unresolvedPath)) {
        const int warningCount = readJsonObject(unresolvedPath).value("items").toArray().size();
        for (int i = 0; i < warningCount; ++i) {
            summary.warningCodes.append(QStringLiteral("EVW001_PENDING_LEGACY_FIELD"));
        }
    }

    const QJsonArray groups = itemsIn(packagePath, QString::fromLatin1(kGroups));
    const QJsonArray bars = itemsIn(packagePath, QString::fromLatin1(kBars));
    const QJsonArray segments = itemsIn(packagePath, QString::fromLatin1(kSegments));
    const std::set<QString> groupIds = idSet(groups);
    const std::set<QString> barIds = idSet(bars);
    const std::set<QString> segmentIds = idSet(segments);

    for (const QJsonValue& value : groups) {
        const QJsonObject group = value.toObject();
        if (!allStringRefsExist(group.value("barIds").toArray(), barIds) ||
            !allStringRefsExist(group.value("segmentIds").toArray(), segmentIds)) {
            appendUnique(summary.errorCodes, QStringLiteral("PRV002_REBAR_REF_BROKEN"));
        }
    }
    for (const QJsonValue& value : bars) {
        const QJsonObject bar = value.toObject();
        if (groupIds.find(bar.value("groupId").toString()) == groupIds.end() ||
            !allStringRefsExist(bar.value("segmentIds").toArray(), segmentIds)) {
            appendUnique(summary.errorCodes, QStringLiteral("PRV002_REBAR_REF_BROKEN"));
        }
    }
    for (const QJsonValue& value : segments) {
        const QJsonObject segment = value.toObject();
        if (groupIds.find(segment.value("groupId").toString()) == groupIds.end() ||
            barIds.find(segment.value("barId").toString()) == barIds.end()) {
            appendUnique(summary.errorCodes, QStringLiteral("PRV002_REBAR_REF_BROKEN"));
        }
    }

    for (const QString& rel : {QString::fromLatin1(kGroups),
                               QString::fromLatin1(kBars),
                               QString::fromLatin1(kSegments)}) {
        const QJsonArray items = itemsIn(packagePath, rel);
        for (const QJsonValue& value : items) {
            const QJsonObject item = value.toObject();
            const QJsonObject binding = item.value("binding").toObject();
            const QJsonArray bindings = binding.value("items").toArray();
            for (const QJsonValue& bindingValue : bindings) {
                const QString path = bindingValue.toObject().value("geometryPath").toString();
                if (!path.isEmpty() && !jsonPathExists(packagePath, path)) {
                    summary.errorCodes.append(QStringLiteral("LGV004_GEOMETRY_PATH_BROKEN"));
                    break;
                }
                if (path.isEmpty() && binding.value("state").toString() == QStringLiteral("resolved")) {
                    summary.errorCodes.append(QStringLiteral("LGV004_GEOMETRY_PATH_BROKEN"));
                    break;
                }
            }
            if (binding.value("state").toString() == QStringLiteral("unresolvedGeometry") &&
                item.value("openMode").toString() != QStringLiteral("repairRequired") &&
                item.value("openMode").toString() != QStringLiteral("readOnlyGeometry")) {
                summary.errorCodes.append(QStringLiteral("LGV007_UNRESOLVED_STATE_NOT_ENFORCED"));
            }
            if (!summary.errorCodes.isEmpty()) {
                break;
            }
        }
        if (!summary.errorCodes.isEmpty()) {
            break;
        }
    }

    summary.errorCount = summary.errorCodes.size();
    summary.warningCount = summary.warningCodes.size();
    if (summary.errorCount > 0) {
        summary.decision = QStringLiteral("fail");
    } else if (summary.warningCount > 0) {
        summary.decision = QStringLiteral("warning-only");
    } else {
        summary.decision = QStringLiteral("pass");
    }
    return summary;
}

void copyDir(const QString& src, const QString& dst)
{
    if (!removeDirIfExists(dst)) {
        throw std::runtime_error(QString("cannot remove destination: %1")
                                     .arg(dst)
                                     .toStdString());
    }
    QDir().mkpath(dst);
    QDir source(src);
    const QFileInfoList entries = source.entryInfoList(
        QDir::NoDotAndDotDot | QDir::AllEntries,
        QDir::DirsFirst | QDir::Name);
    for (const QFileInfo& entry : entries) {
        const QString targetPath = QDir(dst).filePath(entry.fileName());
        if (entry.isDir()) {
            copyDir(entry.absoluteFilePath(), targetPath);
        } else if (!QFile::copy(entry.absoluteFilePath(), targetPath)) {
            throw std::runtime_error(QString("cannot copy file: %1")
                                         .arg(entry.absoluteFilePath())
                                         .toStdString());
        }
    }
}

void replaceDir(const QString& current, const QString& candidate)
{
    const QString replaceTmp = current + QStringLiteral(".replace_tmp");
    const QString backup = current + QStringLiteral(".backup_tmp");
    removeDirIfExists(replaceTmp);
    removeDirIfExists(backup);
    copyDir(candidate, replaceTmp);
    if (QDir(current).exists() && !QDir().rename(current, backup)) {
        removeDirIfExists(replaceTmp);
        throw std::runtime_error(QString("cannot back up package: %1")
                                     .arg(current)
                                     .toStdString());
    }
    if (!QDir().rename(replaceTmp, current)) {
        if (QDir(backup).exists() && !QDir(current).exists()) {
            QDir().rename(backup, current);
        }
        removeDirIfExists(replaceTmp);
        throw std::runtime_error(QString("cannot install package: %1")
                                     .arg(current)
                                     .toStdString());
    }
    removeDirIfExists(backup);
}

ProjectDocumentState stateForOpen(const TsRebarValidationSummary& validation,
                                  const QStringList& bindingStates)
{
    if (validation.decision == QStringLiteral("pass")) {
        return ProjectDocumentState::OpenedFormal;
    }
    if (bindingStates.contains(QStringLiteral("repairRequired")) ||
        validation.errorCodes.contains(QStringLiteral("LGV004_GEOMETRY_PATH_BROKEN"))) {
        return ProjectDocumentState::OpenedRepairRequired;
    }
    if (validation.decision == QStringLiteral("warning-only")) {
        return ProjectDocumentState::OpenedWarning;
    }
    return ProjectDocumentState::OpenBlocked;
}

QString bindingDecisionFor(ProjectDocumentState state,
                           const TsRebarValidationSummary& validation,
                           const QStringList& bindingStates)
{
    if (state == ProjectDocumentState::OpenedFormal) {
        return QStringLiteral("resolved");
    }
    if (state == ProjectDocumentState::OpenedWarning) {
        return QStringLiteral("warningOnly");
    }
    if (state == ProjectDocumentState::OpenedRepairRequired) {
        if (validation.errorCodes.contains(QStringLiteral("LGV004_GEOMETRY_PATH_BROKEN"))) {
            return QStringLiteral("repairRequired");
        }
        if (bindingStates.contains(QStringLiteral("pendingLegacyEvidence"))) {
            return QStringLiteral("pendingLegacyEvidence");
        }
        return QStringLiteral("repairRequired");
    }
    return QStringLiteral("blocked");
}

QStringList collectBindingStates(const QString& packagePath)
{
    QStringList states;
    for (const QString& rel : {QString::fromLatin1(kGroups),
                               QString::fromLatin1(kBars),
                               QString::fromLatin1(kSegments)}) {
        const QJsonArray items = itemsIn(packagePath, rel);
        for (const QJsonValue& value : items) {
            const QJsonObject item = value.toObject();
            const QString state = item.value("binding").toObject().value("state").toString();
            if (!state.isEmpty()) {
                states.append(state);
            }
            if (item.value("openMode").toString() == QStringLiteral("repairRequired")) {
                states.append(QStringLiteral("repairRequired"));
            }
        }
    }
    return states;
}

std::vector<std::string> vectorFromJsonStrings(const QJsonArray& array)
{
    std::vector<std::string> result;
    for (const QJsonValue& value : array) {
        result.push_back(stdstr(value.toString()));
    }
    return result;
}

QString fieldNameFromJsonPath(const QString& jsonPath)
{
    const int index = jsonPath.lastIndexOf('/');
    if (index < 0 || index == jsonPath.size() - 1) {
        return {};
    }
    QString token = jsonPath.mid(index + 1);
    token.replace(QStringLiteral("~1"), QStringLiteral("/"));
    token.replace(QStringLiteral("~0"), QStringLiteral("~"));
    return token;
}

std::map<QString, std::vector<UnresolvedLegacyField>> unresolvedFieldsByObject(
    const QString& packagePath)
{
    std::map<QString, std::vector<UnresolvedLegacyField>> fields;
    const QString path = QDir(packagePath).filePath(kUnresolvedFields);
    if (!QFileInfo::exists(path)) {
        return fields;
    }

    const QJsonArray items = readJsonObject(path).value("items").toArray();
    for (const QJsonValue& value : items) {
        const QJsonObject object = value.toObject();
        const QString objectId = object.value("objectId").toString();
        const QString fieldName = fieldNameFromJsonPath(object.value("jsonPath").toString());
        if (objectId.isEmpty() || fieldName.isEmpty()) {
            continue;
        }
        fields[objectId].push_back({
            stdstr(fieldName),
            stdstr(object.value("reason").toString()),
            stdstr(object.value("gapId").toString()),
        });
    }
    return fields;
}

std::vector<std::string> evidenceIdsFromIndex(const QString& packagePath)
{
    std::vector<std::string> ids;
    const QString path = QDir(packagePath).filePath(kEvidenceIndex);
    if (!QFileInfo::exists(path)) {
        return ids;
    }

    const QJsonArray items = readJsonObject(path).value("items").toArray();
    for (const QJsonValue& value : items) {
        const QString id = value.toObject().value("id").toString();
        if (id.isEmpty()) {
            continue;
        }
        const std::string evidenceId = stdstr(id);
        if (std::find(ids.begin(), ids.end(), evidenceId) == ids.end()) {
            ids.push_back(evidenceId);
        }
    }
    return ids;
}

TsRebarProjectSnapshot readSnapshot(const QString& packagePath)
{
    TsRebarProjectSnapshot snapshot;
    const auto unresolved = unresolvedFieldsByObject(packagePath);
    snapshot.evidenceIds = evidenceIdsFromIndex(packagePath);
    if (QFileInfo::exists(QDir(packagePath).filePath(kProject))) {
        const QJsonObject project = readJsonObject(QDir(packagePath).filePath(kProject));
        snapshot.projectId = stdstr(project.value("projectId").toString());
        snapshot.projectName = stdstr(project.value("name").toString());
    }
    if (QFileInfo::exists(QDir(packagePath).filePath(kSourceStep))) {
        const QJsonObject source = readJsonObject(QDir(packagePath).filePath(kSourceStep));
        snapshot.sourceStep.sourceStepId = stdstr(source.value("sourceStepId").toString());
        snapshot.sourceStep.path = stdstr(source.value("path").toString());
        snapshot.sourceStep.originalPath = stdstr(source.value("originalPath").toString());
        snapshot.sourceStep.sha256 = stdstr(source.value("sha256").toString());
    }
    if (QFileInfo::exists(QDir(packagePath).filePath(kTopologyRefs))) {
        const QJsonArray refs =
            readJsonObject(QDir(packagePath).filePath(kTopologyRefs)).value("topologyRefs").toArray();
        for (const QJsonValue& value : refs) {
            const QJsonObject object = value.toObject();
            TsRebarTopologyRef ref;
            ref.topologyId = stdstr(object.value("topologyId").toString());
            ref.shapeType = stdstr(object.value("shapeType").toString());
            ref.sourceStepId = stdstr(object.value("sourceStepId").toString());
            ref.fingerprint = stdstr(object.value("fingerprint").toObject().value("stableId").toString());
            ref.evidence = vectorFromJsonStrings(object.value("evidence").toArray());
            snapshot.topologyRefs.push_back(std::move(ref));
        }
    }
    if (QFileInfo::exists(QDir(packagePath).filePath(kSteelData))) {
        const QJsonArray items =
            readJsonObject(QDir(packagePath).filePath(kSteelData)).value("items").toArray();
        if (!items.isEmpty()) {
            const QJsonObject item = items.at(0).toObject();
            snapshot.steelData.steelDataId = stdstr(item.value("id").toString());
            snapshot.steelData.legacyRaw = legacyRawFromObject(item.value("legacyObject").toObject());
            for (const QJsonValue& evidence : item.value("evidence").toArray()) {
                snapshot.steelData.evidence.push_back({stdstr(evidence.toString()), {}});
            }
        }
    }
    if (QFileInfo::exists(QDir(packagePath).filePath(kGroups))) {
        const QJsonArray items =
            readJsonObject(QDir(packagePath).filePath(kGroups)).value("items").toArray();
        for (const QJsonValue& value : items) {
            const QJsonObject object = value.toObject();
            SteelBarGroup group;
            group.groupId = stdstr(object.value("id").toString());
            group.id = group.groupId;
            group.rsdId = stdstr(object.value("rsdId").toString());
            group.steelDataId = stdstr(object.value("steelDataId").toString());
            group.createCommand = stdstr(object.value("createCommand").toString());
            group.legacyCommand = stdstr(object.value("legacyCommand").toString());
            group.diameter = object.value("diameter").toDouble();
            group.interval = object.value("interval").toDouble();
            group.barCount = object.value("barCount").toInt();
            group.segmentCount = object.value("segmentCount").toInt();
            group.barIds = vectorFromJsonStrings(object.value("barIds").toArray());
            group.sourceCurveIds = vectorFromJsonStrings(object.value("sourceCurveIds").toArray());
            group.legacyRaw = legacyRawFromObject(object.value("legacyObject").toObject());
            group.geometryRef = geometryRefFromObject(object.value("geometryRef").toObject());
            group.binding = bindingFromObject(object.value("binding").toObject());
            const auto unresolvedIt = unresolved.find(qstr(group.groupId));
            if (unresolvedIt != unresolved.end()) {
                group.unresolvedLegacyFields = unresolvedIt->second;
            }
            for (const QJsonValue& evidence : object.value("evidence").toArray()) {
                group.evidence.push_back({stdstr(evidence.toString()), {}});
            }
            snapshot.steelData.groups.push_back(std::move(group));
        }
    }
    if (QFileInfo::exists(QDir(packagePath).filePath(kBars))) {
        const QJsonArray items =
            readJsonObject(QDir(packagePath).filePath(kBars)).value("items").toArray();
        for (const QJsonValue& value : items) {
            const QJsonObject object = value.toObject();
            SteelBar bar;
            bar.barId = stdstr(object.value("id").toString());
            bar.id = bar.barId;
            bar.groupId = stdstr(object.value("groupId").toString());
            bar.sequenceNo = object.value("sequenceNo").toInt();
            bar.segmentIds = vectorFromJsonStrings(object.value("segmentIds").toArray());
            bar.length = object.value("length").toDouble();
            bar.shapeType = stdstr(object.value("shapeType").toString());
            bar.legacyRaw = legacyRawFromObject(object.value("legacyObject").toObject());
            bar.geometryRef = geometryRefFromObject(object.value("geometryRef").toObject());
            bar.binding = bindingFromObject(object.value("binding").toObject());
            const auto unresolvedIt = unresolved.find(qstr(bar.barId));
            if (unresolvedIt != unresolved.end()) {
                bar.unresolvedLegacyFields = unresolvedIt->second;
            }
            for (const QJsonValue& evidence : object.value("evidence").toArray()) {
                bar.evidence.push_back({stdstr(evidence.toString()), {}});
            }
            snapshot.steelData.bars.push_back(std::move(bar));
        }
    }
    if (QFileInfo::exists(QDir(packagePath).filePath(kSegments))) {
        const QJsonArray items =
            readJsonObject(QDir(packagePath).filePath(kSegments)).value("items").toArray();
        for (const QJsonValue& value : items) {
            const QJsonObject object = value.toObject();
            SteelBarSegment segment;
            segment.segmentId = stdstr(object.value("id").toString());
            segment.id = segment.segmentId;
            segment.barId = stdstr(object.value("barId").toString());
            segment.sequenceNo = object.value("sequenceNo").toInt();
            segment.shapeType = segmentShapeFromString(object.value("shapeType").toString());
            segment.startPoint = pointFromObject(object.value("startPoint").toObject());
            segment.middlePoint = pointFromObject(object.value("middlePoint").toObject());
            segment.endPoint = pointFromObject(object.value("endPoint").toObject());
            segment.length = object.value("length").toDouble();
            segment.legacyRaw = legacyRawFromObject(object.value("legacyObject").toObject());
            segment.geometryRef = geometryRefFromObject(object.value("geometryRef").toObject());
            segment.binding = bindingFromObject(object.value("binding").toObject());
            const auto unresolvedIt = unresolved.find(qstr(segment.segmentId));
            if (unresolvedIt != unresolved.end()) {
                segment.unresolvedLegacyFields = unresolvedIt->second;
            }
            for (const QJsonValue& evidence : object.value("evidence").toArray()) {
                segment.evidence.push_back({stdstr(evidence.toString()), {}});
            }
            snapshot.steelData.segments.push_back(std::move(segment));
        }
    }
    return snapshot;
}

} // namespace

QString TsRebarProjectRuntime::packageHash(const QString& packagePath) const
{
    const QString root = absolutePackagePath(packagePath);
    QStringList files;
    QDirIterator iterator(root, QDir::Files, QDirIterator::Subdirectories);
    const QDir rootDir(root);
    while (iterator.hasNext()) {
        const QString path = iterator.next();
        files.append(slashPath(rootDir.relativeFilePath(path)));
    }
    std::sort(files.begin(), files.end());

    QCryptographicHash hash(QCryptographicHash::Sha256);
    for (const QString& rel : files) {
        QFile file(rootDir.filePath(rel));
        if (!file.open(QIODevice::ReadOnly)) {
            throw std::runtime_error(QString("cannot hash package file: %1")
                                          .arg(file.fileName())
                                         .toStdString());
        }
        hash.addData(rel.toUtf8());
        hash.addData("\0", 1);
        hash.addData(file.readAll());
        hash.addData("\0", 1);
    }
    return QStringLiteral("sha256:") + QString::fromLatin1(hash.result().toHex());
}

TsRebarProjectSaveResult TsRebarProjectRuntime::saveSnapshot(
    const QString& packagePath,
    const TsRebarProjectSnapshot& snapshot,
    bool dirtyBefore) const
{
    const QString root = absolutePackagePath(packagePath);
    TsRebarProjectSaveResult result;
    result.dirtyBefore = dirtyBefore;
    result.dirtyAfter = dirtyBefore;

    const QString candidate = root + QStringLiteral(".candidate_tmp");
    result.candidatePackagePath = slashPath(candidate);
    try {
        result.oldPackageHash = QDir(root).exists() ? packageHash(root) : QString{};
        writeSnapshotPackage(candidate, snapshot);
        result.validation = validatePackage(candidate);

        if (result.validation.errorCount > 0) {
            result.ok = false;
            result.finalState = ProjectDocumentState::SaveFailed;
            result.bindingDecision = result.validation.errorCodes.contains(QStringLiteral("LGV004_GEOMETRY_PATH_BROKEN"))
                ? QStringLiteral("repairRequired")
                : QStringLiteral("blocked");
            result.newPackageHash = result.oldPackageHash;
            removeDirIfExists(candidate);
            return result;
        }

        const QString candidateHash = packageHash(candidate);
        replaceDir(root, candidate);
        removeDirIfExists(candidate);
        result.ok = true;
        result.finalState = result.validation.decision == QStringLiteral("warning-only")
            ? ProjectDocumentState::OpenedWarning
            : ProjectDocumentState::OpenedFormal;
        result.bindingDecision = result.finalState == ProjectDocumentState::OpenedWarning
            ? QStringLiteral("warningOnly")
            : QStringLiteral("resolved");
        result.dirtyAfter = false;
        result.newPackageHash = candidateHash;
        return result;
    } catch (const std::exception&) {
        removeDirIfExists(candidate);
        removeDirIfExists(root + QStringLiteral(".replace_tmp"));
        if (QDir(root + QStringLiteral(".backup_tmp")).exists() && !QDir(root).exists()) {
            QDir().rename(root + QStringLiteral(".backup_tmp"), root);
        }
        result.ok = false;
        result.finalState = ProjectDocumentState::SaveFailed;
        result.bindingDecision = QStringLiteral("blocked");
        result.newPackageHash = result.oldPackageHash;
        result.dirtyAfter = dirtyBefore;
    }
    return result;
}

TsRebarProjectOpenResult TsRebarProjectRuntime::open(const QString& packagePath) const
{
    const QString root = absolutePackagePath(packagePath);
    TsRebarProjectOpenResult result;
    result.validation = validatePackage(root);
    const QStringList bindingStates = collectBindingStates(root);
    result.finalState = stateForOpen(result.validation, bindingStates);
    result.bindingDecision = bindingDecisionFor(result.finalState, result.validation, bindingStates);
    result.ok = result.finalState != ProjectDocumentState::OpenBlocked;
    result.snapshot = readSnapshot(root);
    return result;
}

} // namespace tsrebar
