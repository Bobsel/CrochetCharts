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
#ifndef TESTCHARTVIEW_H
#define TESTCHARTVIEW_H

#include <QtTest/QTest>
#include <QObject>

/**
 * Port-regression tests for ChartView.
 *
 * Purpose: pin the Qt4 zoom/wheel semantics so the Qt6 port (which removes
 * QWheelEvent::delta() in favour of angleDelta().y()) is caught by a
 * semantic oracle rather than by users.
 *
 * Observable under test: QGraphicsView::transform().m11() and the
 * zoomLevelChanged(int) signal. No rendering, no pixels.
 */
class TestChartView : public QObject
{
    Q_OBJECT
private slots:
    // Pins integer arithmetic of ChartView::zoom(int mouseDelta)
    // independent of QWheelEvent API. This slot survives any Qt upgrade.
    void zoom_positiveDelta_scalesUpBy5Percent();
    void zoom_negativeDelta_scalesDownBy5Percent();
    void zoom_emitsZoomLevelChangedWithNewPercent();

    // Pins the QWheelEvent -> ChartView::zoom(delta) bridge. This slot is
    // the Qt6 canary: once QWheelEvent::delta() is removed, these will
    // fail to compile and force the port to carry the semantic over to
    // angleDelta().y().
    void wheelEvent_ctrlPositiveDelta_zoomsIn();
    void wheelEvent_ctrlNegativeDelta_zoomsOut();
    void wheelEvent_noModifier_doesNotZoom();
};

#endif // TESTCHARTVIEW_H
