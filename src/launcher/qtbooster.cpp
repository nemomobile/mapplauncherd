/*
 * qtbooster.cpp
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

#include "qtbooster.h"

const string QtBooster::m_socketId = "/tmp/qtlnchr";

QtBooster::QtBooster()
{
}

QtBooster::~QtBooster()
{
}

const string & QtBooster::socketId() const
{
    return m_socketId;
}

const string & QtBooster::socketName()
{
    return m_socketId;
}

char QtBooster::type()
{
    return 'q';
}

bool QtBooster::preload()
{
    return true;
}
