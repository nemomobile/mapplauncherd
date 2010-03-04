/*
 * ut_daemon.cpp
 *
 * This file is part of applauncherd
 *
 * Copyright (C) 2010 Nokia Corporation. All rights reserved.
 *
 * This software, including documentation, is protected by copyright
 * controlled by Nokia Corporation. All rights are reserved.
 * Copying, including reproducing, storing, adapting or translating,
 * any or all of this material requires the prior written consent of
 * Nokia Corporation. This material also contains confidential
 * information which may not be disclosed to others without the prior
 * written consent of Nokia.
 */

#include "ut_daemon.h"
#include "daemon.h"

Ut_Daemon::Ut_Daemon()
{
    int argc = 3;
    char **argv    = new char * [argc];
    argv[0] = strdup("app");
    argv[1] = strdup("--testParameter");
    argv[2] = strdup("--123");
    m_subject.reset(new Daemon( argc, argv ));
}

Ut_Daemon::~Ut_Daemon()
{
}

void Ut_Daemon::initTestCase()
{}

void Ut_Daemon::cleanupTestCase()
{}

void Ut_Daemon::testInitialArguments()
{
    QVERIFY2(m_subject->initialArgc == 3, "Failure");
    QCOMPARE(m_subject->initialArgv[0], "app");
    QCOMPARE(m_subject->initialArgv[1], "--testParameter");
    QCOMPARE(m_subject->initialArgv[2], "--123");
}


void Ut_Daemon::testParseArgs()
{
    int argc = 4;
    char **argv    = new char * [argc];

    argv[0] = strdup("app");
    argv[1] = strdup("--daemon");
    argv[2] = strdup("--quiet");
    argv[3] = strdup("--test");

    QVERIFY2(m_subject->daemon == false, "Failure");
    QVERIFY2(m_subject->quiet == false, "Failure");
    QVERIFY2(m_subject->testMode == false, "Failure");

    m_subject->parseArgs(vector<string>(argv, argv + argc));

    QVERIFY2(m_subject->daemon == true, "Failure");
    QVERIFY2(m_subject->quiet == true, "Failure");
    QVERIFY2(m_subject->testMode == true, "Failure");
}

void Ut_Daemon::testVerifyInstance()
{
    QVERIFY2(m_subject.get() == Daemon::instance(), "Failure");
}

void Ut_Daemon::testReapZombies()
{
    QVERIFY2(m_subject->m_children.size() == 0, "Failure");

    for (int i=234234; i<234245; i++) {
        m_subject->m_children.push_back(i);
    }

    QVERIFY2(m_subject->m_children.size() == 11, "Failure");

    m_subject->reapZombies();

    QVERIFY2(m_subject->m_children.size() == 0, "Failure");
}


QTEST_APPLESS_MAIN(Ut_Daemon);

