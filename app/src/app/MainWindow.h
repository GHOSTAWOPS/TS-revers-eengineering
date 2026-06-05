#pragma once

#include "command/CommandId.h"
#include "command/CommandRegistry.h"

#include <QMainWindow>

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

private:
    void buildUi();
    void buildCommandTabs();
    void executeCommand(tsrebar::CommandId id);

    tsrebar::CommandRegistry m_commands;
    QTabWidget* m_tabs = nullptr;
    ModelTreePanel* m_modelTree = nullptr;
    tsrebar::OccViewerWidget* m_viewer = nullptr;
};
