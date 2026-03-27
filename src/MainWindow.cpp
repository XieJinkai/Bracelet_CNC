#include "MainWindow.h"

#include "OccView.h"

#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QSignalBlocker>
#include <QSpinBox>
#include <QSizePolicy>
#include <QVBoxLayout>
#include <QWidget>

#include <IFSelect_ReturnStatus.hxx>
#include <STEPControl_StepModelType.hxx>
#include <STEPControl_Writer.hxx>

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
      cutDepthSpin(nullptr),
      exportNcButton(nullptr),
      wireframeRadio(nullptr),
      solidRadio(nullptr)
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
    braceletLayout->setContentsMargins(12, 12, 12, 12);
    braceletLayout->setHorizontalSpacing(12);
    braceletLayout->setVerticalSpacing(8);
    braceletLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    braceletLayout->setFormAlignment(Qt::AlignTop);
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

    QGroupBox* toolGroup = new QGroupBox(QStringLiteral("倒角刀参数"), leftPanel);
    QFormLayout* toolLayout = new QFormLayout(toolGroup);
    toolLayout->setContentsMargins(12, 12, 12, 12);
    toolLayout->setHorizontalSpacing(12);
    toolLayout->setVerticalSpacing(8);
    toolLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    toolLayout->setFormAlignment(Qt::AlignTop);
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
    feedRateSpin = new QSpinBox(toolGroup);
    feedRateSpin->setRange(1, 20000);
    feedRateSpin->setValue(1200);
    cutDepthSpin = new QDoubleSpinBox(toolGroup);
    cutDepthSpin->setRange(0.0, 50.0);
    cutDepthSpin->setDecimals(2);
    cutDepthSpin->setValue(1.0);
    toolLayout->addRow(QStringLiteral("刀具刀号"), toolIdSpin);
    toolLayout->addRow(QStringLiteral("刀具直径"), toolDiameterSpin);
    toolLayout->addRow(QStringLiteral("主轴转速"), spindleSpeedSpin);
    toolLayout->addRow(QStringLiteral("进给速率"), feedRateSpin);
    toolLayout->addRow(QStringLiteral("切入深度"), cutDepthSpin);

    QGroupBox* viewGroup = new QGroupBox(QStringLiteral("视图效果"), leftPanel);
    QVBoxLayout* viewLayout = new QVBoxLayout(viewGroup);
    wireframeRadio = new QRadioButton(QStringLiteral("线框"), viewGroup);
    solidRadio = new QRadioButton(QStringLiteral("实体"), viewGroup);
    solidRadio->setChecked(true);
    viewLayout->addWidget(wireframeRadio);
    viewLayout->addWidget(solidRadio);

    exportNcButton = new QPushButton(QStringLiteral("输出NC代码"), leftPanel);
    exportNcButton->setMinimumHeight(exportNcButton->sizeHint().height() * 2);

    leftLayout->addWidget(braceletGroup);
    leftLayout->addWidget(toolGroup);
    leftLayout->addWidget(viewGroup);
    leftLayout->addWidget(exportNcButton);
    leftLayout->setStretch(0, 2);
    leftLayout->setStretch(1, 2);
    leftLayout->setStretch(2, 1);
    leftLayout->setStretch(3, 0);

    braceletGroup->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    toolGroup->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    viewGroup->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    viewWidget = new OccView(central);

    mainLayout->addWidget(leftPanel, 0);
    mainLayout->addWidget(viewWidget, 1);
    mainLayout->setStretch(0, 0);
    mainLayout->setStretch(1, 1);

    connect(outerDiameterSpin, SIGNAL(valueChanged(double)), this, SLOT(onBraceletParamsChanged()));
    connect(innerDiameterSpin, SIGNAL(valueChanged(double)), this, SLOT(onBraceletParamsChanged()));
    connect(widthSpin, SIGNAL(valueChanged(double)), this, SLOT(onBraceletParamsChanged()));
    connect(cutDepthSpin, SIGNAL(valueChanged(double)), this, SLOT(onBraceletParamsChanged()));
    connect(exportNcButton, SIGNAL(clicked()), this, SLOT(onExportNcCode()));
    connect(wireframeRadio, SIGNAL(clicked()), this, SLOT(onDisplayWireframe()));
    connect(solidRadio, SIGNAL(clicked()), this, SLOT(onDisplaySolid()));

    setCentralWidget(central);
    setWindowTitle(QStringLiteral("手镯倒角"));
    resize(1200, 700);
}

