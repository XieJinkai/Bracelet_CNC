#pragma once

#include <QWidget>

#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <TopoDS_Shape.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>

class QPaintEngine;
class QPaintEvent;
class QShowEvent;
class QResizeEvent;
class QMouseEvent;
class QWheelEvent;

class OccView : public QWidget
{
    Q_OBJECT

public:
    explicit OccView(QWidget* parent = nullptr);

    void setBraceletParameters(double outerDiameter, double innerDiameter, double width);
    void recoverViewIfNeeded();
    bool isViewSafe() const;

protected:
    QPaintEngine* paintEngine() const override;
    void paintEvent(QPaintEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    void initializeViewer();
    void updateShape();
    bool checkViewSafeInternal() const;

    bool initialized;
    double currentOuterDiameter;
    double currentInnerDiameter;
    double currentWidth;

    Handle(V3d_Viewer) viewer;
    Handle(V3d_View) view;
    Handle(AIS_InteractiveContext) context;
    Handle(AIS_Shape) currentAis;
    TopoDS_Shape currentShape;
    Standard_Integer xmin;
    Standard_Integer ymin;
    Standard_Integer xmax;
    Standard_Integer ymax;
};
