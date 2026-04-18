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
#include "teststitch.h"

#include <QPainter>
#include <QPixmap>
#include <QImage>
#include <QtSvg/QSvgRenderer>

#include <QDebug>

void TestStitch::initTestCase()
{
    mS = new Stitch();
}

void TestStitch::stitchSetup()
{
    //FIXME: don't hard code the stitches.
    mS->setName("ch");
    mS->setFile("../stitches/ch.svg");
    mS->setDescription("chain");
    mS->setCategory("Basic");
    mS->setWrongSide("ch");

    QVERIFY(mS->name() == "ch");
    QVERIFY(mS->file() == "../stitches/ch.svg");
    QVERIFY(mS->description() == "chain");
    QVERIFY(mS->category() == "Basic");
    QVERIFY(mS->wrongSide() == "ch");
}

void TestStitch::stitchRender()
{
    QFETCH(QString, stitch);
    QFETCH(QString, stitchFile);
    QFETCH(qreal, width);
    QFETCH(qreal, height);

    mS->setFile(stitchFile);

    QCOMPARE(mS->width(), width);
    QCOMPARE(mS->height(), height);

    QPixmap* rendered = mS->renderPixmap();
    QVERIFY(rendered != 0);
    QVERIFY(!rendered->isNull());

    QPixmap canvas = QPixmap(int(width), int(height));
    canvas.fill(QColor(Qt::white));
    QPainter painter(&canvas);
    painter.drawPixmap(0, 0, *rendered);
    painter.end();

    const QImage img = canvas.toImage();
    const QRgb whiteRgb = QColor(Qt::white).rgb() & 0x00FFFFFF;
    int nonWhite = 0;
    for (int y = 0; y < img.height(); ++y) {
        for (int x = 0; x < img.width(); ++x) {
            if ((img.pixel(x, y) & 0x00FFFFFF) != whiteRgb) {
                ++nonWhite;
            }
        }
    }
    QVERIFY2(nonWhite > 0, qPrintable(
        QString("stitch '%1' rendered all-white pixels").arg(stitch)));

    QSvgRenderer* svg = mS->renderSvg();
    QVERIFY(svg != 0);
    QVERIFY(svg->isValid());
}

void TestStitch::stitchRender_data()
{
    QTest::addColumn<QString>("stitch");
    QTest::addColumn<QString>("stitchFile");
    QTest::addColumn<qreal>("width");
    QTest::addColumn<qreal>("height");

    QTest::newRow("ch")  << "ch"  << "../stitches/ch.svg"  << 32.0 << 16.0;
    QTest::newRow("hdc") << "hdc" << "../stitches/hdc.svg" << 32.0 << 64.0;
    QTest::newRow("dc")  << "dc"  << "../stitches/dc.svg"  << 32.0 << 80.0;
}

void TestStitch::cleanupTestCase()
{
}

