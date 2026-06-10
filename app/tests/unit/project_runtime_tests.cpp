#include "domain/rebar/SteelData.h"
#include "project/runtime/TsRebarProjectRuntime.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTemporaryDir>

#include <cmath>
#include <algorithm>
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

QJsonObject readObject(const QString& path)
{
    QFile file(path);
    expect(file.open(QIODevice::ReadOnly), "json file must open");
    const QJsonDocument document = QJsonDocument::fromJson(file.readAll());
    expect(document.isObject(), "json root must be object");
    return document.object();
}

bool near(double left, double right)
{
    return std::abs(left - right) < 1.0e-9;
}

struct CurrentDirGuard
{
    QString previous = QDir::currentPath();

    ~CurrentDirGuard()
    {
        QDir::setCurrent(previous);
    }
};

tsrebar::SteelData steelDataWithLineGroup()
{
    tsrebar::SteelBarSegment segment;
    segment.segmentId = "segment-001";
    segment.barId = "bar-001";
    segment.sequenceNo = 1;
    segment.shapeType = tsrebar::SteelBarSegmentShape::Line;
    segment.startPoint = {0.0, 0.0, 1.0};
    segment.endPoint = {12.0, 0.0, 1.0};
    segment.middlePoint = {6.0, 0.0, 1.0};
    segment.length = 12.0;
    segment.legacyRaw.legacyType = "seg_steelbargroup";
    segment.legacyRaw.writeOrder = {"int68", "string72", "ptr80", "ptr96", "ptr88", "int64"};
    segment.legacyRaw.fields.push_back({"string72DisplayName", "fixture-segment", "E-IDA-017"});
    segment.legacyRaw.fields.push_back({"ptr80", "pending-ptr80", "GAP-SFL-005"});
    segment.geometryRef.curveStableIds.push_back("selection-v1:edge:17");
    segment.binding.state = tsrebar::BindingState::Resolved;
    segment.binding.items.push_back({
        "rebar/segments.json#/items/0/legacyObject/raw/fields/string72DisplayName",
        "geometry/topology_refs.json#/topologyRefs/1",
        "E-DEV-004",
    });
    segment.unresolvedLegacyFields.push_back({"ptr80", "pending legacy pointer role", "GAP-SFL-005"});
    segment.evidence.push_back({"E-IDA-017", "seg_steelbargroup read/write evidence"});
    segment.evidence.push_back({"GAP-SFL-005", "legacy pointer role pending"});

    tsrebar::SteelBar bar;
    bar.barId = "bar-001";
    bar.groupId = "group-001";
    bar.sequenceNo = 1;
    bar.segmentIds.push_back(segment.segmentId);
    bar.length = segment.length;
    bar.shapeType = "lineStb";
    bar.legacyRaw.legacyType = "steelbar";
    bar.legacyRaw.fields.push_back({"name64", "fixture-bar", "E-IDA-017"});
    bar.geometryRef.curveStableIds.push_back("curve-seg-001");
    bar.binding.state = tsrebar::BindingState::Resolved;
    bar.binding.items.push_back({
        "rebar/bars.json#/items/0/legacyObject/raw/fields/name64",
        "rebar/segments.json#/items/0/geometryRef/curveRefs/0",
        "E-DEV-004",
    });
    bar.evidence.push_back({"E-IDA-017", "steelbar evidence"});

    tsrebar::SteelBarGroup group;
    group.groupId = "group-001";
    group.rsdId = "rsd-001";
    group.steelDataId = "steel-data-001";
    group.createCommand = "Rebar.Create.LineGroup";
    group.legacyCommand = "sgroupbarline";
    group.diameter = 25.0;
    group.interval = 200.0;
    group.barCount = 1;
    group.segmentCount = 1;
    group.barIds.push_back(bar.barId);
    group.sourceCurveIds.push_back("selection-v1:edge:17");
    group.legacyRaw.legacyType = "steelbargroup";
    group.legacyRaw.fields.push_back({"name72", "fixture-group", "E-IDA-016"});
    group.legacyRaw.fields.push_back({"positions168_192", "pending-position-block", "GAP-SFL-004"});
    group.geometryRef.faceStableIds.push_back("face-001");
    group.geometryRef.curveStableIds.push_back("selection-v1:edge:17");
    group.binding.state = tsrebar::BindingState::Resolved;
    group.binding.items.push_back({
        "rebar/groups.json#/items/0/legacyObject/raw/fields/positions168_192",
        "geometry/topology_refs.json#/topologyRefs/0",
        "E-DEV-004",
    });
    group.unresolvedLegacyFields.push_back({
        "positions168_192",
        "steelbargroup large position block pending",
        "GAP-SFL-004",
    });
    group.evidence.push_back({"E-IDA-016", "steelbargroup read/write evidence"});
    group.evidence.push_back({"E-DEV-004", "legacy geometry binding contract"});

    tsrebar::SteelData steelData;
    steelData.steelDataId = "steel-data-001";
    steelData.level = "HRB400";
    steelData.gradeName = "fixture-grade";
    steelData.legacyRaw.legacyType = "steelData";
    steelData.legacyRaw.fields.push_back({"name", "fixture-steel-data", "E-IDA-015"});
    steelData.evidence.push_back({"E-IDA-015", "steelData evidence"});
    steelData.groups.push_back(group);
    steelData.bars.push_back(bar);
    steelData.segments.push_back(segment);
    return steelData;
}

