#include "MainWindow.h"

#include "OccView.h"

#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      viewWidget(nullptr),
      outerDiameterSpin(nullptr),
      innerDiameterSpin(nullptr),
      widthSpin(nullptr),
      toolIdSpin(nullptr),
      toolDiameterSpin(nullptr),
      spindleSpeedSpin(nullptr),
      feedRateSpin(nullptr),
      cutDepthSpin(nullptr)
{
    buildUi();
    onBraceletParamsChanged();
}

void MainWindow::buildUi()
{
    QWidget* central = new QWidget(this);
    QHBoxLayout* mainLayout = new QHBoxLayout(central);

    QWidget* leftPanel = new QWidget(central);
    QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);

    QGroupBox* braceletGroup = new QGroupBox(QStringLiteral("手镯参数"), leftPanel);
    QFormLayout* braceletLayout = new QFormLayout(braceletGroup);
    outerDiameterSpin = new QDoubleSpinBox(braceletGroup);
    outerDiameterSpin->setRange(1.0, 500.0);
    outerDiameterSpin->setDecimals(2);
    outerDiameterSpin->setValue(70.0);
    innerDiameterSpin = new QDoubleSpinBox(braceletGroup);
    innerDiameterSpin->setRange(1.0, 500.0);
    innerDiameterSpin->setDecimals(2);
    innerDiameterSpin->setValue(60.0);
    widthSpin = new QDoubleSpinBox(braceletGroup);
    widthSpin->setRange(1.0, 200.0);
    widthSpin->setDecimals(2);
    widthSpin->setValue(20.0);
    braceletLayout->addRow(QStringLiteral("外圈直径"), outerDiameterSpin);
    braceletLayout->addRow(QStringLiteral("内圈直径"), innerDiameterSpin);
    braceletLayout->addRow(QStringLiteral("手镯宽度"), widthSpin);

    QGroupBox* toolGroup = new QGroupBox(QStringLiteral("刀具参数"), leftPanel);
    QFormLayout* toolLayout = new QFormLayout(toolGroup);
    toolIdSpin = new QSpinBox(toolGroup);
    toolIdSpin->setRange(1, 999);
    toolIdSpin->setValue(1);
    toolDiameterSpin = new QDoubleSpinBox(toolGroup);
    toolDiameterSpin->setRange(0.1, 100.0);
    toolDiameterSpin->setDecimals(2);
    toolDiameterSpin->setValue(6.0);
    spindleSpeedSpin = new QSpinBox(toolGroup);
    spindleSpeedSpin->setRange(100, 60000);
    spindleSpeedSpin->setValue(12000);
    feedRateSpin = new QDoubleSpinBox(toolGroup);
    feedRateSpin->setRange(1.0, 20000.0);
    feedRateSpin->setDecimals(1);
    feedRateSpin->setValue(1200.0);
    cutDepthSpin = new QDoubleSpinBox(toolGroup);
    cutDepthSpin->setRange(0.1, 50.0);
    cutDepthSpin->setDecimals(2);
    cutDepthSpin->setValue(1.0);
    toolLayout->addRow(QStringLiteral("刀具刀号"), toolIdSpin);
    toolLayout->addRow(QStringLiteral("刀具直径"), toolDiameterSpin);
    toolLayout->addRow(QStringLiteral("主轴转速"), spindleSpeedSpin);
    toolLayout->addRow(QStringLiteral("进给速率"), feedRateSpin);
    toolLayout->addRow(QStringLiteral("切入深度"), cutDepthSpin);

    leftLayout->addWidget(braceletGroup);
    leftLayout->addWidget(toolGroup);
    leftLayout->addStretch(1);

    viewWidget = new OccView(central);

    mainLayout->addWidget(leftPanel, 0);
    mainLayout->addWidget(viewWidget, 1);
    mainLayout->setStretch(0, 0);
    mainLayout->setStretch(1, 1);

    connect(outerDiameterSpin, SIGNAL(valueChanged(double)), this, SLOT(onBraceletParamsChanged()));
    connect(innerDiameterSpin, SIGNAL(valueChanged(double)), this, SLOT(onBraceletParamsChanged()));
    connect(widthSpin, SIGNAL(valueChanged(double)), this, SLOT(onBraceletParamsChanged()));

    setCentralWidget(central);
    setWindowTitle(QStringLiteral("Bracelet CNC"));
    resize(1200, 700);
}

void MainWindow::onBraceletParamsChanged()
{
    if (!viewWidget) {
        return;
    }
    viewWidget->setBraceletParameters(
        outerDiameterSpin->value(),
        innerDiameterSpin->value(),
        widthSpin->value());
}
