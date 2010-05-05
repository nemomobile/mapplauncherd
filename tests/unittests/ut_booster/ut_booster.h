/*
 * ut_booster.h
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

#ifndef UT_BOOSTER_H
#define UT_BOOSTER_H

#include<QtTest/QtTest>
#include<QObject>

#include <tr1/memory>

#define UNIT_TEST

class Booster;

class Ut_Booster : public QObject
{
    Q_OBJECT

public:
    Ut_Booster();
    virtual ~Ut_Booster();

private Q_SLOTS:
    void initTestCase();
    void testRenameProcess();
    void testRenameProcessNotEnoughSpace();
    void testRenameProcessNotEnoughSpace2();
    void testRenameProcessNotEnoughSpace3();
    void testRenameProcessNotEnoughSpace4();
    void testRenameProcessNoArgs();
    void testRenameBoosterProcess();
    void cleanupTestCase();

private:

    char ** packTwoArgs(const char * arg0, const char * arg1);
    std::tr1::shared_ptr<Booster> m_subject;
};

#endif // UT_BOOSTER_H


