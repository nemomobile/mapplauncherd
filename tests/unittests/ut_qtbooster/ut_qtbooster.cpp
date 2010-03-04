/*
 * ut_qtbooster.cpp
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

#include "ut_qtbooster.h"
#include "qtbooster.h"

Ut_QtBooster::Ut_QtBooster() :
    m_subject(new QtBooster)
{}

Ut_QtBooster::~Ut_QtBooster()
{}

void Ut_QtBooster::initTestCase()
{}

void Ut_QtBooster::cleanupTestCase()
{}

void Ut_QtBooster::testSocketName()
{
    QVERIFY2(QtBooster::socketName() == QtBooster::m_socketId, "Failure");
    QVERIFY2(m_subject->socketId() == QtBooster::m_socketId, "Failure");
}

void Ut_QtBooster::testType()
{
    QVERIFY2(QtBooster::type() == 'q', "Failure");
    QVERIFY2(m_subject->boosterType() == 'q', "Failure");
}

QTEST_APPLESS_MAIN(Ut_QtBooster);

