/*
 * ut_daemon.h
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

#ifndef UT_DAEMON_H
#define UT_DAEMON_H

#include<QtTest/QtTest>
#include<QObject>

#include <tr1/memory>

#define UNIT_TEST

class Daemon;

class Ut_Daemon : public QObject
{
    Q_OBJECT

public:
    Ut_Daemon();
    ~Ut_Daemon();
private Q_SLOTS:

    void initTestCase();
    void cleanupTestCase();
    void testInitialArguments();
    void testParseArgs();
    void testVerifyInstance();
    void testReapZombies();

private:
    std::tr1::shared_ptr<Daemon> m_subject;
};

#endif // UT_DAEMON_H


