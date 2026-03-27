#include "OccView.h"

#include <AIS_Shape.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <V3d_Viewer.hxx>
#include <WNT_Window.hxx>

#include <QShowEvent>

OccView::OccView(QWidget* parent)
    : QWidget(parent),
      initialized(false),
      currentOuterDiameter(70.0),
      currentInnerDiameter(60.0),
      currentWidth(20.0)
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
    view->SetBackgroundColor(Quantity_NOC_GRAY15);
    view->MustBeResized();
    initialized = true;
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
