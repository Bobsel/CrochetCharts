// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Stefan Dieringer <stefan.dieringer@googlemail.com>

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
