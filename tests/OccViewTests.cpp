#include <QtTest/QtTest>
#include <QApplication>
#include <QCoreApplication>
#include <QWheelEvent>

#include "OccView.h"

class OccViewTests : public QObject
{
    Q_OBJECT

private slots:
    void testRapidRotation();
    void testExtremeZoom();
    void testResize();
};

void OccViewTests::testRapidRotation()
{
    OccView view;
    view.resize(800, 600);
    view.show();
    QTest::qWait(50);
    view.setBraceletParameters(80.0, 60.0, 20.0);
    QTest::qWait(50);

    QPoint center = view.rect().center();
    QTest::mousePress(&view, Qt::LeftButton, Qt::NoModifier, center);
    for (int i = 0; i < 20; ++i) {
        QPoint pos = center + QPoint((i % 2 == 0) ? 120 : -120, (i % 3 == 0) ? 80 : -80);
        QTest::mouseMove(&view, pos, 1);
    }
    QTest::mouseRelease(&view, Qt::LeftButton, Qt::NoModifier, center);
    view.recoverViewIfNeeded();
    QVERIFY(view.isViewSafe());
}

void OccViewTests::testExtremeZoom()
{
    OccView view;
    view.resize(800, 600);
    view.show();
    QTest::qWait(50);
    view.setBraceletParameters(80.0, 60.0, 20.0);
    QTest::qWait(50);

    QPoint center = view.rect().center();
    QPoint globalPos = view.mapToGlobal(center);
    QWheelEvent zoomIn(QPointF(center), QPointF(globalPos), QPoint(), QPoint(0, 1200),
                       Qt::NoButton, Qt::NoModifier, Qt::NoScrollPhase, false);
    QCoreApplication::sendEvent(&view, &zoomIn);
    QWheelEvent zoomOut(QPointF(center), QPointF(globalPos), QPoint(), QPoint(0, -1200),
                        Qt::NoButton, Qt::NoModifier, Qt::NoScrollPhase, false);
    QCoreApplication::sendEvent(&view, &zoomOut);
    view.recoverViewIfNeeded();
    QVERIFY(view.isViewSafe());
}

void OccViewTests::testResize()
{
    OccView view;
    view.resize(800, 600);
    view.show();
    QTest::qWait(50);
    view.setBraceletParameters(80.0, 60.0, 20.0);
    QTest::qWait(50);

    view.resize(1200, 800);
    QTest::qWait(10);
    view.resize(240, 180);
    QTest::qWait(10);
    view.resize(900, 700);
    QTest::qWait(10);
    view.recoverViewIfNeeded();
    QVERIFY(view.isViewSafe());
}

QTEST_MAIN(OccViewTests)

#include "OccViewTests.moc"
