#pragma once

#include "domain/rebar/SteelData.h"

#include <QString>
#include <QStringList>

#include <string>
#include <vector>

namespace tsrebar {

enum class ProjectDocumentState {
    NoProject,
    OpenedFormal,
    OpenedWarning,
    OpenedRepairRequired,
    OpenBlocked,
    Dirty,
    Saving,
    SaveFailed,
};

struct TsRebarSourceStep
{
    std::string sourceStepId;
    std::string path;
    std::string originalPath;
    std::string sha256;
};

struct TsRebarTopologyRef
{
    std::string topologyId;
    std::string shapeType;
    std::string sourceStepId;
    std::string fingerprint;
    std::vector<std::string> evidence;
};

struct TsRebarProjectSnapshot
{
    std::string projectId;
    std::string projectName;
    TsRebarSourceStep sourceStep;
    std::vector<TsRebarTopologyRef> topologyRefs;
    SteelData steelData;
    std::vector<std::string> evidenceIds;
};

struct TsRebarValidationSummary
{
    QString decision;
    QStringList errorCodes;
    QStringList warningCodes;
    int errorCount = 0;
    int warningCount = 0;
};

struct TsRebarProjectOpenResult
{
    bool ok = false;
    ProjectDocumentState finalState = ProjectDocumentState::NoProject;
    QString bindingDecision;
    TsRebarValidationSummary validation;
    TsRebarProjectSnapshot snapshot;
};

struct TsRebarProjectSaveResult
{
    bool ok = false;
    ProjectDocumentState finalState = ProjectDocumentState::Dirty;
    bool dirtyBefore = false;
    bool dirtyAfter = true;
    QString oldPackageHash;
    QString newPackageHash;
    QString bindingDecision;
    QString candidatePackagePath;
    TsRebarValidationSummary validation;
};

class TsRebarProjectRuntime
{
public:
    [[nodiscard]] TsRebarProjectSaveResult saveSnapshot(
        const QString& packagePath,
        const TsRebarProjectSnapshot& snapshot,
        bool dirtyBefore) const;

    [[nodiscard]] TsRebarProjectOpenResult open(const QString& packagePath) const;

    [[nodiscard]] QString packageHash(const QString& packagePath) const;
};

} // namespace tsrebar
