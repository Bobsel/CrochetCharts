// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Stefan Dieringer <stefan.dieringer@googlemail.com>

#include "testfilefactory.h"

#include <QDir>
#include <QFile>
#include <QTabWidget>
#include <QGraphicsItem>
#include <QStringList>

#include "../src/mainwindow.h"
#include "../src/settings.h"
#include "../src/crochettab.h"
#include "../src/scene.h"
#include "../src/filefactory.h"
#include "../src/cell.h"
#include "../src/ChartItemTools.h"
#include "../src/stitch.h"

namespace {

class TestMW : public MainWindow
{
public:
    using MainWindow::MainWindow;
    QTabWidget* tabs() { return tabWidget(); }
    CrochetTab* makeTab(Scene::ChartStyle s) { return createTab(s); }
};

const QString kBlankPath          = "fixtures/port/blank_v2.crochetcharts";
const QString kBasicPath          = "fixtures/port/basic_v2.crochetcharts";
const QString kBasicSnapshotPath  = "fixtures/port/basic_v2.snapshot.txt";
const QString kLegacyPath         = "fixtures/port/legacy_v1.crochetcharts";
const QString kLegacySnapshotPath = "fixtures/port/legacy_v1.snapshot.txt";

int cellCount(Scene* scene)
{
    int n = 0;
    foreach (QGraphicsItem* item, scene->items()) {
        if (item->type() == Cell::Type) n++;
    }
    return n;
}

QString fmt(qreal d) { return QString::number(d, 'f', 3); }

QString dumpCell(Cell* c)
{
    const QString stitchName = c->stitch() ? c->stitch()->name() : QString("(null)");
    return QString("cell pos=(%1,%2) rot=%3 scale=(%4,%5) bg=%6 color=%7 stitch=%8 layer=%9")
        .arg(fmt(c->pos().x()))
        .arg(fmt(c->pos().y()))
        .arg(fmt(c->rotation()))
        .arg(fmt(ChartItemTools::getScaleX(c)))
        .arg(fmt(ChartItemTools::getScaleY(c)))
        .arg(c->bgColor().name())
        .arg(c->color().name())
        .arg(stitchName)
        .arg(c->layer());
}

QString dumpScene(Scene* scene)
{
    QStringList lines;
    foreach (QGraphicsItem* item, scene->items()) {
        if (item->type() == Cell::Type) {
            lines << dumpCell(static_cast<Cell*>(item));
        }
    }
    lines.sort();
    return lines.join("\n") + (lines.isEmpty() ? QString() : QString("\n"));
}

bool writeText(const QString& path, const QString& content)
{
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
    f.write(content.toUtf8());
    return f.error() == QFile::NoError;
}

QString readText(const QString& path)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return QString();
    return QString::fromUtf8(f.readAll());
}

CrochetTab* firstTab(TestMW& mw)
{
    if (mw.tabs()->count() < 1) return 0;
    return qobject_cast<CrochetTab*>(mw.tabs()->widget(0));
}

}

void TestFileFactory::initTestCase()
{
    Settings::inst()->setValue("checkForUpdates", false);
}

void TestFileFactory::portFixturesDirReachable()
{
    const QString portDir = "fixtures/port";

    QVERIFY2(QDir(portDir).exists(),
             "tests/fixtures/port/ not found — tests must run with CWD=tests/");
    QVERIFY2(QFile::exists(portDir + "/README.md"),
             "tests/fixtures/port/README.md missing — fixture corpus not seeded");
}

void TestFileFactory::mainWindowConstructs()
{
    MainWindow mw;
    QVERIFY2(mw.isWindow(),
             "MainWindow construction failed — port-fixture generator path is blocked");
}

void TestFileFactory::generateBlankV2_ifMissing()
{
    if (QFile::exists(kBlankPath)) {
        QVERIFY2(QFileInfo(kBlankPath).size() > 0,
                 "blank_v2.crochetcharts exists but is empty");
        return;
    }

    TestMW mw;
    CrochetTab* tab = mw.makeTab(Scene::Blank);
    QVERIFY(tab != 0);
    mw.tabs()->addTab(tab, "BlankChart");
    tab->createChart(Scene::Blank, 0, 0, "ch", QSizeF(32, 96), 0);

    FileFactory ff(&mw);
    ff.fileName = kBlankPath;
    FileFactory::FileError err = ff.save(FileFactory::Version_1_2);

    QCOMPARE(static_cast<int>(err), static_cast<int>(FileFactory::No_Error));
    QVERIFY(QFile::exists(kBlankPath));
    QVERIFY(QFileInfo(kBlankPath).size() > 0);
}

