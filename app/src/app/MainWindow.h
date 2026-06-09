#pragma once

#include "command/CommandId.h"
#include "command/CommandRegistry.h"
#include "command/RebarLineGroupCommandHandler.h"

#include <QMainWindow>

#include <memory>

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
    explicit MainWindow(QWidget* parent = nullptr);
    bool verifyLegacyUiActionMetadata(QString* errorMessage = nullptr) const;

private:
    void buildUi();
    void buildCommandTabs();
    void registerCommandHandlers();
    void executeCommand(tsrebar::CommandId id);

    tsrebar::CommandRegistry m_commands;
    tsrebar::SteelData m_steelData;
    tsrebar::RebarLineGroupCommandParameters m_lineGroupParameters;
    std::shared_ptr<tsrebar::LegacyRebarGeometryReader> m_lineGroupGeometryReader;
    std::shared_ptr<tsrebar::RebarLineGroupCommandHandler> m_lineGroupHandler;
    QTabWidget* m_tabs = nullptr;
    ModelTreePanel* m_modelTree = nullptr;
    tsrebar::OccViewerWidget* m_viewer = nullptr;
};