tsrebar::TsRebarProjectSnapshot snapshotWithLineGroup()
{
    tsrebar::TsRebarProjectSnapshot snapshot;
    snapshot.projectId = "proj-runtime-test";
    snapshot.projectName = "runtime test";
    snapshot.sourceStep.sourceStepId = "step-main";
    snapshot.sourceStep.path = "models/source.step";
    snapshot.sourceStep.originalPath = "C:/project/source.step";
    snapshot.sourceStep.sha256 = "sha256-fixture-source-step";
    snapshot.topologyRefs.push_back({"face-001", "face", "step-main", "face-fingerprint", {"GAP-DEV-002"}});
    snapshot.topologyRefs.push_back({"edge-001", "edge", "step-main", "edge-fingerprint", {"GAP-DEV-002"}});
    snapshot.steelData = steelDataWithLineGroup();
    snapshot.evidenceIds = {
        "E-DEV-004",
        "E-IDA-015",
        "E-IDA-016",
        "E-IDA-017",
        "GAP-DEV-002",
        "GAP-SFL-004",
        "GAP-SFL-005",
    };
    return snapshot;
}

void testSaveWritesJointTsRebarPackageAndOpenRestoresDomain()
{
    QTemporaryDir temp;
    expect(temp.isValid(), "temporary dir must be valid");
    const QString packagePath = QDir(temp.path()).filePath("project.tsrebar");

    const tsrebar::TsRebarProjectRuntime runtime;
    const auto save = runtime.saveSnapshot(packagePath, snapshotWithLineGroup(), true);

    expect(save.finalState == tsrebar::ProjectDocumentState::OpenedWarning,
           "valid package with pending evidence must save as OpenedWarning");
    expect(save.validation.decision == "warning-only",
           "save validation must preserve pending legacy warnings");
    expect(!save.dirtyAfter, "successful save must clear dirty");
    expect(save.newPackageHash == runtime.packageHash(packagePath),
           "save result hash must match installed package hash");
    expect(QFileInfo::exists(QDir(packagePath).filePath("manifest.json")),
           "manifest must be written");
    expect(QFileInfo::exists(QDir(packagePath).filePath("geometry/source_step.json")),
           "source_step must be written");
    expect(QFileInfo::exists(QDir(packagePath).filePath("rebar/groups.json")),
           "groups must be written");

    const QJsonObject group =
        readObject(QDir(packagePath).filePath("rebar/groups.json"))
            .value("items").toArray().at(0).toObject();
    expect(group.value("legacyObject").toObject().contains("raw"),
           "group must keep legacyObject.raw");
    expect(group.value("geometryRef").toObject().value("topologyRefs").toArray().at(0).toString() == "face-001",
           "group must keep geometryRef topology reference");
    expect(group.value("geometryRef").toObject().value("sourceStepId").toString() == "step-main",
           "group geometryRef must use snapshot STEP source id");
    expect(group.value("binding").toObject().value("items").toArray().size() == 1,
           "group must keep binding items");
    expect(group.value("evidence").toArray().contains("E-IDA-016"),
           "group must keep IDA evidence");

    const QJsonObject segment =
        readObject(QDir(packagePath).filePath("rebar/segments.json"))
            .value("items").toArray().at(0).toObject();
    expect(segment.value("groupId").toString() == "group-001",
           "segment must preserve owning groupId for ProjectReferenceValidator");

    const auto open = runtime.open(packagePath);
    expect(open.finalState == tsrebar::ProjectDocumentState::OpenedWarning,
           "open must restore warning state for pending legacy fields");
    expect(open.snapshot.sourceStep.sourceStepId == "step-main",
           "open must restore STEP source id");
    expect(open.snapshot.steelData.groups.size() == 1,
           "open must restore one group");
    expect(open.snapshot.steelData.bars.size() == 1,
           "open must restore one bar");
    expect(open.snapshot.steelData.segments.size() == 1,
           "open must restore one segment");
    expect(open.snapshot.steelData.groups.front().groupId == "group-001",
           "open group id mismatch");
    expect(open.snapshot.steelData.groups.front().binding.state == tsrebar::BindingState::Resolved,
           "open must restore resolved group binding");
    expect(open.snapshot.steelData.segments.front().geometryRef.curveStableIds.front() ==
               "selection-v1:edge:17",
           "open must restore selection-v1 curve ref");
    expect(near(open.snapshot.steelData.segments.front().length, 12.0),
           "open must restore segment length");
}

