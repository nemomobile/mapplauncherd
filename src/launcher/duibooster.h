/*
 * duibooster.h
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

#ifndef DUIBOOSTER_H
#define DUIBOOSTER_H

#include "booster.h"

/*!
    \class DuiBooster
    \brief Dui-specific version of the Booster

    DuiBooster effectively fills DuiComponentCache with fresh objects.
    DUI applications can then try to use already initialized objects from DuiComponentCache.
    This can significantly reduce the startup time of a DUI application.
 */
class DuiBooster : public Booster
{
public:

    //! \brief Constructor
    DuiBooster();

    //! \brief Destructor
    virtual ~DuiBooster();

    //! \reimp
    virtual bool preload();

    /*!
     * \brief Return the socket name common to all DuiBooster objects.
     * \return Path to the socket file.
     */
    static const string & socketName();

    //! \reimp
    virtual char boosterType() const { return type(); }

    /*!
     * \brief Return a unique character ('d') represtenting the type of DuiBoosters.
     * \return Type character.
     */
    static char type();

protected:

    //! \reimp
    virtual const string & socketId() const;

private:

    //! Disable copy-constructor
    DuiBooster(const DuiBooster & r);

    //! Disable assignment operator
    DuiBooster & operator= (const DuiBooster & r);

    static const string m_socketId;

#ifdef UNIT_TEST
    friend class Ut_DuiBooster;
#endif
};

#endif //DUIBOOSTER_H
