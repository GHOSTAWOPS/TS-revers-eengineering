#include "ui/ModelTreePanel.h"

#include <QLabel>
#include <QVBoxLayout>

ModelTreePanel::ModelTreePanel(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);

    m_summary = new QLabel(QStringLiteral("未导入模型"), this);
    m_summary->setWordWrap(true);
    layout->addWidget(m_summary);
    layout->addStretch(1);
}

void ModelTreePanel::setImportSummary(const QString& fileName,
                                      const QString& unit,
                                      int solids,
                                      int faces,
                                      int edges,
                                      int vertices)
{
    m_summary->setText(QStringLiteral("%1\nsolids: %2\nfaces: %3\nedges: %4\nvertices: %5\nunit: %6")
                           .arg(fileName)
                           .arg(solids)
                           .arg(faces)
                           .arg(edges)
                           .arg(vertices)
                           .arg(unit));
}

