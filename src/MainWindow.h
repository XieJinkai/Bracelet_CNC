#pragma once

#include <QByteArray>
#include <QMainWindow>

class QDoubleSpinBox;
class QSpinBox;
class QPushButton;
class QRadioButton;
class QString;
class OccView;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onBraceletParamsChanged();
    void onExportNcCode();
    void onDisplayWireframe();
    void onDisplaySolid();

private:
    void buildUi();
    void updateParameterConstraints();
    QByteArray createChamferNcCode();
    bool exportChamferModel(const QString& filePath);

    OccView* viewWidget;
    QDoubleSpinBox* outerDiameterSpin;
    QDoubleSpinBox* innerDiameterSpin;
    QDoubleSpinBox* widthSpin;

    QSpinBox* toolIdSpin;
    QDoubleSpinBox* toolDiameterSpin;
    QSpinBox* spindleSpeedSpin;
    QSpinBox* feedRateSpin;
    QDoubleSpinBox* cutDepthSpin;
    QPushButton* exportNcButton;
    QRadioButton* wireframeRadio;
    QRadioButton* solidRadio;
};
