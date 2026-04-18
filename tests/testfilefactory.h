// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Stefan Dieringer <stefan.dieringer@googlemail.com>

#ifndef TESTFILEFACTORY_H
#define TESTFILEFACTORY_H

#include <QtTest/QTest>
#include <QDebug>
#include <QObject>

class TestFileFactory : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void portFixturesDirReachable();
    void mainWindowConstructs();
    void generateBlankV2_ifMissing();
    void generateBasicV2_ifMissing();
    void generateBasicV2Snapshot_ifMissing();
    void loadBlankV2_preservesEmpty();
    void loadBasicV2_preservesStructure();
    void roundTrip_basicV2_preservesSnapshot();
    void generateLegacyV1_ifMissing();
    void generateLegacyV1Snapshot_ifMissing();
    void loadLegacyV1_preservesStructure();
    void roundTrip_legacyV1_preservesSnapshot();
    void cleanupTestCase();
};

#endif // TESTFILEFACTORY_H