void TestFileFactory::generateBasicV2_ifMissing()
{
    if (QFile::exists(kBasicPath)) {
        QVERIFY2(QFileInfo(kBasicPath).size() > 0,
                 "basic_v2.crochetcharts exists but is empty");
        return;
    }

    TestMW mw;
    CrochetTab* tab = mw.makeTab(Scene::Rows);
    QVERIFY(tab != 0);
    mw.tabs()->addTab(tab, "BasicChart");
    tab->createChart(Scene::Rows, 1, 3, "ch", QSizeF(32, 96), 0);

    FileFactory ff(&mw);
    ff.fileName = kBasicPath;
    FileFactory::FileError err = ff.save(FileFactory::Version_1_2);

    QCOMPARE(static_cast<int>(err), static_cast<int>(FileFactory::No_Error));
    QVERIFY(QFile::exists(kBasicPath));
    QVERIFY(QFileInfo(kBasicPath).size() > 0);
}

void TestFileFactory::generateBasicV2Snapshot_ifMissing()
{
    if (QFile::exists(kBasicSnapshotPath)) {
        QVERIFY2(QFileInfo(kBasicSnapshotPath).size() > 0,
                 "basic_v2.snapshot.txt exists but is empty");
        return;
    }

    QVERIFY(QFile::exists(kBasicPath));

    TestMW mw;
    FileFactory ff(&mw);
    ff.fileName = kBasicPath;
    QCOMPARE(static_cast<int>(ff.load()), static_cast<int>(FileFactory::No_Error));

    CrochetTab* tab = firstTab(mw);
    QVERIFY(tab != 0);

    const QString dump = dumpScene(tab->scene());
    QVERIFY2(writeText(kBasicSnapshotPath, dump),
             qPrintable("failed to write " + kBasicSnapshotPath));
    QVERIFY(QFileInfo(kBasicSnapshotPath).size() > 0);
}

void TestFileFactory::loadBlankV2_preservesEmpty()
{
    QVERIFY2(QFile::exists(kBlankPath),
             "blank_v2.crochetcharts missing — run generateBlankV2_ifMissing first");

    TestMW mw;
    FileFactory ff(&mw);
    ff.fileName = kBlankPath;
    FileFactory::FileError err = ff.load();

    QCOMPARE(static_cast<int>(err), static_cast<int>(FileFactory::No_Error));
    QCOMPARE(mw.tabs()->count(), 1);

    CrochetTab* tab = firstTab(mw);
    QVERIFY(tab != 0);
    QCOMPARE(cellCount(tab->scene()), 0);
    QCOMPARE(dumpScene(tab->scene()), QString());
}

void TestFileFactory::loadBasicV2_preservesStructure()
{
    QVERIFY2(QFile::exists(kBasicPath),
             "basic_v2.crochetcharts missing — run generateBasicV2_ifMissing first");
    QVERIFY2(QFile::exists(kBasicSnapshotPath),
             "basic_v2.snapshot.txt missing — run generateBasicV2Snapshot_ifMissing first");

    TestMW mw;
    FileFactory ff(&mw);
    ff.fileName = kBasicPath;
    QCOMPARE(static_cast<int>(ff.load()), static_cast<int>(FileFactory::No_Error));
    QCOMPARE(mw.tabs()->count(), 1);

    CrochetTab* tab = firstTab(mw);
    QVERIFY(tab != 0);
    QCOMPARE(cellCount(tab->scene()), 3);

    const QString actual = dumpScene(tab->scene());
    const QString expected = readText(kBasicSnapshotPath);
    QCOMPARE(actual, expected);
}

void TestFileFactory::roundTrip_basicV2_preservesSnapshot()
{
    QVERIFY(QFile::exists(kBasicPath));

    TestMW mw1;
    FileFactory ff1(&mw1);
    ff1.fileName = kBasicPath;
    QCOMPARE(static_cast<int>(ff1.load()), static_cast<int>(FileFactory::No_Error));
    CrochetTab* tab1 = firstTab(mw1);
    QVERIFY(tab1 != 0);
    const QString dumpBefore = dumpScene(tab1->scene());

    const QString tmpPath = QDir::tempPath() + "/basic_v2_roundtrip.crochetcharts";
    QFile::remove(tmpPath);
    ff1.fileName = tmpPath;
    QCOMPARE(static_cast<int>(ff1.save(FileFactory::Version_1_2)),
             static_cast<int>(FileFactory::No_Error));
    QVERIFY(QFile::exists(tmpPath));

    TestMW mw2;
    FileFactory ff2(&mw2);
    ff2.fileName = tmpPath;
    QCOMPARE(static_cast<int>(ff2.load()), static_cast<int>(FileFactory::No_Error));
    CrochetTab* tab2 = firstTab(mw2);
    QVERIFY(tab2 != 0);
    const QString dumpAfter = dumpScene(tab2->scene());

    QCOMPARE(dumpAfter, dumpBefore);
    QFile::remove(tmpPath);
}

