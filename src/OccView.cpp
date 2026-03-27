#include "OccView.h"

#include <AIS_Shape.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepBndLib.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <Bnd_Box.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <Graphic3d_TypeOfBackfacingModel.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <Quantity_NameOfColor.hxx>
#include <V3d_Viewer.hxx>
#include <WNT_Window.hxx>
#include <gp_Pnt.hxx>
#include <Graphic3d_Camera.hxx>

#include <QMouseEvent>
#include <QPaintEvent>
#include <QShowEvent>
#include <QWheelEvent>

OccView::OccView(QWidget* parent)
    : QWidget(parent),
      initialized(false),
      currentOuterDiameter(70.0),
      currentInnerDiameter(60.0),
      currentWidth(20.0),
      xmin(0),
      ymin(0),
      xmax(0),
      ymax(0)
{
    setAttribute(Qt::WA_NativeWindow);
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);
    setMinimumSize(400, 400);
}

void OccView::setBraceletParameters(double outerDiameter, double innerDiameter, double width)
{
    currentOuterDiameter = outerDiameter;
    currentInnerDiameter = innerDiameter;
    currentWidth = width;
    if (initialized) {
        updateShape();
    }
}

void OccView::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    if (!initialized) {
        initializeViewer();
        updateShape();
    }
}

void OccView::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    if (!view.IsNull()) {
        view->MustBeResized();
    }
    recoverViewIfNeeded();
}

void OccView::initializeViewer()
{
    Handle(Aspect_DisplayConnection) displayConnection = new Aspect_DisplayConnection();
    Handle(OpenGl_GraphicDriver) graphicDriver = new OpenGl_GraphicDriver(displayConnection);
    viewer = new V3d_Viewer(graphicDriver);
    viewer->SetDefaultLights();
    viewer->SetLightOn();

    context = new AIS_InteractiveContext(viewer);
    view = viewer->CreateView();

    Handle(WNT_Window) window = new WNT_Window(reinterpret_cast<Aspect_Handle>(winId()));
    view->SetWindow(window);
    if (!window->IsMapped()) {
        window->Map();
    }
    view->SetBackgroundColor(Quantity_NOC_GRAY50);
    view->SetBackFacingModel(Graphic3d_TypeOfBackfacingModel_DoubleSided);
    view->TriedronDisplay(Aspect_TOTP_LEFT_LOWER, Quantity_NOC_WHITE, 0.1, V3d_ZBUFFER);
    view->MustBeResized();
    initialized = true;
}

QPaintEngine* OccView::paintEngine() const
{
    return nullptr;
}

void OccView::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)
    recoverViewIfNeeded();
    if (!view.IsNull()) {
        view->Redraw();
    }
}

void OccView::updateShape()
{
    if (context.IsNull()) {
        return;
    }

    double outerRadius = currentOuterDiameter * 0.5;
    double innerRadius = currentInnerDiameter * 0.5;
    if (innerRadius >= outerRadius) {
        innerRadius = outerRadius * 0.8;
    }
    if (currentWidth <= 0.0) {
        currentWidth = 1.0;
    }

    TopoDS_Shape outer = BRepPrimAPI_MakeCylinder(outerRadius, currentWidth).Shape();
    TopoDS_Shape inner = BRepPrimAPI_MakeCylinder(innerRadius, currentWidth).Shape();
    currentShape = BRepAlgoAPI_Cut(outer, inner).Shape();

    if (!currentAis.IsNull()) {
        context->Remove(currentAis, false);
    }
    currentAis = new AIS_Shape(currentShape);
    context->Display(currentAis, true);
    view->FitAll();
}

void OccView::mousePressEvent(QMouseEvent* event)
{
    if (!event || view.IsNull()) {
        return;
    }
    if (event->button() == Qt::LeftButton) {
        xmin = event->x();
        ymin = event->y();
        xmax = event->x();
        ymax = event->y();
        view->StartRotation(event->x(), event->y());
    }
}

void OccView::mouseMoveEvent(QMouseEvent* event)
{
    if (!event || view.IsNull()) {
        return;
    }
    if (event->buttons() & Qt::LeftButton) {
        if (event->modifiers() & Qt::ShiftModifier) {
            view->Pan(event->x() - xmax, ymax - event->y());
            xmax = event->x();
            ymax = event->y();
        } else {
            view->Rotation(event->x(), event->y());
        }
    }
}

void OccView::mouseReleaseEvent(QMouseEvent* event)
{
    Q_UNUSED(event)
    recoverViewIfNeeded();
}

void OccView::wheelEvent(QWheelEvent* event)
{
    if (!event || view.IsNull()) {
        return;
    }
    Standard_Real zoomFactor = (event->angleDelta().y() > 0) ? 1.1 : 0.9;
    view->SetZoom(zoomFactor);
    recoverViewIfNeeded();
}

void OccView::recoverViewIfNeeded()
{
    if (!checkViewSafeInternal()) {
        view->FitAll();
        view->ZFitAll();
    }
}

bool OccView::isViewSafe() const
{
    return checkViewSafeInternal();
}

bool OccView::checkViewSafeInternal() const
{
    if (view.IsNull()) {
        return true;
    }
    if (currentShape.IsNull()) {
        return true;
    }
    Bnd_Box bbox;
    BRepBndLib::Add(currentShape, bbox);
    if (bbox.IsVoid()) {
        return true;
    }
    Standard_Real xminBox = 0.0;
    Standard_Real yminBox = 0.0;
    Standard_Real zminBox = 0.0;
    Standard_Real xmaxBox = 0.0;
    Standard_Real ymaxBox = 0.0;
    Standard_Real zmaxBox = 0.0;
    bbox.Get(xminBox, yminBox, zminBox, xmaxBox, ymaxBox, zmaxBox);
    gp_Pnt minPnt(xminBox, yminBox, zminBox);
    gp_Pnt maxPnt(xmaxBox, ymaxBox, zmaxBox);
    gp_Pnt center((xminBox + xmaxBox) * 0.5, (yminBox + ymaxBox) * 0.5, (zminBox + zmaxBox) * 0.5);
    Standard_Real diag = minPnt.Distance(maxPnt);
    if (diag <= 0.0) {
        return true;
    }
    Handle(Graphic3d_Camera) camera = view->Camera();
    if (camera.IsNull()) {
        return true;
    }
    gp_Pnt eye = camera->Eye();
    gp_Pnt camCenter = camera->Center();
    Standard_Real dist = eye.Distance(camCenter);
    Standard_Real centerDist = camCenter.Distance(center);
    Standard_Real minDist = diag * 0.2;
    Standard_Real maxDist = diag * 2000.0;
    Standard_Real maxCenterDist = diag * 5.0;
    if (dist < minDist || dist > maxDist) {
        return false;
    }
    if (centerDist > maxCenterDist) {
        return false;
    }
    return true;
}
