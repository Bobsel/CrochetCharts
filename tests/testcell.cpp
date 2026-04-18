/****************************************************************************\
 Copyright (c) 2010-2014 Stitch Works Software
 Brian C. Milco <bcmilco@gmail.com>
 Copyright (c) 2026 Stefan Dieringer <stefan.dieringer@googlemail.com>

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
#include "testcell.h"
#include "../src/stitchlibrary.h"
#include "../src/ChartItemTools.h"

void TestCell::initTestCase()
{
    StitchLibrary::inst()->loadStitchSets();
}

void TestCell::setCellValues()
{
    QFETCH(QString, name);
    QFETCH(qreal, width);
    QFETCH(qreal, height);

    Cell* c = new Cell();
    Stitch* s = StitchLibrary::inst()->findStitch(name);
    c->setStitch(s);

    QVERIFY(c->sceneBoundingRect().width() == width);
    QVERIFY(c->sceneBoundingRect().height() == height);

    delete c;
    c = 0;

}

void TestCell::setCellValues_data()
{

    QTest::addColumn<QString>("name");
    QTest::addColumn<qreal>("width");
    QTest::addColumn<qreal>("height");

    QTest::newRow("ch")    << "ch" << 32.0 << 16.0;
    QTest::newRow("hdc")   << "hdc" << 32.0 << 64.0;
    QTest::newRow("dc")    << "dc" << 32.0 << 80.0;

}

void TestCell::setRotation()
{
    QFETCH(QString, name);
    QFETCH(qreal, angle);

    Cell* c = new Cell();
    QGraphicsScene scene;
    scene.addItem(c);

    Stitch* s = StitchLibrary::inst()->findStitch(name);
    c->setStitch(s);

    QPointF origin(c->boundingRect().width() / 2, c->boundingRect().height());
    c->setTransformOriginPoint(origin);
    c->setRotation(angle);

    QCOMPARE(c->rotation(), angle);
    QCOMPARE(c->transformOriginPoint(), origin);

    scene.removeItem(c);
    delete c;
}

void TestCell::setRotation_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<qreal>("angle");

    QTest::newRow("ch 45")   << "ch"  << 45.0;
    QTest::newRow("hdc 90")  << "hdc" << 90.0;
    QTest::newRow("dc -30")  << "dc"  << -30.0;
}

void TestCell::setScale()
{
    QFETCH(QString, name);
    QFETCH(qreal, scaleX);
    QFETCH(qreal, scaleY);

    Cell* c = new Cell();
    QGraphicsScene scene;
    scene.addItem(c);

    Stitch* s = StitchLibrary::inst()->findStitch(name);
    c->setStitch(s);

    QPointF origin(c->boundingRect().width() / 2, c->boundingRect().height());
    c->setTransformOriginPoint(origin);
    ChartItemTools::setScaleX(c, scaleX);
    ChartItemTools::setScaleY(c, scaleY);

    QCOMPARE(ChartItemTools::getScaleX(c), scaleX);
    QCOMPARE(ChartItemTools::getScaleY(c), scaleY);

    scene.removeItem(c);
    delete c;
}

void TestCell::setScale_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<qreal>("scaleX");
    QTest::addColumn<qreal>("scaleY");

    QTest::newRow("ch identity")    << "ch"  << 1.0 << 1.0;
    QTest::newRow("hdc uniform")    << "hdc" << 2.5 << 2.5;
    QTest::newRow("dc non-uniform") << "dc"  << 1.5 << 3.0;
}

void TestCell::setBgColor()
{
    QFETCH(QString, name);
    QFETCH(QString, color);

    QGraphicsScene scene;
    Cell* c = new Cell();
    scene.addItem(c);

    Stitch* s = StitchLibrary::inst()->findStitch(name);
    c->setStitch(s);

    c->setBgColor(QColor(color));

    QCOMPARE(c->bgColor(), QColor(color));

    scene.removeItem(c);
    delete c;
}

void TestCell::setBgColor_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<QString>("color");

    QTest::newRow("ch blue")   << "ch"  << "#0000FF";
    QTest::newRow("hdc red")   << "hdc" << "#FF0000";
    QTest::newRow("dc green")  << "dc"  << "#00FF00";
    QTest::newRow("named")     << "ch"  << "cornflowerblue";
}

void TestCell::setAllProperties()
{
    QFETCH(QString, name);
    QFETCH(qreal, angle);
    QFETCH(qreal, scaleX);
    QFETCH(qreal, scaleY);
    QFETCH(QString, color);

    QGraphicsScene scene;
    Cell* c = new Cell();
    scene.addItem(c);

    Stitch* s = StitchLibrary::inst()->findStitch(name);
    c->setStitch(s);

    QPointF origin(c->boundingRect().width() / 2, c->boundingRect().height());
    c->setTransformOriginPoint(origin);
    c->setBgColor(QColor(color));
    c->setRotation(angle);
    ChartItemTools::setScaleX(c, scaleX);
    ChartItemTools::setScaleY(c, scaleY);

    QCOMPARE(c->rotation(), angle);
    QCOMPARE(c->transformOriginPoint(), origin);
    QCOMPARE(c->bgColor(), QColor(color));
    QCOMPARE(ChartItemTools::getScaleX(c), scaleX);
    QCOMPARE(ChartItemTools::getScaleY(c), scaleY);

    scene.removeItem(c);
    delete c;
}

void TestCell::setAllProperties_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<qreal>("angle");
    QTest::addColumn<qreal>("scaleX");
    QTest::addColumn<qreal>("scaleY");
    QTest::addColumn<QString>("color");

    QTest::newRow("ch")  << "ch"  << 45.0 << 1.5 << 2.0 << "#0000FF";
    QTest::newRow("hdc") << "hdc" << 45.0 << 1.5 << 2.0 << "#FF0000";
    QTest::newRow("dc")  << "dc"  << 45.0 << 1.5 << 2.0 << "#00FF00";
}

void TestCell::cleanupTestCase()
{
}
