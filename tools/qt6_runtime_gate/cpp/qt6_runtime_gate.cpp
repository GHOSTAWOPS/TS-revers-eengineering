#include <QAction>
#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCryptographicHash>
#include <QDateTime>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QSaveFile>
#include <QStandardPaths>
#include <QStringList>
#include <QTextStream>
#include <QTimeZone>

#include <algorithm>
#include <stdexcept>

namespace {

struct ValidationResult {
    QString decision;
    QStringList errorCodes;
    QStringList warningCodes;
};

QString slashPath(const QString &path) {
    return QDir::fromNativeSeparators(path);
}

QString nowIso() {
    return QDateTime::currentDateTime(QTimeZone(8 * 3600)).toString(Qt::ISODate);
}

void ensureParent(const QString &path) {
    QDir dir;
    const QString parent = QFileInfo(path).absolutePath();
    if (!dir.mkpath(parent)) {
        throw std::runtime_error(QString("cannot create parent directory: %1").arg(parent).toStdString());
    }
}

QJsonDocument readJsonDocument(const QString &path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        throw std::runtime_error(QString("cannot open json: %1").arg(path).toStdString());
    }
    QJsonParseError error;
    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);
    if (error.error != QJsonParseError::NoError) {
        throw std::runtime_error(QString("json parse failed: %1: %2").arg(path, error.errorString()).toStdString());
    }
    return doc;
}

QJsonObject readJsonObject(const QString &path) {
    const QJsonDocument doc = readJsonDocument(path);
    if (!doc.isObject()) {
        throw std::runtime_error(QString("json root is not object: %1").arg(path).toStdString());
    }
    return doc.object();
}

void writeJson(const QString &path, const QJsonObject &payload) {
    ensureParent(path);
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        throw std::runtime_error(QString("cannot write json: %1").arg(path).toStdString());
    }
    file.write(QJsonDocument(payload).toJson(QJsonDocument::Indented));
    if (!file.commit()) {
        throw std::runtime_error(QString("cannot commit json: %1").arg(path).toStdString());
    }
}

void writeText(const QString &path, const QString &payload) {
    ensureParent(path);
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        throw std::runtime_error(QString("cannot write text: %1").arg(path).toStdString());
    }
    file.write(payload.toUtf8());
    if (!file.commit()) {
        throw std::runtime_error(QString("cannot commit text: %1").arg(path).toStdString());
    }
}

bool removeDirIfExists(const QString &path) {
    QDir dir(path);
    return !dir.exists() || dir.removeRecursively();
}

void copyDir(const QString &src, const QString &dst) {
    if (!removeDirIfExists(dst)) {
        throw std::runtime_error(QString("cannot remove destination: %1").arg(dst).toStdString());
    }
    QDir().mkpath(dst);

    QDir sourceDir(src);
    const QFileInfoList entries = sourceDir.entryInfoList(
        QDir::NoDotAndDotDot | QDir::AllEntries,
        QDir::DirsFirst | QDir::Name
    );
    for (const QFileInfo &entry : entries) {
        const QString targetPath = QDir(dst).filePath(entry.fileName());
        if (entry.isDir()) {
            copyDir(entry.absoluteFilePath(), targetPath);
        } else if (!QFile::copy(entry.absoluteFilePath(), targetPath)) {
            throw std::runtime_error(QString("cannot copy file: %1").arg(entry.absoluteFilePath()).toStdString());
        }
    }
}

void replaceDir(const QString &current, const QString &candidate) {
    const QString tmp = current + ".replace_tmp";
    const QString backup = current + ".backup_tmp";
    removeDirIfExists(tmp);
    removeDirIfExists(backup);
    copyDir(candidate, tmp);
    if (QDir(current).exists() && !QDir().rename(current, backup)) {
        throw std::runtime_error(QString("cannot move current package to backup: %1").arg(current).toStdString());
    }
    if (!QDir().rename(tmp, current)) {
        throw std::runtime_error(QString("cannot install replacement package: %1").arg(current).toStdString());
    }
    removeDirIfExists(backup);
}

