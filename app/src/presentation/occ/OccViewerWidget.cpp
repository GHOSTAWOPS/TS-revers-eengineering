#include "presentation/occ/OccViewerWidget.h"

#include "presentation/occ/RebarAisPresentationAdapter.h"

#include <AIS_Shape.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <Prs3d_Drawer.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Failure.hxx>
#include <StdSelect_BRepOwner.hxx>

#include <QMouseEvent>
#include <QPaintEvent>
#include <QResizeEvent>

#include <cmath>

namespace {

QString failureMessage(const QString& operation, const Standard_Failure& failure)
{
    const QString reason = QString::fromUtf8(failure.GetMessageString()).trimmed();
    if (reason.isEmpty()) {
        return QStringLiteral("%1失败。").arg(operation);
    }
    return QStringLiteral("%1失败：%2").arg(operation, reason);
}

} // namespace

namespace tsrebar {

OccViewerWidget::OccViewerWidget(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_NativeWindow, true);
    setAttribute(Qt::WA_PaintOnScreen, true);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setMouseTracking(true);
    setMinimumSize(320, 240);
}

OccViewerWidget::~OccViewerWidget()
{
    resetViewer();
}

bool OccViewerWidget::displayDocument(const OcctImportedDocument& document,
                                      QString* errorMessage)
{
    if (!document.isSuccess()) {
        if (errorMessage != nullptr) {
            *errorMessage = QString::fromStdString(document.summary.error);
            if (errorMessage->isEmpty()) {
                *errorMessage = QStringLiteral("导入文档为空。");
            }
        }
        return false;
    }

    try {
        if (!ensureViewer(errorMessage) || !initializeWindow(errorMessage)) {
            return false;
        }

        m_context->RemoveAll(Standard_False);
        m_displayedShapeCount = 0;
        m_displayedRebarShapeCount = 0;
        m_lastDisplayedRebarGroupId.clear();
        m_selectionIndex = {};
        clearCurrentSelection();

        for (const OcctImportedPart& part : document.parts) {
            if (part.shape.IsNull()) {
                continue;
            }
            Handle(AIS_Shape) aisShape = new AIS_Shape(part.shape);
            m_context->Display(aisShape, Standard_False);
            ++m_displayedShapeCount;
        }

        m_context->UpdateCurrentViewer();
        if (!fitAll(errorMessage)) {
            return false;
        }

        m_selectionIndex = OccSelectionIndex::fromDocument(document);
        return true;
    } catch (const Standard_Failure& failure) {
        if (errorMessage != nullptr) {
            *errorMessage = failureMessage(QStringLiteral("显示 STEP"), failure);
        }
    } catch (...) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("显示 STEP 失败：未知异常。");
        }
    }
    return false;
}

bool OccViewerWidget::displayRebarPresentation(
    const RebarAisPresentationResult& presentation,
    QString* errorMessage)
{
    if (!presentation.ok || presentation.items.empty()) {
        if (errorMessage != nullptr) {
            *errorMessage = presentation.diagnostic.isEmpty()
                                ? QStringLiteral("钢筋显示结果为空。")
                                : presentation.diagnostic;
        }
        return false;
    }

    for (const RebarAisDisplayItem& item : presentation.items) {
        if (item.aisShape.IsNull()) {
            if (errorMessage != nullptr) {
                *errorMessage = item.diagnostic.isEmpty()
                                    ? QStringLiteral("钢筋显示对象为空。")
                                    : item.diagnostic;
            }
            return false;
        }
    }

    try {
        if (!ensureViewer(errorMessage) || !initializeWindow(errorMessage)) {
            return false;
        }

        for (const RebarAisDisplayItem& item : presentation.items) {
            m_context->Display(item.aisShape, Standard_False);
        }
        m_displayedRebarShapeCount += static_cast<int>(presentation.items.size());
        m_lastDisplayedRebarGroupId = QString::fromStdString(presentation.groupId);
        m_context->UpdateCurrentViewer();
        redraw();
        return true;
    } catch (const Standard_Failure& failure) {
        if (errorMessage != nullptr) {
            *errorMessage = failureMessage(QStringLiteral("显示钢筋"), failure);
        }
    } catch (...) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("显示钢筋失败：未知异常。");
        }
    }
    return false;
}

