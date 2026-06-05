#pragma once

#include <QWidget>

class QLabel;

class ModelTreePanel final : public QWidget
{
    Q_OBJECT

public:
    explicit ModelTreePanel(QWidget* parent = nullptr);

    void setImportSummary(const QString& fileName,
                          const QString& unit,
                          int solids,
                          int faces,
                          int edges,
                          int vertices);

private:
    QLabel* m_summary = nullptr;
};

