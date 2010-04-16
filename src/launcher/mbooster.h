/*
 * mbooster.h
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

#ifndef MBOOSTER_H
#define MBOOSTER_H

#include "booster.h"

/*!
    \class MBooster
    \brief MeeGo Touch -specific version of the Booster.

    MBooster effectively fills MComponentCache with fresh objects.
    MeeGo Touch applications can then try to use already initialized objects 
    from MComponentCache. This can significantly reduce the startup time of a 
    MeeGo Touch application.
 */
class MBooster : public Booster
{
public:

    //! \brief Constructor
    MBooster();

    //! \brief Destructor
    virtual ~MBooster();

    //! \reimp
    virtual bool preload();

    /*!
     * \brief Return the socket name common to all MBooster objects.
     * \return Path to the socket file.
     */
    static const string & socketName();

    //! \reimp
    virtual char boosterType() const { return type(); }

    /*!
     * \brief Return a unique character ('d') represtenting the type of MBoosters.
     * \return Type character.
     */
    static char type();

protected:

    //! \reimp
    virtual const string & socketId() const;

private:

    //! Disable copy-constructor
    MBooster(const MBooster & r);

    //! Disable assignment operator
    MBooster & operator= (const MBooster & r);

    static const string m_socketId;

#ifdef UNIT_TEST
    friend class Ut_MBooster;
#endif
};

#endif // MBOOSTER_H
