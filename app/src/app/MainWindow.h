#pragma once

#include "command/CommandId.h"
#include "command/CommandRegistry.h"
#include "command/RebarLineGroupCommandHandler.h"
#include "project/runtime/TsRebarProjectRuntime.h"

#include <QMainWindow>
#include <QString>

#include <memory>
#include <optional>

class QAction;
class QTabWidget;
class ModelTreePanel;

namespace tsrebar {
class OccViewerWidget;
}

class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    struct DirtyState
    {
        bool projectDirty = false;
        bool geometryDirty = false;
        bool rebarDirty = false;
        bool drawingDirty = false;
        bool selectionDirty = false;
        bool viewDirty = false;
        int committedTransactionCount = 0;
        QString lastDirtyCommand;
        QString legacyDirtyEvidenceId;
        QString unresolvedDirtyParityGap;
    };

    explicit MainWindow(QWidget* parent = nullptr);
    bool verifyLegacyUiActionMetadata(QString* errorMessage = nullptr) const;
    [[nodiscard]] const tsrebar::SteelData& steelDataForInspection() const;
    [[nodiscard]] const DirtyState& dirtyStateForInspection() const;
    void setProjectPackagePathForInspection(const QString& path);
    [[nodiscard]] const std::optional<tsrebar::TsRebarProjectSaveResult>&
    lastSaveResultForInspection() const;

private:
    void buildUi();
    void buildCommandTabs();
    void registerCommandHandlers();
    void executeCommand(tsrebar::CommandId id);
    void applyDirtyStateFromCommandResult(const tsrebar::CommandResult& result);
    void applyDirtyStateFromSaveResult(const tsrebar::TsRebarProjectSaveResult& result);
    [[nodiscard]] tsrebar::TsRebarProjectSnapshot currentProjectSnapshotForSave() const;
    [[nodiscard]] bool hasPersistentDirtyState() const;
    [[nodiscard]] bool lineGroupSelectionPreflightForCommand(QString* errorMessage = nullptr) const;
    [[nodiscard]] bool configureLineGroupParametersForCommand();
    [[nodiscard]] tsrebar::RebarLineGroupCommandParameters nextLineGroupParameters();
    [[nodiscard]] bool displayCreatedLineGroup(QString* errorMessage = nullptr);

    tsrebar::CommandRegistry m_commands;
    tsrebar::SteelData m_steelData;
    tsrebar::TsRebarProjectRuntime m_projectRuntime;
    DirtyState m_dirtyState;
    QString m_projectPackagePath;
    std::optional<tsrebar::TsRebarProjectSaveResult> m_lastSaveResult;
    tsrebar::RebarLineGroupCommandParameters m_lineGroupParameters;
    int m_lineGroupCreateSequence = 0;
    std::shared_ptr<tsrebar::LegacyRebarGeometryReader> m_lineGroupGeometryReader;
    std::shared_ptr<tsrebar::RebarLineGroupCommandHandler> m_lineGroupHandler;
    QTabWidget* m_tabs = nullptr;
    ModelTreePanel* m_modelTree = nullptr;
    tsrebar::OccViewerWidget* m_viewer = nullptr;
};