void testOpenBrokenBindingEntersRepairRequiredWithoutSilentRebind()
{
    QTemporaryDir temp;
    expect(temp.isValid(), "temporary dir must be valid");
    const QString packagePath = QDir(temp.path()).filePath("broken.tsrebar");

    tsrebar::TsRebarProjectSnapshot snapshot = snapshotWithLineGroup();
    snapshot.steelData.groups.front().binding.items.front().geometryPath =
        "geometry/topology_refs.json#/topologyRefs/999";
    snapshot.steelData.groups.front().binding.state = tsrebar::BindingState::UnresolvedGeometry;

    const tsrebar::TsRebarProjectRuntime runtime;
    const auto save = runtime.saveSnapshot(packagePath, snapshot, true);
    expect(save.finalState == tsrebar::ProjectDocumentState::SaveFailed,
           "broken binding must not be saved as formal package");

    const QString validPath = QDir(temp.path()).filePath("valid.tsrebar");
    expect(runtime.saveSnapshot(validPath, snapshotWithLineGroup(), true).ok,
           "valid package must be available for mutation");
    QJsonObject groups = readObject(QDir(validPath).filePath("rebar/groups.json"));
    QJsonObject group = groups.value("items").toArray().at(0).toObject();
    QJsonObject binding = group.value("binding").toObject();
    QJsonArray items = binding.value("items").toArray();
    QJsonObject bindingItem = items.at(0).toObject();
    bindingItem.insert("geometryPath", "geometry/topology_refs.json#/topologyRefs/999");
    items.replace(0, bindingItem);
    binding.insert("items", items);
    binding.insert("state", "unresolvedGeometry");
    group.insert("binding", binding);
    group.insert("openMode", "repairRequired");
    QJsonArray groupItems = groups.value("items").toArray();
    groupItems.replace(0, group);
    groups.insert("items", groupItems);
    QFile out(QDir(validPath).filePath("rebar/groups.json"));
    expect(out.open(QIODevice::WriteOnly | QIODevice::Text), "mutated groups must open");
    out.write(QJsonDocument(groups).toJson(QJsonDocument::Indented));
    out.close();

    const auto open = runtime.open(validPath);
    expect(open.finalState == tsrebar::ProjectDocumentState::OpenedRepairRequired,
           "broken geometryPath must open as repairRequired");
    expect(open.bindingDecision == "repairRequired",
           "binding decision must be repairRequired");
    expect(open.validation.errorCodes.contains("LGV004_GEOMETRY_PATH_BROKEN"),
           "broken binding diagnostic must be stable");
}

