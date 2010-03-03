/*
 * duibooster.cpp
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

#include "duibooster.h"

#ifdef HAVE_DUI
#include <duicomponentcache.h>
#endif

const string DuiBooster::m_socketId  = "/tmp/duilnchr";

DuiBooster::DuiBooster()
{
}

DuiBooster::~DuiBooster()
{
}

const string & DuiBooster::socketId() const
{
    return m_socketId;
}

bool DuiBooster::preload()
{
#ifdef HAVE_DUI
    DuiComponentCache::populate();
#endif
    return true;
}

const string & DuiBooster::socketName()
{
    return m_socketId;
}

char DuiBooster::type()
{
    return 'd';
}