void MainWindow::onBraceletParamsChanged()
{
    if (!viewWidget) {
        return;
    }
    updateParameterConstraints();
    viewWidget->setBraceletParameters(
        outerDiameterSpin->value(),
        innerDiameterSpin->value(),
        widthSpin->value(),
        cutDepthSpin->value());
}

void MainWindow::updateParameterConstraints()
{
    double innerDiameter = innerDiameterSpin->value();
    double outerDiameter = outerDiameterSpin->value();
    bool outerAdjusted = false;
    if (outerDiameter < innerDiameter) {
        outerDiameter = innerDiameter;
        QSignalBlocker outerBlocker(outerDiameterSpin);
        outerDiameterSpin->setValue(outerDiameter);
        outerAdjusted = true;
    }
    outerDiameterSpin->setMinimum(innerDiameter);

    double maxCutDepth = (outerDiameter - innerDiameter) * 0.5 - 0.01;
    if (maxCutDepth < 0.0) {
        maxCutDepth = 0.0;
    }
    cutDepthSpin->setMaximum(maxCutDepth);
    bool cutDepthAdjusted = false;
    if (cutDepthSpin->value() > maxCutDepth) {
        QSignalBlocker cutDepthBlocker(cutDepthSpin);
        cutDepthSpin->setValue(maxCutDepth);
        cutDepthAdjusted = true;
    }
    if (outerAdjusted || cutDepthAdjusted) {
        QString message;
        if (outerAdjusted) {
            message += QStringLiteral("外圈直径不能小于内圈直径，已自动调整为 %1。\n")
                .arg(outerDiameter, 0, 'f', 2);
        }
        if (cutDepthAdjusted) {
            message += QStringLiteral("切入深度超出允许范围，已自动调整为 %1。\n允许最大值: %2\n")
                .arg(cutDepthSpin->value(), 0, 'f', 2)
                .arg(maxCutDepth, 0, 'f', 2);
        }
        QMessageBox::warning(this, QStringLiteral("参数调整提示"), message.trimmed());
    }
}

void MainWindow::onExportNcCode()
{
    QByteArray ncCode = createChamferNcCode();
    QString filePath = QFileDialog::getSaveFileName(
        this,
        QStringLiteral("导出NC代码"),
        QStringLiteral("bracelet_chamfer.nc"),
        QStringLiteral("NC文件 (*.nc);;文本文件 (*.txt);;所有文件 (*)"));
    if (filePath.isEmpty()) {
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QMessageBox::warning(this, QStringLiteral("导出失败"), QStringLiteral("无法写入文件。"));
        return;
    }
    file.write(ncCode);
    file.close();
    QFileInfo fileInfo(filePath);
    QString modelPath = fileInfo.path() + QLatin1Char('/') + fileInfo.completeBaseName() + QStringLiteral(".step");
    if (!exportChamferModel(modelPath)) {
        QMessageBox::warning(this, QStringLiteral("导出部分成功"), QStringLiteral("NC代码已导出，但三维模型导出失败。"));
        return;
    }
    QMessageBox::information(this, QStringLiteral("导出成功"), QStringLiteral("NC代码与三维模型已导出。"));
}

void MainWindow::onDisplayWireframe()
{
    if (!viewWidget) {
        return;
    }
    viewWidget->setDisplayWireframe();
}

void MainWindow::onDisplaySolid()
{
    if (!viewWidget) {
        return;
    }
    viewWidget->setDisplaySolid();
}

