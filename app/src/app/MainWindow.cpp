#include "app/MainWindow.h"

#include "command/LegacyUiCommandMap.h"
#include "geometry/occ/import/OcctStepImportService.h"
#include "presentation/occ/OccViewerWidget.h"
#include "ui/MainTabs.h"
#include "ui/ModelTreePanel.h"

#include <QAction>
#include <QDockWidget>
#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QLabel>
#include <QStatusBar>
#include <QTabWidget>
#include <QToolBar>
#include <QVBoxLayout>

namespace {

QString commandStatusText(const tsrebar::CommandResult& result)
{
    switch (result.status) {
    case tsrebar::CommandStatus::Completed:
        return QStringLiteral("完成：%1").arg(result.message);
    case tsrebar::CommandStatus::NotImplemented:
        return QStringLiteral("未实现：%1").arg(result.message);
    case tsrebar::CommandStatus::Failed:
        return QStringLiteral("失败：%1").arg(result.message);
    }
    return QStringLiteral("未知命令状态");
}

} // namespace

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    m_commands.registerHandler(tsrebar::CommandId::ImportStep, [this]() {
        const QString path = QFileDialog::getOpenFileName(
            this,
            QStringLiteral("导入 STEP"),
            QString(),
            QStringLiteral("STEP 文件 (*.stp *.step)"));

        if (path.isEmpty()) {
            return tsrebar::CommandResult{tsrebar::CommandStatus::Failed,
                                          QStringLiteral("导入已取消")};
        }

        const tsrebar::OcctImportedDocument document =
            tsrebar::OcctStepImportService().importDocument(path);
        if (!document.summary.ok) {
            return tsrebar::CommandResult{
                tsrebar::CommandStatus::Failed,
                QString::fromStdString(document.summary.error)};
        }

        QString displayError;
        if (!m_viewer->displayDocument(document, &displayError)) {
            return tsrebar::CommandResult{
                tsrebar::CommandStatus::Failed,
                displayError};
        }

        m_modelTree->setImportSummary(QFileInfo(path).fileName(),
                                      QString::fromStdString(document.summary.lengthUnit),
                                      document.summary.solids,
                                      document.summary.faces,
                                      document.summary.edges,
                                      document.summary.vertices);

        return tsrebar::CommandResult{
            tsrebar::CommandStatus::Completed,
            QStringLiteral("%1: solids=%2, faces=%3, unit=%4")
                .arg(QFileInfo(path).fileName())
                .arg(document.summary.solids)
                .arg(document.summary.faces)
                .arg(QString::fromStdString(document.summary.lengthUnit))};
    });

    m_commands.registerHandler(tsrebar::CommandId::ViewFitAll, [this]() {
        QString error;
        if (!m_viewer->fitAll(&error)) {
            return tsrebar::CommandResult{tsrebar::CommandStatus::Failed, error};
        }
        return tsrebar::CommandResult{tsrebar::CommandStatus::Completed,
                                      QStringLiteral("全显")};
    });

    m_commands.registerHandler(tsrebar::CommandId::ViewSelectFace, [this]() {
        m_viewer->setSelectionMode(TopAbs_FACE);
        return tsrebar::CommandResult{tsrebar::CommandStatus::Completed,
                                      QStringLiteral("选择模式：面")};
    });

    m_commands.registerHandler(tsrebar::CommandId::ViewSelectEdge, [this]() {
        m_viewer->setSelectionMode(TopAbs_EDGE);
        return tsrebar::CommandResult{tsrebar::CommandStatus::Completed,
                                      QStringLiteral("选择模式：边")};
    });

    m_commands.registerHandler(tsrebar::CommandId::ViewSelectVertex, [this]() {
        m_viewer->setSelectionMode(TopAbs_VERTEX);
        return tsrebar::CommandResult{tsrebar::CommandStatus::Completed,
                                      QStringLiteral("选择模式：点")};
    });

    buildUi();
    buildCommandTabs();
    statusBar()->showMessage(QStringLiteral("就绪"));
}

void MainWindow::buildUi()
{
    setWindowTitle(QStringLiteral("图石钢筋 1:1 复刻"));

    auto* central = new QWidget(this);
    auto* layout = new QVBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);

    m_tabs = new QTabWidget(central);
    layout->addWidget(m_tabs);

    m_viewer = new tsrebar::OccViewerWidget(central);
    layout->addWidget(m_viewer, 1);

    setCentralWidget(central);

    m_modelTree = new ModelTreePanel(this);
    auto* modelDock = new QDockWidget(QStringLiteral("模型树"), this);
    modelDock->setObjectName(QStringLiteral("dock_model_tree"));
    modelDock->setWidget(m_modelTree);
    addDockWidget(Qt::LeftDockWidgetArea, modelDock);

    auto* propertyDock = new QDockWidget(QStringLiteral("属性"), this);
    propertyDock->setObjectName(QStringLiteral("dock_properties"));
    auto* propertyLabel = new QLabel(QStringLiteral("选择对象后显示属性"), propertyDock);
    propertyLabel->setAlignment(Qt::AlignCenter);
    propertyLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    propertyDock->setWidget(propertyLabel);
    addDockWidget(Qt::RightDockWidgetArea, propertyDock);

    connect(m_viewer,
            &tsrebar::OccViewerWidget::selectionChanged,
            this,
            [this, propertyLabel](const QString& summary,
                                  const QString& details,
                                  const QString& stableId) {
                if (stableId.isEmpty()) {
                    propertyLabel->setText(QStringLiteral("选择对象后显示属性"));
                    statusBar()->showMessage(QStringLiteral("未选中对象"), 2000);
                    return;
                }
                propertyLabel->setText(details);
                statusBar()->showMessage(QStringLiteral("已选择：%1").arg(summary), 3000);
            });
}

void MainWindow::buildCommandTabs()
{
    const auto tabs = tsrebar::mainTabDefinitions();
    const auto commands = tsrebar::legacyUiCommands();

    for (const auto& tab : tabs) {
        auto* page = new QWidget(m_tabs);
        auto* pageLayout = new QHBoxLayout(page);
        pageLayout->setContentsMargins(8, 8, 8, 8);
        pageLayout->setSpacing(8);

        for (const auto& group : tab.groups) {
            auto* toolbar = new QToolBar(group.title, page);
            toolbar->setObjectName(QStringLiteral("toolbar_%1_%2")
                                       .arg(tab.objectName, group.objectName));
            toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

            for (const auto& command : commands) {
                if (command.tabObjectName != tab.objectName ||
                    command.groupObjectName != group.objectName) {
                    continue;
                }

                auto* action = new QAction(command.caption, toolbar);
                action->setObjectName(command.objectName);
                connect(action, &QAction::triggered, this, [this, id = command.id]() {
                    executeCommand(id);
                });
                toolbar->addAction(action);
            }

            pageLayout->addWidget(toolbar);
        }

        pageLayout->addStretch(1);
        m_tabs->addTab(page, tab.title);
    }
}

void MainWindow::executeCommand(tsrebar::CommandId id)
{
    const tsrebar::CommandResult result = m_commands.execute(id);
    statusBar()->showMessage(commandStatusText(result), 5000);
}
