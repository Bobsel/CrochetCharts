/****************************************************************************\
 Copyright (c) 2010-2014 Stitch Works Software
 Brian C. Milco <bcmilco@gmail.com>

 This file is part of Crochet Charts.

 Crochet Charts is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Crochet Charts is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Crochet Charts. If not, see <http://www.gnu.org/licenses/>.

 \****************************************************************************/
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