void TestFileFactory::generateLegacyV1_ifMissing()
{
    if (QFile::exists(kLegacyPath)) {
        QVERIFY2(QFileInfo(kLegacyPath).size() > 0,
                 "legacy_v1.crochetcharts exists but is empty");
        return;
    }

    TestMW mw;
    CrochetTab* tab = mw.makeTab(Scene::Rows);
    QVERIFY(tab != 0);
    mw.tabs()->addTab(tab, "LegacyChart");
    tab->createChart(Scene::Rows, 1, 3, "ch", QSizeF(32, 96), 0);

    FileFactory ff(&mw);
    ff.fileName = kLegacyPath;
    FileFactory::FileError err = ff.save(FileFactory::Version_1_0);

    QCOMPARE(static_cast<int>(err), static_cast<int>(FileFactory::No_Error));
    QVERIFY(QFile::exists(kLegacyPath));
    QVERIFY(QFileInfo(kLegacyPath).size() > 0);
}

void TestFileFactory::generateLegacyV1Snapshot_ifMissing()
{
    if (QFile::exists(kLegacySnapshotPath)) {
        QVERIFY2(QFileInfo(kLegacySnapshotPath).size() > 0,
                 "legacy_v1.snapshot.txt exists but is empty");
        return;
    }

    QVERIFY(QFile::exists(kLegacyPath));

    TestMW mw;
    FileFactory ff(&mw);
    ff.fileName = kLegacyPath;
    QCOMPARE(static_cast<int>(ff.load()), static_cast<int>(FileFactory::No_Error));

    CrochetTab* tab = firstTab(mw);
    QVERIFY(tab != 0);

    const QString dump = dumpScene(tab->scene());
    QVERIFY2(writeText(kLegacySnapshotPath, dump),
             qPrintable("failed to write " + kLegacySnapshotPath));
    QVERIFY(QFileInfo(kLegacySnapshotPath).size() > 0);
}

void TestFileFactory::loadLegacyV1_preservesStructure()
{
    QVERIFY2(QFile::exists(kLegacyPath),
             "legacy_v1.crochetcharts missing \u2014 run generateLegacyV1_ifMissing first");
    QVERIFY2(QFile::exists(kLegacySnapshotPath),
             "legacy_v1.snapshot.txt missing \u2014 run generateLegacyV1Snapshot_ifMissing first");

    TestMW mw;
    FileFactory ff(&mw);
    ff.fileName = kLegacyPath;
    QCOMPARE(static_cast<int>(ff.load()), static_cast<int>(FileFactory::No_Error));
    QCOMPARE(mw.tabs()->count(), 1);

    CrochetTab* tab = firstTab(mw);
    QVERIFY(tab != 0);
    QCOMPARE(cellCount(tab->scene()), 3);

    const QString actual = dumpScene(tab->scene());
    const QString expected = readText(kLegacySnapshotPath);
    QCOMPARE(actual, expected);
}

void TestFileFactory::roundTrip_legacyV1_preservesSnapshot()
{
    QVERIFY(QFile::exists(kLegacyPath));

    TestMW mw1;
    FileFactory ff1(&mw1);
    ff1.fileName = kLegacyPath;
    QCOMPARE(static_cast<int>(ff1.load()), static_cast<int>(FileFactory::No_Error));
    CrochetTab* tab1 = firstTab(mw1);
    QVERIFY(tab1 != 0);
    const QString dumpBefore = dumpScene(tab1->scene());

    const QString tmpPath = QDir::tempPath() + "/legacy_v1_roundtrip.crochetcharts";
    QFile::remove(tmpPath);
    ff1.fileName = tmpPath;
    QCOMPARE(static_cast<int>(ff1.save(FileFactory::Version_1_0)),
             static_cast<int>(FileFactory::No_Error));
    QVERIFY(QFile::exists(tmpPath));

    TestMW mw2;
    FileFactory ff2(&mw2);
    ff2.fileName = tmpPath;
    QCOMPARE(static_cast<int>(ff2.load()), static_cast<int>(FileFactory::No_Error));
    CrochetTab* tab2 = firstTab(mw2);
    QVERIFY(tab2 != 0);
    const QString dumpAfter = dumpScene(tab2->scene());

    QCOMPARE(dumpAfter, dumpBefore);
    QFile::remove(tmpPath);
}

void TestFileFactory::cleanupTestCase()
{
}