bool OccViewerWidget::fitAll(QString* errorMessage)
{
    try {
        if (m_view.IsNull()) {
            if (errorMessage != nullptr) {
                *errorMessage = QStringLiteral("Viewer 尚未初始化。");
            }
            return false;
        }
        if (!syncWindow(errorMessage)) {
            return false;
        }
        m_view->FitAll(0.01, Standard_True);
        m_view->ZFitAll();
        redraw();
        return true;
    } catch (const Standard_Failure& failure) {
        if (errorMessage != nullptr) {
            *errorMessage = failureMessage(QStringLiteral("全显"), failure);
        }
    } catch (...) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("全显失败：未知异常。");
        }
    }
    return false;
}

void OccViewerWidget::setSelectionMode(TopAbs_ShapeEnum shapeType)
{
    m_selectionMode = shapeType;
    if (m_context.IsNull()) {
        return;
    }

    m_context->Deactivate();
    if (shapeType != TopAbs_SHAPE) {
        m_context->Activate(AIS_Shape::SelectionMode(shapeType), Standard_True);
    }
}

TopAbs_ShapeEnum OccViewerWidget::selectionMode() const
{
    return m_selectionMode;
}

bool OccViewerWidget::selectByStableId(const QString& stableId, QString* errorMessage)
{
    const OccSelectionResolveResult resolved = m_selectionIndex.resolveStableId(stableId);
    if (!resolved.found) {
        if (errorMessage != nullptr) {
            *errorMessage = resolved.diagnostic;
        }
        return false;
    }

    applyCurrentSelection(resolved.ref);
    return true;
}

std::optional<LegacySelectionRef> OccViewerWidget::currentSelectionRef() const
{
    return m_currentSelection;
}

std::string OccViewerWidget::currentSelectionStableId() const
{
    return m_currentSelection.has_value() ? m_currentSelection->stableId : std::string();
}

QString OccViewerWidget::currentSelectionSummary() const
{
    return m_currentSelectionSummary;
}

QString OccViewerWidget::currentSelectionDetails() const
{
    return m_currentSelectionDetails;
}

const OccSelectionIndex& OccViewerWidget::selectionIndex() const
{
    return m_selectionIndex;
}

bool OccViewerWidget::hasViewer() const
{
    return !m_view.IsNull() && !m_context.IsNull();
}

int OccViewerWidget::displayedShapeCount() const
{
    return m_displayedShapeCount;
}

int OccViewerWidget::displayedRebarShapeCount() const
{
    return m_displayedRebarShapeCount;
}

QString OccViewerWidget::lastDisplayedRebarGroupId() const
{
    return m_lastDisplayedRebarGroupId;
}

QPaintEngine* OccViewerWidget::paintEngine() const
{
    return nullptr;
}

void OccViewerWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    redraw();
}

void OccViewerWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton || m_context.IsNull() || m_view.IsNull()) {
        QWidget::mousePressEvent(event);
        return;
    }

    QString error;
    try {
        m_context->MoveTo(event->position().toPoint().x(),
                          event->position().toPoint().y(),
                          m_view,
                          Standard_True);
        if (!m_context->HasDetected()) {
            clearCurrentSelection();
            return;
        }

        const Handle(StdSelect_BRepOwner) owner =
            Handle(StdSelect_BRepOwner)::DownCast(m_context->DetectedOwner());
        if (owner.IsNull()) {
            clearCurrentSelection();
            return;
        }

        if (!updateSelectionFromShape(owner->Shape(), &error)) {
            clearCurrentSelection();
        }
    } catch (...) {
        clearCurrentSelection();
    }
}

void OccViewerWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    QString ignored;
    (void)syncWindow(&ignored);
    redraw();
}

bool OccViewerWidget::ensureViewer(QString* errorMessage)
{
    if (!m_view.IsNull()) {
        return true;
    }

    try {
        m_displayConnection = new Aspect_DisplayConnection();
        m_graphicDriver = new OpenGl_GraphicDriver(m_displayConnection, Standard_False);
        m_graphicDriver->ChangeOptions().buffersNoSwap = Standard_False;
        m_graphicDriver->ChangeOptions().useSystemBuffer = Standard_False;

        m_viewer = new V3d_Viewer(m_graphicDriver);
        m_viewer->SetDefaultLights();
        m_viewer->SetLightOn();

        m_context = new AIS_InteractiveContext(m_viewer);
        m_view = m_viewer->CreateView();
        m_view->SetBackgroundColor(Quantity_NOC_WHITE);
        m_view->SetImmediateUpdate(Standard_False);
        m_window = new Aspect_NeutralWindow();
        return true;
    } catch (const Standard_Failure& failure) {
        resetViewer();
        if (errorMessage != nullptr) {
            *errorMessage = failureMessage(QStringLiteral("初始化 Viewer"), failure);
        }
    } catch (...) {
        resetViewer();
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("初始化 Viewer 失败：未知异常。");
        }
    }
    return false;
}

