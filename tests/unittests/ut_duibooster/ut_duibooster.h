/*
 * ut_duibooster.h
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

#ifndef UT_DUIBOOSTER_H
#define UT_DUIBOOSTER_H

#include<QtTest/QtTest>
#include<QObject>

#include <tr1/memory>

#define UNIT_TEST

class DuiBooster;

class Ut_DuiBooster : public QObject
{
    Q_OBJECT

public:
    Ut_DuiBooster();
    virtual ~Ut_DuiBooster();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testSocketName();
    void testType();

private:
    std::tr1::shared_ptr<DuiBooster> m_subject;
};

#endif // UT_DUIBOOSTER_H