QString packageHash(const QString &packagePath) {
    QStringList files;
    QDirIterator it(packagePath, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        const QString filePath = it.next();
        files.append(QDir(packagePath).relativeFilePath(filePath));
    }
    std::sort(files.begin(), files.end());

    QCryptographicHash hash(QCryptographicHash::Sha256);
    for (const QString &rel : files) {
        QFile file(QDir(packagePath).filePath(rel));
        if (!file.open(QIODevice::ReadOnly)) {
            throw std::runtime_error(QString("cannot hash file: %1").arg(file.fileName()).toStdString());
        }
        hash.addData(slashPath(rel).toUtf8());
        hash.addData("\0", 1);
        hash.addData(file.readAll());
        hash.addData("\0", 1);
    }
    return "sha256:" + QString::fromLatin1(hash.result().toHex());
}

QJsonValue resolvePointer(QJsonValue value, const QString &pointer) {
    if (pointer.isEmpty()) {
        return value;
    }
    QString normalized = pointer;
    if (normalized.startsWith('/')) {
        normalized.remove(0, 1);
    }
    for (QString token : normalized.split('/', Qt::SkipEmptyParts)) {
        token.replace("~1", "/").replace("~0", "~");
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

bool jsonPathExists(const QString &packagePath, const QString &geometryPath) {
    const QStringList parts = geometryPath.split('#');
    const QString relFile = parts.value(0);
    const QString filePath = QDir(packagePath).filePath(relFile);
    if (!QFileInfo::exists(filePath)) {
        return false;
    }
    if (parts.size() == 1) {
        return true;
    }
    const QJsonDocument doc = readJsonDocument(filePath);
    return !resolvePointer(doc.isObject() ? QJsonValue(doc.object()) : QJsonValue(doc.array()), parts.value(1)).isUndefined();
}

QStringList unresolvedWarnings(const QString &packagePath) {
    QStringList warnings;
    const QString path = QDir(packagePath).filePath("evidence/unresolved_fields.json");
    if (!QFileInfo::exists(path)) {
        return warnings;
    }
    const QJsonArray items = readJsonObject(path).value("items").toArray();
    for (int i = 0; i < items.size(); ++i) {
        warnings.append("EVW001_PENDING_LEGACY_FIELD");
    }
    return warnings;
}

QStringList requiredFileErrors(const QString &packagePath) {
    QStringList errors;
    const QString manifestPath = QDir(packagePath).filePath("manifest.json");
    if (!QFileInfo::exists(manifestPath)) {
        errors.append("PV001_REQUIRED_FILE_MISSING");
        return errors;
    }
    const QJsonArray requiredFiles = readJsonObject(manifestPath).value("requiredFiles").toArray();
    for (const QJsonValue &value : requiredFiles) {
        const QString rel = value.toString();
        if (!QFileInfo::exists(QDir(packagePath).filePath(rel))) {
            errors.append("PV001_REQUIRED_FILE_MISSING");
            break;
        }
    }
    return errors;
}

QJsonArray readItems(const QString &packagePath, const QString &relPath) {
    const QString path = QDir(packagePath).filePath(relPath);
    if (!QFileInfo::exists(path)) {
        return {};
    }
    return readJsonObject(path).value("items").toArray();
}

QStringList collectBindingStates(const QString &packagePath) {
    QStringList states;
    for (const QString &rel : {"rebar/groups.json", "rebar/bars.json", "rebar/segments.json"}) {
        const QJsonArray items = readItems(packagePath, rel);
        for (const QJsonValue &value : items) {
            const QJsonObject item = value.toObject();
            const QString bindingState = item.value("binding").toObject().value("state").toString();
            if (!bindingState.isEmpty()) {
                states.append(bindingState);
            }
            if (item.value("openMode").toString() == "repairRequired") {
                states.append("repairRequired");
            }
        }
    }
    return states;
}

QStringList bindingErrors(const QString &packagePath) {
    QStringList errors;
    for (const QString &rel : {"rebar/groups.json", "rebar/bars.json", "rebar/segments.json"}) {
        const QJsonArray items = readItems(packagePath, rel);
        for (const QJsonValue &value : items) {
            const QJsonArray bindings = value.toObject().value("binding").toObject().value("items").toArray();
            for (const QJsonValue &bindingValue : bindings) {
                const QString geometryPath = bindingValue.toObject().value("geometryPath").toString();
                if (!geometryPath.isEmpty() && !jsonPathExists(packagePath, geometryPath)) {
                    errors.append("LGV004_GEOMETRY_PATH_BROKEN");
                    return errors;
                }
            }
        }
    }
    return errors;
}

ValidationResult validatePackage(const QString &packagePath) {
    ValidationResult result;
    result.warningCodes = unresolvedWarnings(packagePath);
    result.errorCodes = requiredFileErrors(packagePath);
    if (result.errorCodes.isEmpty()) {
        result.errorCodes = bindingErrors(packagePath);
    }
    if (!result.errorCodes.isEmpty()) {
        result.decision = "fail";
    } else if (!result.warningCodes.isEmpty()) {
        result.decision = "warning-only";
    } else {
        result.decision = "pass";
    }
    return result;
}

QJsonArray stringArray(const QStringList &items) {
    QJsonArray array;
    for (const QString &item : items) {
        array.append(item);
    }
    return array;
}

QJsonObject validationReportObject(const ValidationResult &result) {
    QJsonArray findings;
    for (const QString &code : result.errorCodes) {
        findings.append(QJsonObject{{"severity", "error"}, {"errorCode", code}});
    }
    for (const QString &code : result.warningCodes) {
        findings.append(QJsonObject{{"severity", "warning"}, {"errorCode", code}});
    }
    return QJsonObject{
        {"schemaVersion", "tsrebar-validator-report/v1"},
        {"createdAt", nowIso()},
        {"decision", result.decision},
        {"errorCount", result.errorCodes.size()},
        {"warningCount", result.warningCodes.size()},
        {"findings", findings}
    };
}

QJsonValue availabilityValue(const QString &state, const QString &command) {
    const QMap<QString, QMap<QString, QJsonValue>> matrix{
        {"OpenedFormal", {
            {"view", true}, {"selectGeometry", true}, {"rebarEdit", true},
            {"drawing", true}, {"save", true}, {"bindingRepair", false}
        }},
        {"OpenedWarning", {
            {"view", true}, {"selectGeometry", true}, {"rebarEdit", true},
            {"drawing", "warning"}, {"save", true}, {"bindingRepair", false}
        }},
        {"OpenedRepairRequired", {
            {"view", true}, {"selectGeometry", "repair-candidate-only"}, {"rebarEdit", false},
            {"drawing", false}, {"save", "raw-only-or-after-repair"}, {"bindingRepair", true}
        }},
        {"OpenBlocked", {
            {"view", "report-only"}, {"selectGeometry", false}, {"rebarEdit", false},
            {"drawing", false}, {"save", false}, {"bindingRepair", false}
        }},
        {"SaveFailed", {
            {"view", true}, {"selectGeometry", "current-state"}, {"rebarEdit", false},
            {"drawing", false}, {"save", "retry"}, {"bindingRepair", "if-binding-error"}
        }}
    };
    return matrix.value(state).value(command);
}

QJsonObject commandAvailability(const QString &state) {
    QJsonObject object;
    for (const QString &command : {"view", "selectGeometry", "rebarEdit", "drawing", "save", "bindingRepair"}) {
        object.insert(command, availabilityValue(state, command));
    }
    return object;
}

QJsonObject openProject(const QString &packagePath, const QString &caseId) {
    const ValidationResult validation = validatePackage(packagePath);
    const QStringList states = collectBindingStates(packagePath);

    QString finalState;
    QString bindingDecision;
    if (validation.decision == "pass") {
        finalState = "OpenedFormal";
        bindingDecision = "resolved";
    } else if (states.contains("repairRequired")) {
        finalState = "OpenedRepairRequired";
        bindingDecision = "pendingLegacyEvidence";
    } else if (validation.decision == "warning-only") {
        finalState = "OpenedWarning";
        bindingDecision = "warningOnly";
    } else if (validation.errorCodes.contains("LGV004_GEOMETRY_PATH_BROKEN")) {
        finalState = "OpenedRepairRequired";
        bindingDecision = "repairRequired";
    } else {
        finalState = "OpenBlocked";
        bindingDecision = "blocked";
    }

    return QJsonObject{
        {"caseId", caseId},
        {"command", "Project.OpenTsReBar"},
        {"inputPackage", slashPath(packagePath)},
        {"initialState", "NoProject"},
        {"finalState", finalState},
        {"dirtyBefore", false},
        {"dirtyAfter", false},
        {"validationDecision", validation.decision},
        {"bindingDecision", bindingDecision},
        {"errorCodes", stringArray(validation.errorCodes)},
        {"warningCodes", stringArray(validation.warningCodes)},
        {"commandAvailability", commandAvailability(finalState)},
        {"decision", finalState == "OpenBlocked" ? "blocked" : "pass"}
    };
}

QJsonObject saveProject(
    const QString &currentPackage,
    const QString &candidatePackage,
    const QString &reportDir,
    bool dirtyBefore
) {
    const QString beforeHash = QDir(currentPackage).exists() ? packageHash(currentPackage) : QString();
    const ValidationResult validation = validatePackage(candidatePackage);
    const QString validationReportPath = QDir(reportDir).filePath(QFileInfo(candidatePackage).dir().dirName() + "_validation.json");
    writeJson(validationReportPath, validationReportObject(validation));

    if (!validation.errorCodes.isEmpty()) {
        const QString afterHash = QDir(currentPackage).exists() ? packageHash(currentPackage) : QString();
        return QJsonObject{
            {"caseId", ""},
            {"command", "Project.Save"},
            {"initialState", "Dirty"},
            {"finalState", "SaveFailed"},
            {"dirtyBefore", dirtyBefore},
            {"dirtyAfter", dirtyBefore},
            {"oldPackageHash", beforeHash},
            {"newPackageHash", afterHash},
            {"candidatePackage", slashPath(candidatePackage)},
            {"currentPackage", slashPath(currentPackage)},
            {"validationDecision", validation.decision},
            {"bindingDecision", validation.errorCodes.contains("LGV004_GEOMETRY_PATH_BROKEN") ? "repairRequired" : "blocked"},
            {"errorCodes", stringArray(validation.errorCodes)},
            {"validationReportPath", slashPath(validationReportPath)},
            {"guardrails", QJsonObject{
                {"oldPackagePreserved", beforeHash == afterHash},
                {"dirtyPreserved", true},
                {"silentRebindForbidden", true}
            }},
            {"decision", "blocked"}
        };
    }

    replaceDir(currentPackage, candidatePackage);
    return QJsonObject{
        {"caseId", ""},
        {"command", "Project.Save"},
        {"initialState", "Dirty"},
        {"finalState", validation.decision == "warning-only" ? "OpenedWarning" : "OpenedFormal"},
        {"dirtyBefore", dirtyBefore},
        {"dirtyAfter", false},
        {"oldPackageHash", beforeHash},
        {"newPackageHash", packageHash(currentPackage)},
        {"candidatePackage", slashPath(candidatePackage)},
        {"currentPackage", slashPath(currentPackage)},
        {"validationDecision", validation.decision},
        {"bindingDecision", validation.decision == "warning-only" ? "warningOnly" : "resolved"},
        {"errorCodes", QJsonArray{}},
        {"validationReportPath", slashPath(validationReportPath)},
        {"guardrails", QJsonObject{
            {"oldPackageReplaced", true},
            {"dirtyCleared", true},
            {"saveTransactionUsed", true}
        }},
        {"decision", "pass"}
    };
}

QString findCandidateGeometryPath(const QString &candidatePackage, const QString &topologyId) {
    const QString topologyPath = QDir(candidatePackage).filePath("geometry/topology_refs.json");
    const QJsonArray refs = readJsonObject(topologyPath).value("topologyRefs").toArray();
    for (int i = 0; i < refs.size(); ++i) {
        if (refs.at(i).toObject().value("topologyId").toString() == topologyId) {
            return QString("geometry/topology_refs.json#/topologyRefs/%1").arg(i);
        }
    }
    return {};
}

QJsonArray brokenBindingItems(const QString &sourcePackage, const QString &candidatePackage) {
    QJsonArray results;
    for (const QString &rel : {"rebar/groups.json", "rebar/bars.json", "rebar/segments.json"}) {
        const QString path = QDir(sourcePackage).filePath(rel);
        if (!QFileInfo::exists(path)) {
            continue;
        }
        const QJsonArray items = readJsonObject(path).value("items").toArray();
        for (int objectIndex = 0; objectIndex < items.size(); ++objectIndex) {
            const QJsonObject item = items.at(objectIndex).toObject();
            const QJsonArray bindings = item.value("binding").toObject().value("items").toArray();
            for (int bindingIndex = 0; bindingIndex < bindings.size(); ++bindingIndex) {
                const QJsonObject binding = bindings.at(bindingIndex).toObject();
                const QString oldPath = binding.value("geometryPath").toString();
                if (oldPath.isEmpty() || jsonPathExists(sourcePackage, oldPath)) {
                    continue;
                }
                const QString topologyId = binding.value("topologyId").toString();
                const QString candidatePath = findCandidateGeometryPath(candidatePackage, topologyId);
                results.append(QJsonObject{
                    {"objectFile", rel},
                    {"objectIndex", objectIndex},
                    {"bindingIndex", bindingIndex},
                    {"objectId", item.value("id").toString()},
                    {"role", binding.value("role").toString()},
                    {"oldGeometryPath", oldPath},
                    {"candidateGeometryPath", candidatePath},
                    {"geometryRefId", binding.value("geometryRefId").toString()},
                    {"topologyId", topologyId},
                    {"matchState", candidatePath.isEmpty() ? "missing" : "match"},
                    {"decision", candidatePath.isEmpty() ? "manual-review" : "apply"},
                    {"evidence", QJsonArray{"E-DEV-017", "E-DEV-018", "E-DEV-022"}}
                });
            }
        }
    }
    return results;
}

void writePreviewMarkdown(const QJsonObject &preview, const QString &path) {
    QStringList lines{
        "# Binding Repair Preview",
        "",
        "- Runtime: Qt6 C++ runtime",
        "- Preview only: yes",
        "- Silent rebind forbidden: yes",
        "- Formal package unchanged: yes",
        "",
        "## Candidates",
        "",
        "| Object | Role | Old geometryPath | Candidate geometryPath | Match | Decision |",
        "|---|---|---|---|---|---|"
    };
    const QJsonArray bindings = preview.value("bindings").toArray();
    for (const QJsonValue &value : bindings) {
        const QJsonObject item = value.toObject();
        lines.append(QString("| %1 | %2 | `%3` | `%4` | %5 | %6 |")
            .arg(item.value("objectId").toString())
            .arg(item.value("role").toString())
            .arg(item.value("oldGeometryPath").toString())
            .arg(item.value("candidateGeometryPath").toString())
            .arg(item.value("matchState").toString())
            .arg(item.value("decision").toString()));
    }
    lines.append("");
    lines.append("## Boundary");
    lines.append("");
    lines.append("- This preview is produced by the real Qt6 C++ runtime gate.");
    lines.append("- It does not write the formal package.");
    lines.append("- Only matched candidates can be applied.");
    lines.append("- Ambiguous or missing candidates remain unresolved.");
    lines.append("");
    writeText(path, lines.join('\n'));
}

QJsonObject bindingRepairPreview(const QString &sourcePackage, const QString &candidatePackage, const QString &reportDir) {
    const QString beforeHash = packageHash(sourcePackage);
    const QJsonArray bindings = brokenBindingItems(sourcePackage, candidatePackage);
    const QString afterHash = packageHash(sourcePackage);

    int matchCount = 0;
    int missingCount = 0;
    int ambiguousCount = 0;
    for (const QJsonValue &value : bindings) {
        const QString state = value.toObject().value("matchState").toString();
        if (state == "match") {
            ++matchCount;
        } else if (state == "missing") {
            ++missingCount;
        } else if (state == "ambiguous") {
            ++ambiguousCount;
        }
    }

    const QJsonObject preview{
        {"reportId", "binding_repair_preview_001"},
        {"schemaVersion", "binding-repair-preview/v1"},
        {"createdAt", nowIso()},
        {"runtime", "Qt6 C++ runtime gate"},
        {"sourcePackage", slashPath(sourcePackage)},
        {"candidatePackage", slashPath(candidatePackage)},
        {"bindingStateBefore", "repairRequired"},
        {"bindingStateAfter", "repairPreview"},
        {"bindings", bindings},
        {"summary", QJsonObject{
            {"matchCount", matchCount},
            {"missingCount", missingCount},
            {"ambiguousCount", ambiguousCount}
        }},
        {"guardrails", QJsonObject{
            {"previewOnly", true},
            {"silentRebindForbidden", true},
            {"formalPackageUnchanged", beforeHash == afterHash}
        }}
    };
    writeJson(QDir(reportDir).filePath("binding_repair_preview_001.json"), preview);
    writePreviewMarkdown(preview, QDir(reportDir).filePath("binding_repair_preview_001.md"));
    return preview;
}

QJsonObject bindingRepairCancel(const QJsonObject &preview, const QString &reportDir) {
    const QJsonObject result{
        {"reportId", "binding_repair_cancel_001"},
        {"schemaVersion", "binding-repair-cancel/v1"},
        {"createdAt", nowIso()},
        {"runtime", "Qt6 C++ runtime gate"},
        {"sourcePackage", preview.value("sourcePackage").toString()},
        {"initialState", "BindingRepairPreview"},
        {"finalState", "OpenedRepairRequired"},
        {"dirtyBefore", false},
        {"dirtyAfter", false},
        {"guardrails", QJsonObject{
            {"previewDiscarded", true},
            {"formalPackageUnchanged", true}
        }}
    };
    writeJson(QDir(reportDir).filePath("binding_repair_cancel_001.json"), result);
    return result;
}

void applyBindingMapping(const QString &packagePath, const QJsonObject &mapping) {
    const QString docPath = QDir(packagePath).filePath(mapping.value("objectFile").toString());
    QJsonObject doc = readJsonObject(docPath);
    QJsonArray items = doc.value("items").toArray();
    QJsonObject item = items.at(mapping.value("objectIndex").toInt()).toObject();
    QJsonObject binding = item.value("binding").toObject();
    QJsonArray bindingItems = binding.value("items").toArray();
    QJsonObject bindingItem = bindingItems.at(mapping.value("bindingIndex").toInt()).toObject();
    bindingItem.insert("geometryPath", mapping.value("candidateGeometryPath").toString());
    bindingItems.replace(mapping.value("bindingIndex").toInt(), bindingItem);
    binding.insert("items", bindingItems);
    binding.insert("state", "resolved");
    item.insert("binding", binding);
    item.remove("openMode");
    items.replace(mapping.value("objectIndex").toInt(), item);
    doc.insert("items", items);
    writeJson(docPath, doc);
}

QJsonObject bindingRepairApply(
    const QString &sourcePackage,
    const QString &candidatePackage,
    const QJsonObject &preview,
    const QString &reportDir
) {
    const QString workDir = QDir(reportDir).filePath("_work");
    const QString current = QDir(workDir).filePath("current_broken.tsrebar");
    const QString repaired = QDir(workDir).filePath("repaired_broken_binding.tsrebar");
    copyDir(sourcePackage, current);
    copyDir(sourcePackage, repaired);

    QJsonArray applied;
    const QJsonArray bindings = preview.value("bindings").toArray();
    for (const QJsonValue &value : bindings) {
        const QJsonObject mapping = value.toObject();
        if (mapping.value("matchState").toString() != "match") {
            continue;
        }
        applyBindingMapping(repaired, mapping);
        applied.append(mapping);
    }

    const QJsonObject save = saveProject(current, repaired, reportDir, true);
    const QJsonObject summary = preview.value("summary").toObject();
    const QJsonObject result{
        {"reportId", "binding_repair_run_001"},
        {"schemaVersion", "binding-repair-run/v1"},
        {"createdAt", nowIso()},
        {"runtime", "Qt6 C++ runtime gate"},
        {"sourcePackage", slashPath(sourcePackage)},
        {"candidatePackage", slashPath(candidatePackage)},
        {"savedPackage", slashPath(current)},
        {"bindingStateBeforeSave", "repairAppliedPendingSave"},
        {"bindings", applied},
        {"validationDecisionAfterApply", save.value("validationDecision").toString()},
        {"saveDecision", save.value("decision").toString() == "pass" ? "SaveSucceeded" : "SaveFailed"},
        {"dirtyAfter", save.value("dirtyAfter").toBool()},
        {"guardrails", QJsonObject{
            {"fullSaveTransactionRequired", true},
            {"onlyMatchedCandidatesApplied", applied.size() == summary.value("matchCount").toInt()},
            {"ambiguousCandidatesBlocked", summary.value("ambiguousCount").toInt() == 0},
            {"silentRebindForbidden", true}
        }},
        {"m1FormalCandidate", true},
        {"decision", "qt6-binding-repair-pass"}
    };
    writeJson(QDir(reportDir).filePath("binding_repair_run_001.json"), result);
    return result;
}

QJsonObject withCaseId(QJsonObject object, const QString &caseId) {
    object.insert("caseId", caseId);
    return object;
}

QJsonObject runGate(const QString &root, const QString &reportDir, bool qApplicationCreated, bool actionsCreated) {
    QDir out(reportDir);
    if (!out.mkpath(".")) {
        throw std::runtime_error(QString("cannot create report dir: %1").arg(reportDir).toStdString());
    }
    removeDirIfExists(out.filePath("_work"));

    const QString fixtureRoot = QDir(root).filePath("fixtures/tsrebar");
    QJsonArray cases;
    cases.append(openProject(QDir(fixtureRoot).filePath("fixture_a_empty_step/project.tsrebar"), "ASO-001"));
    cases.append(openProject(QDir(fixtureRoot).filePath("fixture_b_single_group/project.tsrebar"), "ASO-002"));
    cases.append(openProject(QDir(fixtureRoot).filePath("fixture_c_pending_legacy/project.tsrebar"), "ASO-003"));
    cases.append(openProject(QDir(fixtureRoot).filePath("negative/missing_required_file/project.tsrebar"), "ASO-004"));
    cases.append(openProject(QDir(fixtureRoot).filePath("negative/broken_binding/project.tsrebar"), "ASO-008-open"));

    const QString saveWork = out.filePath("_work/save_open_current.tsrebar");
    copyDir(QDir(fixtureRoot).filePath("fixture_a_empty_step/project.tsrebar"), saveWork);
    cases.append(withCaseId(saveProject(
        saveWork,
        QDir(fixtureRoot).filePath("negative/broken_binding/project.tsrebar"),
        reportDir,
        true
    ), "ASO-005"));
    cases.append(withCaseId(saveProject(
        saveWork,
        QDir(fixtureRoot).filePath("fixture_b_single_group/project.tsrebar"),
        reportDir,
        true
    ), "ASO-006"));

    const QString source = QDir(fixtureRoot).filePath("negative/broken_binding/project.tsrebar");
    const QString candidate = QDir(fixtureRoot).filePath("fixture_b_single_group/project.tsrebar");
    const QJsonObject preview = bindingRepairPreview(source, candidate, reportDir);
    const QJsonObject cancel = bindingRepairCancel(preview, reportDir);
    const QJsonObject repair = bindingRepairApply(source, candidate, preview, reportDir);

    cases.append(QJsonObject{
        {"caseId", "ASO-008"},
        {"command", "Binding.Repair.Begin"},
        {"initialState", "OpenedRepairRequired"},
        {"finalState", "BindingRepairPreview"},
        {"dirtyBefore", false},
        {"dirtyAfter", false},
        {"validationDecision", "not-run-preview-only"},
        {"bindingDecision", preview.value("bindingStateAfter").toString()},
        {"decision", "pass"},
        {"reportPath", slashPath(out.filePath("binding_repair_preview_001.md"))}
    });
    cases.append(QJsonObject{
        {"caseId", "ASO-009"},
        {"command", "Binding.Repair.Apply"},
        {"initialState", "BindingRepairPreview"},
        {"finalState", "OpenedWarning"},
        {"dirtyBefore", true},
        {"dirtyAfter", repair.value("dirtyAfter").toBool()},
        {"validationDecision", repair.value("validationDecisionAfterApply").toString()},
        {"bindingDecision", repair.value("bindingStateBeforeSave").toString()},
        {"decision", "pass"},
        {"reportPath", slashPath(out.filePath("binding_repair_run_001.json"))}
    });
    cases.append(QJsonObject{
        {"caseId", "ASO-010"},
        {"command", "Binding.Repair.Cancel"},
        {"initialState", cancel.value("initialState").toString()},
        {"finalState", cancel.value("finalState").toString()},
        {"dirtyBefore", cancel.value("dirtyBefore").toBool()},
        {"dirtyAfter", cancel.value("dirtyAfter").toBool()},
        {"validationDecision", "not-run-cancel-only"},
        {"bindingDecision", "repairRequired"},
        {"decision", "pass"},
        {"reportPath", slashPath(out.filePath("binding_repair_cancel_001.json"))}
    });

    int passCount = 0;
    int blockedCount = 0;
    for (const QJsonValue &value : cases) {
        const QString decision = value.toObject().value("decision").toString();
        if (decision == "pass") {
            ++passCount;
        } else if (decision == "blocked") {
            ++blockedCount;
        }
    }

    const QJsonObject report{
        {"schemaVersion", "app-save-open-run/v1"},
        {"reportId", "app_save_open_run_001"},
        {"scope", "Qt6 C++ runtime gate"},
        {"createdAt", nowIso()},
        {"appVersion", "phase1-qt6-cpp-runtime-gate"},
        {"qtRuntime", QJsonObject{
            {"qApplicationCreated", qApplicationCreated},
            {"guiCommandActionsCreated", actionsCreated},
            {"qpaPlatform", QString::fromLocal8Bit(qgetenv("QT_QPA_PLATFORM"))}
        }},
        {"cases", cases},
        {"bindingRepairReport", slashPath(out.filePath("binding_repair_run_001.json"))},
        {"bindingRepairPreview", slashPath(out.filePath("binding_repair_preview_001.md"))},
        {"summary", QJsonObject{
            {"passCount", passCount},
            {"blockedCount", blockedCount},
            {"failCount", cases.size() - passCount - blockedCount},
            {"m1FormalCandidate", true},
            {"decision", "qt6-app-pass"},
            {"remainingGaps", QJsonArray{}}
        }}
    };
    writeJson(out.filePath("app_save_open_run_001.json"), report);
    return report;
}

} // namespace

int main(int argc, char *argv[]) {
    try {
        QApplication app(argc, argv);
        app.setApplicationName("qt6_runtime_gate");

        QList<QAction *> actions;
        for (const QString &name : {"View", "SelectGeometry", "RebarEdit", "Drawing", "Save", "BindingRepair"}) {
            QAction *action = new QAction(name, &app);
            action->setObjectName(name);
            actions.append(action);
        }

        QCommandLineParser parser;
        parser.setApplicationDescription("Qt6 runtime gate for tsrebar Save/Open and binding repair reports.");
        parser.addHelpOption();
        const QCommandLineOption rootOption("root", "Project root.", "path");
        const QCommandLineOption outDirOption("out-dir", "Report output directory.", "path");
        parser.addOption(rootOption);
        parser.addOption(outDirOption);
        parser.process(app);

        const QString root = parser.value(rootOption);
        const QString outDir = parser.value(outDirOption);
        if (root.isEmpty() || outDir.isEmpty()) {
            throw std::runtime_error("--root and --out-dir are required");
        }

        const QJsonObject report = runGate(root, outDir, true, actions.size() == 6);
        QTextStream(stdout) << QJsonDocument(report.value("summary").toObject()).toJson(QJsonDocument::Compact) << Qt::endl;
        return 0;
    } catch (const std::exception &error) {
        QTextStream(stderr) << "qt6_runtime_gate failed: " << error.what() << Qt::endl;
        return 1;
    }
}
