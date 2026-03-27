#pragma once

#include <QWidget>

#include <AIS_InteractiveContext.hxx>
#include <TopoDS_Shape.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>

class OccView : public QWidget
{
    Q_OBJECT

public:
    explicit OccView(QWidget* parent = nullptr);

    void setBraceletParameters(double outerDiameter, double innerDiameter, double width);

protected:
    void showEvent(QShowEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void initializeViewer();
    void updateShape();

    bool initialized;
    double currentOuterDiameter;
    double currentInnerDiameter;
    double currentWidth;

    Handle(V3d_Viewer) viewer;
    Handle(V3d_View) view;
    Handle(AIS_InteractiveContext) context;
    Handle(AIS_Shape) currentAis;
    TopoDS_Shape currentShape;
};