bool OccViewerWidget::syncWindow(QString* errorMessage)
{
    if (m_window.IsNull()) {
        return true;
    }

    try {
        const double pixelRatio = devicePixelRatioF();
        const int widthPixels = qMax(1, static_cast<int>(std::lround(width() * pixelRatio)));
        const int heightPixels = qMax(1, static_cast<int>(std::lround(height() * pixelRatio)));
        const Aspect_Drawable nativeHandle =
            reinterpret_cast<Aspect_Drawable>(static_cast<quintptr>(winId()));

        m_window->SetNativeHandle(nativeHandle);
        m_window->SetSize(widthPixels, heightPixels);
        m_window->Map();

        if (!m_view.IsNull()) {
            m_view->MustBeResized();
            m_view->Invalidate();
        }
        return true;
    } catch (const Standard_Failure& failure) {
        if (errorMessage != nullptr) {
            *errorMessage = failureMessage(QStringLiteral("同步 Viewer 窗口"), failure);
        }
    } catch (...) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("同步 Viewer 窗口失败：未知异常。");
        }
    }
    return false;
}

bool OccViewerWidget::initializeWindow(QString* errorMessage)
{
    if (m_windowReady) {
        return true;
    }
    if (m_view.IsNull()) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("Viewer 尚未初始化。");
        }
        return false;
    }
    if (!syncWindow(errorMessage)) {
        return false;
    }
    m_view->SetWindow(m_window);
    m_view->MustBeResized();
    m_windowReady = true;
    return true;
}

bool OccViewerWidget::updateSelectionFromShape(const TopoDS_Shape& selectedShape,
                                               QString* errorMessage)
{
    if (selectedShape.IsNull()) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("选中对象为空。");
        }
        return false;
    }

    const std::optional<LegacySelectionRef> ref =
        m_selectionIndex.refForShape(selectedShape);
    if (!ref.has_value()) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("选中对象无法映射到稳定 SelectionRef。");
        }
        return false;
    }

    applyCurrentSelection(*ref);
    return true;
}

QString OccViewerWidget::shapeKindLabel(LegacyShapeKind kind) const
{
    switch (kind) {
    case LegacyShapeKind::Solid:
        return QStringLiteral("Solid");
    case LegacyShapeKind::Face:
        return QStringLiteral("Face");
    case LegacyShapeKind::Edge:
        return QStringLiteral("Edge");
    case LegacyShapeKind::Vertex:
        return QStringLiteral("Vertex");
    case LegacyShapeKind::Whole:
        return QStringLiteral("Whole");
    }
    return QStringLiteral("Unknown");
}

QString OccViewerWidget::selectionSummary(const LegacySelectionRef& ref) const
{
    return QStringLiteral("%1 #%2")
        .arg(shapeKindLabel(ref.shapeKind))
        .arg(ref.subShapeIndex);
}

QString OccViewerWidget::selectionDetails(const LegacySelectionRef& ref) const
{
    return QStringLiteral("%1\n%2")
        .arg(selectionSummary(ref),
             QString::fromStdString(ref.stableId));
}

void OccViewerWidget::applyCurrentSelection(const LegacySelectionRef& ref)
{
    m_currentSelection = ref;
    m_currentSelectionSummary = selectionSummary(ref);
    m_currentSelectionDetails = selectionDetails(ref);
    emit selectionChanged(m_currentSelectionSummary,
                          m_currentSelectionDetails,
                          QString::fromStdString(ref.stableId));
}

void OccViewerWidget::clearCurrentSelection(bool notify)
{
    m_currentSelection.reset();
    m_currentSelectionSummary.clear();
    m_currentSelectionDetails.clear();
    if (notify) {
        emit selectionChanged({}, {}, {});
    }
}

void OccViewerWidget::resetViewer()
{
    m_windowReady = false;
    m_displayedShapeCount = 0;
    m_displayedRebarShapeCount = 0;
    m_lastDisplayedRebarGroupId.clear();
    clearCurrentSelection(false);
    m_selectionIndex = {};
    m_context.Nullify();
    m_view.Nullify();
    m_viewer.Nullify();
    m_graphicDriver.Nullify();
    m_displayConnection.Nullify();
    m_window.Nullify();
}

void OccViewerWidget::redraw()
{
    if (m_view.IsNull() || !m_windowReady) {
        return;
    }
    try {
        m_view->InvalidateImmediate();
        m_view->Redraw();
    } catch (...) {
        resetViewer();
    }
}

} // namespace tsrebar
