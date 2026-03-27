#pragma once

#include <QMainWindow>

class QDoubleSpinBox;
class QSpinBox;
class QPushButton;
class OccView;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onBraceletParamsChanged();
    void onExportNcCode();

private:
    void buildUi();

    OccView* viewWidget;
    QDoubleSpinBox* outerDiameterSpin;
    QDoubleSpinBox* innerDiameterSpin;
    QDoubleSpinBox* widthSpin;

    QSpinBox* toolIdSpin;
    QDoubleSpinBox* toolDiameterSpin;
    QSpinBox* spindleSpeedSpin;
    QDoubleSpinBox* feedRateSpin;
    QDoubleSpinBox* cutDepthSpin;
    QPushButton* exportNcButton;
};
