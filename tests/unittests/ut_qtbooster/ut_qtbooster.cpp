/*
 * ut_qtbooster.cpp
 *
 * This file is part of applauncherd
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
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

