/*
 * ut_connection.h
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

#ifndef UT_CONNECTION_H
#define UT_CONNECTION_H

#include<QtTest/QtTest>
#include<QObject>

#include <tr1/memory>

#define UNIT_TEST

class Connection;

class Ut_Connection : public QObject
{
    Q_OBJECT

public:
    Ut_Connection();
    ~Ut_Connection();

private Q_SLOTS:

    void initTestCase();
    void cleanupTestCase();
    void testInitConnection();
    void testAcceptConnection();
    void testGetEnv();
    void testGetAppName();

private:
    std::tr1::shared_ptr<Connection> m_subject;
};

#endif // UT_CONNECTION_H
