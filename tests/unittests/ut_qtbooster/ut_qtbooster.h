/*
 * ut_qtbooster.h
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

#ifndef UT_QTBOOSTER_H
#define UT_QTBOOSTER_H

#include<QtTest/QtTest>
#include<QObject>

#include <tr1/memory>

#define UNIT_TEST

class QtBooster;

class Ut_QtBooster : public QObject
{
    Q_OBJECT

public:
    Ut_QtBooster();
    virtual ~Ut_QtBooster();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testSocketName();
    void testType();

private:
    std::tr1::shared_ptr<QtBooster> m_subject;
};

#endif // UT_QTBOOSTER_H


