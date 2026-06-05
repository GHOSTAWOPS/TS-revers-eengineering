#pragma once

#include "geometry/occ/import/OcctImportedDocument.h"
#include "geometry/legacy/LegacySelectionRef.h"
#include "geometry/occ/selection/OccSelectionIndex.h"

#include <QWidget>

#include <AIS_InteractiveContext.hxx>
#include <Aspect_NeutralWindow.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>

#include <optional>
#include <string>

class QPaintEngine;
class QPaintEvent;
class QMouseEvent;
class QResizeEvent;
class Aspect_DisplayConnection;

namespace tsrebar {

class OccViewerWidget final : public QWidget
{
    Q_OBJECT

public:
    explicit OccViewerWidget(QWidget* parent = nullptr);
    ~OccViewerWidget() override;

    [[nodiscard]] bool displayDocument(const OcctImportedDocument& document,
                                       QString* errorMessage = nullptr);
    [[nodiscard]] bool fitAll(QString* errorMessage = nullptr);
    void setSelectionMode(TopAbs_ShapeEnum shapeType);
    [[nodiscard]] TopAbs_ShapeEnum selectionMode() const;
    [[nodiscard]] bool selectByStableId(const QString& stableId,
                                        QString* errorMessage = nullptr);
    [[nodiscard]] std::string currentSelectionStableId() const;
    [[nodiscard]] QString currentSelectionSummary() const;
    [[nodiscard]] QString currentSelectionDetails() const;

    [[nodiscard]] bool hasViewer() const;
    [[nodiscard]] int displayedShapeCount() const;

signals:
    void selectionChanged(const QString& summary,
                          const QString& details,
                          const QString& stableId);

protected:
    QPaintEngine* paintEngine() const override;
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    [[nodiscard]] bool ensureViewer(QString* errorMessage = nullptr);
    [[nodiscard]] bool syncWindow(QString* errorMessage = nullptr);
    [[nodiscard]] bool initializeWindow(QString* errorMessage = nullptr);
    [[nodiscard]] bool updateSelectionFromShape(const TopoDS_Shape& selectedShape,
                                                QString* errorMessage = nullptr);
    [[nodiscard]] QString shapeKindLabel(LegacyShapeKind kind) const;
    [[nodiscard]] QString selectionSummary(const LegacySelectionRef& ref) const;
    [[nodiscard]] QString selectionDetails(const LegacySelectionRef& ref) const;
    void applyCurrentSelection(const LegacySelectionRef& ref);
    void clearCurrentSelection(bool notify = true);
    void resetViewer();
    void redraw();

    Handle(Aspect_DisplayConnection) m_displayConnection;
    Handle(OpenGl_GraphicDriver) m_graphicDriver;
    Handle(V3d_Viewer) m_viewer;
    Handle(AIS_InteractiveContext) m_context;
    Handle(V3d_View) m_view;
    Handle(Aspect_NeutralWindow) m_window;
    bool m_windowReady = false;
    int m_displayedShapeCount = 0;
    TopAbs_ShapeEnum m_selectionMode = TopAbs_SHAPE;
    OccSelectionIndex m_selectionIndex;
    std::optional<LegacySelectionRef> m_currentSelection;
    QString m_currentSelectionSummary;
    QString m_currentSelectionDetails;
};

} // namespace tsrebar
