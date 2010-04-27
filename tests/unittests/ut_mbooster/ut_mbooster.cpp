/*
 * ut_mbooster.cpp
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

#include "ut_mbooster.h"
#include "mbooster.h"

Ut_MBooster::Ut_MBooster() :
    m_subject(new MBooster)
{}

Ut_MBooster::~Ut_MBooster()
{}

void Ut_MBooster::initTestCase()
{}

void Ut_MBooster::cleanupTestCase()
{}

void Ut_MBooster::testSocketName()
{
    QVERIFY2(MBooster::socketName() == MBooster::m_socketId, "Failure");
    QVERIFY2(m_subject->socketId() == MBooster::m_socketId, "Failure");
}

void Ut_MBooster::testType()
{
    QVERIFY2(MBooster::type() == 'm', "Failure");
    QVERIFY2(m_subject->boosterType() == 'm', "Failure");
}

QTEST_APPLESS_MAIN(Ut_MBooster);