void testSaveFailurePreservesOldPackageHashAndDirtyState()
{
    QTemporaryDir temp;
    expect(temp.isValid(), "temporary dir must be valid");
    const QString packagePath = QDir(temp.path()).filePath("current.tsrebar");

    const tsrebar::TsRebarProjectRuntime runtime;
    const auto initialSave = runtime.saveSnapshot(packagePath, snapshotWithLineGroup(), true);
    expect(initialSave.ok, "initial valid save must succeed");
    const QString oldHash = runtime.packageHash(packagePath);

    tsrebar::TsRebarProjectSnapshot broken = snapshotWithLineGroup();
    broken.steelData.groups.front().binding.items.front().geometryPath =
        "geometry/topology_refs.json#/topologyRefs/999";

    const auto failedSave = runtime.saveSnapshot(packagePath, broken, true);

    expect(!failedSave.ok, "broken candidate save must fail");
    expect(failedSave.finalState == tsrebar::ProjectDocumentState::SaveFailed,
           "broken candidate must enter SaveFailed");
    expect(failedSave.dirtyAfter, "failed save must preserve dirty");
    expect(failedSave.oldPackageHash == oldHash, "old hash before save mismatch");
    expect(failedSave.newPackageHash == oldHash,
           "failed save must preserve existing package hash");
    expect(failedSave.validation.errorCodes.contains("LGV004_GEOMETRY_PATH_BROKEN"),
           "failed save must report broken binding");
}

void testSaveRejectsBrokenRebarReferencesBeforeReplacingPackage()
{
    QTemporaryDir temp;
    expect(temp.isValid(), "temporary dir must be valid");
    const QString packagePath = QDir(temp.path()).filePath("broken_refs.tsrebar");

    tsrebar::TsRebarProjectSnapshot broken = snapshotWithLineGroup();
    broken.steelData.bars.front().groupId = "missing-group";

    const tsrebar::TsRebarProjectRuntime runtime;
    const auto save = runtime.saveSnapshot(packagePath, broken, true);

    expect(!save.ok, "broken rebar references must not save");
    expect(save.finalState == tsrebar::ProjectDocumentState::SaveFailed,
           "broken rebar references must enter SaveFailed");
    expect(save.dirtyAfter, "broken rebar references must preserve dirty");
    expect(save.validation.errorCodes.contains("PRV002_REBAR_REF_BROKEN"),
           "broken rebar references must emit PRV002_REBAR_REF_BROKEN");
    expect(!QDir(packagePath).exists(),
           "broken rebar references must not install a formal package");
}

void testSaveRejectsResolvedBindingWithoutGeometryPath()
{
    QTemporaryDir temp;
    expect(temp.isValid(), "temporary dir must be valid");
    const QString packagePath = QDir(temp.path()).filePath("missing_geometry_path.tsrebar");

    tsrebar::TsRebarProjectSnapshot broken = snapshotWithLineGroup();
    broken.steelData.groups.front().binding.items.front().geometryPath.clear();

    const tsrebar::TsRebarProjectRuntime runtime;
    const auto save = runtime.saveSnapshot(packagePath, broken, true);

    expect(!save.ok, "resolved binding without geometryPath must not save");
    expect(save.finalState == tsrebar::ProjectDocumentState::SaveFailed,
           "resolved binding without geometryPath must enter SaveFailed");
    expect(save.validation.errorCodes.contains("LGV004_GEOMETRY_PATH_BROKEN"),
           "resolved binding without geometryPath must emit LGV004");
    expect(!QDir(packagePath).exists(),
           "resolved binding without geometryPath must not install formal package");
    expect(!QDir(save.candidatePackagePath).exists(),
           "failed save must clean candidate package");
}