QByteArray MainWindow::createChamferNcCode()
{
    int chamferToolNumber = toolIdSpin->value();
    int chamferSpindleSpeed = spindleSpeedSpin->value();
    int chamferFeedRate = feedRateSpin->value();
    double outerDiameterValue = outerDiameterSpin->value();
    double braceletWidthValue = widthSpin->value();
    double chamferDepthValue = cutDepthSpin->value();

    double radius = outerDiameterValue * 0.5;
    double approachX = 2.0;
    double xZero = 0.0;
    double yDownInner = -braceletWidthValue * 0.5 - 1.3;
    double yBaseInner = -braceletWidthValue * 0.5 - 0.3;
    double zBase = radius - chamferDepthValue - 0.3;

    QString code;
    code += QStringLiteral("(手镯双端倒角NC程序)\n");
    code += QStringLiteral("%\n");
    code += QStringLiteral("O0001\n");
    code += QStringLiteral("G91 G28 Z0\n");
    code += QStringLiteral("G0 G90 G17 G40 G80 G71\n");
    code += QStringLiteral("T%1 M6\n").arg(chamferToolNumber);
    code += QStringLiteral("S%1 M3\n").arg(chamferSpindleSpeed);
    code += QStringLiteral("G0 G90 G54 X%1 Y%2 B-90.0 C0.0 M08\n")
        .arg(approachX, 0, 'f', 3)
        .arg(yDownInner, 0, 'f', 3);
    code += QStringLiteral("G04 X2.\n");
    code += QStringLiteral("G43 H%1 Z%2\n").arg(chamferToolNumber).arg(radius + 5, 0, 'f', 3);
    code += QStringLiteral("F%1\n").arg(chamferFeedRate);

    code += QStringLiteral("G01 X%1 Y%2 Z%3\n")
        .arg(approachX, 0, 'f', 3)
        .arg(yDownInner, 0, 'f', 3)
        .arg(zBase, 0, 'f', 3);
    code += QStringLiteral("X%1 Y%2 Z%3\n")
        .arg(approachX, 0, 'f', 3)
        .arg(yBaseInner, 0, 'f', 3)
        .arg(zBase, 0, 'f', 3);
    code += QStringLiteral("X%1 Y%2 Z%3 C0.\n")
        .arg(xZero, 0, 'f', 3)
        .arg(yBaseInner, 0, 'f', 3)
        .arg(zBase, 0, 'f', 3);
    code += QStringLiteral("C90.\nC180.\nC270.\nC360.\n");
    code += QStringLiteral("X%1\n").arg(-approachX, 0, 'f', 3);
    code += QStringLiteral("Y%1 Z%2\n")
        .arg(yDownInner, 0, 'f', 3)
        .arg(zBase, 0, 'f', 3);
    code += QStringLiteral("G0 Z%1\n").arg(zBase + 5.0, 0, 'f', 3);

    double xDownOuter = -approachX;
    double xFeedOuter = -approachX;
    double yDownOuter = -yDownInner;
    double yBaseOuter = -yBaseInner;
    code += QStringLiteral("X%1 Y%2\n")
        .arg(xDownOuter, 0, 'f', 3)
        .arg(yDownOuter, 0, 'f', 3);
    code += QStringLiteral("G01 X%1 Y%2 Z%3\n")
        .arg(xDownOuter, 0, 'f', 3)
        .arg(yDownOuter, 0, 'f', 3)
        .arg(zBase, 0, 'f', 3);
    code += QStringLiteral("X%1 Y%2 Z%3\n")
        .arg(xFeedOuter, 0, 'f', 3)
        .arg(yBaseOuter, 0, 'f', 3)
        .arg(zBase, 0, 'f', 3);
    code += QStringLiteral("X%1 Y%2 Z%3 C0.\n")
        .arg(xZero, 0, 'f', 3)
        .arg(yBaseOuter, 0, 'f', 3)
        .arg(zBase, 0, 'f', 3);
    code += QStringLiteral("C90.\nC180.\nC270.\nC360.\n");
    code += QStringLiteral("X%1\n").arg(-xFeedOuter, 0, 'f', 3);
    code += QStringLiteral("Y%1 Z%2\n")
        .arg(yDownOuter, 0, 'f', 3)
        .arg(zBase, 0, 'f', 3);

    code += QStringLiteral("G00 Z%1\n").arg(radius + 3.0, 0, 'f', 3);
    code += QStringLiteral("M05\n");
    code += QStringLiteral("G91 G28 Z0.0\n");
    code += QStringLiteral("G28 X0 Y0 B0 C0\n");
    code += QStringLiteral("M30\n");
    code += QStringLiteral("%\n");
    return code.toUtf8();
}

bool MainWindow::exportChamferModel(const QString& filePath)
{
    if (!viewWidget) {
        return false;
    }
    TopoDS_Shape modelShape = viewWidget->currentModelShape();
    if (modelShape.IsNull()) {
        return false;
    }
    STEPControl_Writer writer;
    if (writer.Transfer(modelShape, STEPControl_AsIs) != IFSelect_RetDone) {
        return false;
    }
    return writer.Write(filePath.toStdString().c_str()) == IFSelect_RetDone;
}
