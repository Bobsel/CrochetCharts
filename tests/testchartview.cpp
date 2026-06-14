// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Stefan Dieringer <stefan.dieringer@googlemail.com>

#include "testchartview.h"

#include <QtTest/QSignalSpy>
#include <QWheelEvent>
#include <QPoint>
#include <QTransform>
#include <QVariant>

#include "../src/chartview.h"

namespace {

// Exposes protected wheelEvent for direct invocation. Same pattern as
// TestMW in testfilefactory.cpp. Tests are not friends of ChartView, and
// delivering via QApplication::sendEvent() would couple to the
// QGraphicsView viewport event-filter plumbing which is incidental.
class TestCV : public ChartView
{
public:
    using ChartView::ChartView;
    using ChartView::wheelEvent;
};

}

void TestChartView::zoom_positiveDelta_scalesUpBy5Percent()
{
    ChartView cv;
    QCOMPARE(cv.transform().m11(), qreal(1.0));

    cv.zoom(120);

    QCOMPARE(cv.transform().m11(), qreal(1.05));
    QCOMPARE(cv.transform().m22(), qreal(1.05));
}

void TestChartView::zoom_negativeDelta_scalesDownBy5Percent()
{
    ChartView cv;
    QCOMPARE(cv.transform().m11(), qreal(1.0));

    cv.zoom(-120);

    QCOMPARE(cv.transform().m11(), qreal(0.95));
    QCOMPARE(cv.transform().m22(), qreal(0.95));
}

void TestChartView::zoom_emitsZoomLevelChangedWithNewPercent()
{
    ChartView cv;
    QSignalSpy spy(&cv, SIGNAL(zoomLevelChanged(int)));
    QVERIFY(spy.isValid());

    cv.zoom(120);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.takeFirst().at(0).toInt(), 105);
}

void TestChartView::wheelEvent_ctrlPositiveDelta_zoomsIn()
{
    TestCV cv;
    QCOMPARE(cv.transform().m11(), qreal(1.0));

    QWheelEvent e(QPoint(10, 10), 120, Qt::NoButton, Qt::ControlModifier);
    cv.wheelEvent(&e);

    QCOMPARE(cv.transform().m11(), qreal(1.05));
}

void TestChartView::wheelEvent_ctrlNegativeDelta_zoomsOut()
{
    TestCV cv;
    QCOMPARE(cv.transform().m11(), qreal(1.0));

    QWheelEvent e(QPoint(10, 10), -120, Qt::NoButton, Qt::ControlModifier);
    cv.wheelEvent(&e);

    QCOMPARE(cv.transform().m11(), qreal(0.95));
}

void TestChartView::wheelEvent_noModifier_doesNotZoom()
{
    TestCV cv;
    QCOMPARE(cv.transform().m11(), qreal(1.0));

    QWheelEvent e(QPoint(10, 10), 120, Qt::NoButton, Qt::NoModifier);
    cv.wheelEvent(&e);

    QCOMPARE(cv.transform().m11(), qreal(1.0));
}