void testSaveRejectsUnresolvedBindingWithoutRepairOpenMode()
{
    QTemporaryDir temp;
    expect(temp.isValid(), "temporary dir must be valid");
    const QString packagePath = QDir(temp.path()).filePath("missing_open_mode.tsrebar");

    tsrebar::TsRebarProjectSnapshot broken = snapshotWithLineGroup();
    broken.steelData.groups.front().binding.state = tsrebar::BindingState::UnresolvedGeometry;

    const tsrebar::TsRebarProjectRuntime runtime;
    const auto save = runtime.saveSnapshot(packagePath, broken, true);

    expect(!save.ok, "unresolvedGeometry without repair openMode must not save");
    expect(save.finalState == tsrebar::ProjectDocumentState::SaveFailed,
           "unresolvedGeometry without repair openMode must enter SaveFailed");
    expect(save.validation.errorCodes.contains("LGV007_UNRESOLVED_STATE_NOT_ENFORCED"),
           "unresolvedGeometry without repair openMode must emit LGV007");
    expect(!QDir(packagePath).exists(),
           "unresolvedGeometry without repair openMode must not install formal package");
    expect(!QDir(save.candidatePackagePath).exists(),
           "failed save must clean candidate package");
}

void testSaveDoesNotHardcodeStepMainInGeometryRefs()
{
    QTemporaryDir temp;
    expect(temp.isValid(), "temporary dir must be valid");
    const QString packagePath = QDir(temp.path()).filePath("source_id.tsrebar");

    tsrebar::TsRebarProjectSnapshot snapshot = snapshotWithLineGroup();
    snapshot.sourceStep.sourceStepId = "step-alt";
    for (tsrebar::TsRebarTopologyRef& ref : snapshot.topologyRefs) {
        ref.sourceStepId = "step-alt";
    }

    const tsrebar::TsRebarProjectRuntime runtime;
    const auto save = runtime.saveSnapshot(packagePath, snapshot, true);

    expect(save.ok, "alternate STEP source id package must save");
    const QJsonObject group =
        readObject(QDir(packagePath).filePath("rebar/groups.json"))
            .value("items").toArray().at(0).toObject();
    expect(group.value("geometryRef").toObject().value("sourceStepId").toString() == "step-alt",
           "group geometryRef sourceStepId must not be hardcoded");
    const QJsonObject segment =
        readObject(QDir(packagePath).filePath("rebar/segments.json"))
            .value("items").toArray().at(0).toObject();
    expect(segment.value("geometryRef").toObject().value("sourceStepId").toString() == "step-alt",
           "segment geometryRef sourceStepId must not be hardcoded");
}

void testSaveAcceptsRelativePackagePathWithoutHashHang()
{
    QTemporaryDir temp;
    expect(temp.isValid(), "temporary dir must be valid");
    CurrentDirGuard cwd;
    expect(QDir::setCurrent(temp.path()), "must enter temporary cwd");

    const tsrebar::TsRebarProjectRuntime runtime;
    const auto save = runtime.saveSnapshot("relative_project.tsrebar",
                                           snapshotWithLineGroup(),
                                           true);

    expect(save.ok, "relative package path must save");
    expect(!save.newPackageHash.isEmpty(), "relative package path must produce final hash");
    expect(QFileInfo::exists(QDir(temp.path()).filePath("relative_project.tsrebar/manifest.json")),
           "relative package path must write manifest in cwd");
}

void testOpenSaveRoundTripPreservesUnresolvedFields()
{
    QTemporaryDir temp;
    expect(temp.isValid(), "temporary dir must be valid");
    const QString firstPath = QDir(temp.path()).filePath("first.tsrebar");
    const QString secondPath = QDir(temp.path()).filePath("second.tsrebar");

    const tsrebar::TsRebarProjectRuntime runtime;
    const auto firstSave = runtime.saveSnapshot(firstPath, snapshotWithLineGroup(), true);
    expect(firstSave.validation.decision == "warning-only",
           "first save must keep pending legacy warnings");

    const auto open = runtime.open(firstPath);
    expect(open.snapshot.steelData.groups.front().unresolvedLegacyFields.size() == 1,
           "open must restore group unresolved legacy fields");
    expect(open.snapshot.steelData.segments.front().unresolvedLegacyFields.size() == 1,
           "open must restore segment unresolved legacy fields");

    const auto secondSave = runtime.saveSnapshot(secondPath, open.snapshot, true);
    expect(secondSave.validation.decision == "warning-only",
           "save after open must not lose pending legacy warnings");

    const QJsonArray unresolved =
        readObject(QDir(secondPath).filePath("evidence/unresolved_fields.json"))
            .value("items").toArray();
    expect(unresolved.size() == 2,
           "save after open must preserve unresolved_fields entries");
}

