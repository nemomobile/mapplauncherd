/*
 * mbooster.cpp
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

#include "mbooster.h"

#ifdef HAVE_MCOMPONENTCACHE
#include <mcomponentcache.h>
#endif

const string MBooster::m_socketId  = "/tmp/mlnchr";

MBooster::MBooster()
{
}

MBooster::~MBooster()
{
}

const string & MBooster::socketId() const
{
    return m_socketId;
}

bool MBooster::preload()
{
#ifdef HAVE_MCOMPONENTCACHE
    MComponentCache::populateForMApplication();
#endif
    return true;
}

const string & MBooster::socketName()
{
    return m_socketId;
}

char MBooster::type()
{
    return 'm';
}