void testOpenRestoresEvidenceIndexIds()
{
    QTemporaryDir temp;
    expect(temp.isValid(), "temporary dir must be valid");
    const QString packagePath = QDir(temp.path()).filePath("evidence_index.tsrebar");

    const tsrebar::TsRebarProjectRuntime runtime;
    const auto save = runtime.saveSnapshot(packagePath, snapshotWithLineGroup(), true);
    expect(save.ok, "evidence index fixture must save");

    const auto open = runtime.open(packagePath);

    const auto containsEvidence = [&](const std::string& id) {
        return std::find(open.snapshot.evidenceIds.begin(),
                         open.snapshot.evidenceIds.end(),
                         id) != open.snapshot.evidenceIds.end();
    };
    expect(containsEvidence("E-DEV-004"),
           "open must restore snapshot evidence ids from evidence_index");
    expect(containsEvidence("E-IDA-016"),
           "open must restore group evidence id from evidence_index");
    expect(containsEvidence("GAP-SFL-004"),
           "open must restore gap id from evidence_index");
}

void testSaveInstallFailureCleansTemporaryDirsAndKeepsDirty()
{
    QTemporaryDir temp;
    expect(temp.isValid(), "temporary dir must be valid");
    const QString packagePath = QDir(temp.path()).filePath("blocked.tsrebar");

    QFile blocker(packagePath);
    expect(blocker.open(QIODevice::WriteOnly | QIODevice::Text),
           "blocking file must open");
    blocker.write("existing file must remain");
    blocker.close();

    const tsrebar::TsRebarProjectRuntime runtime;
    const auto save = runtime.saveSnapshot(packagePath, snapshotWithLineGroup(), true);

    expect(!save.ok, "install failure must not report success");
    expect(save.finalState == tsrebar::ProjectDocumentState::SaveFailed,
           "install failure must enter SaveFailed");
    expect(save.dirtyAfter, "install failure must keep dirty");
    expect(QFileInfo(packagePath).isFile(), "blocking file must remain in place");
    expect(!QDir(save.candidatePackagePath).exists(),
           "install failure must clean candidate package");
    expect(!QDir(packagePath + ".replace_tmp").exists(),
           "install failure must clean replace tmp");
    expect(!QDir(packagePath + ".backup_tmp").exists(),
           "install failure must not leave backup tmp");
}

void exportRuntimePackageIfRequested()
{
    const QByteArray exportPath = qgetenv("TSREBAR_RUNTIME_FIXTURE_OUT");
    if (exportPath.isEmpty()) {
        return;
    }

    const tsrebar::TsRebarProjectRuntime runtime;
    const auto save = runtime.saveSnapshot(QString::fromLocal8Bit(exportPath),
                                           snapshotWithLineGroup(),
                                           true);
    expect(save.ok, "exported runtime fixture must save for external validator");
}

} // namespace

int main()
{
    testSaveWritesJointTsRebarPackageAndOpenRestoresDomain();
    testOpenBrokenBindingEntersRepairRequiredWithoutSilentRebind();
    testSaveFailurePreservesOldPackageHashAndDirtyState();
    testSaveRejectsBrokenRebarReferencesBeforeReplacingPackage();
    testSaveRejectsResolvedBindingWithoutGeometryPath();
    testSaveRejectsUnresolvedBindingWithoutRepairOpenMode();
    testSaveDoesNotHardcodeStepMainInGeometryRefs();
    testSaveAcceptsRelativePackagePathWithoutHashHang();
    testOpenSaveRoundTripPreservesUnresolvedFields();
    testOpenRestoresEvidenceIndexIds();
    testSaveInstallFailureCleansTemporaryDirsAndKeepsDirty();
    exportRuntimePackageIfRequested();
    return 0;
}
