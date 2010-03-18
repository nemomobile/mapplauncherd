/*
 * qtbooster.h
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

#ifndef QTBOOSTER_H
#define QTBOOSTER_H

#include "booster.h"

/*!
 * \class QtBooster.
 * \brief Qt-specific version of the Booster.
 */
class QtBooster : public Booster
{
public:

    //! Constructor.
    QtBooster();

    //! Destructor.
    virtual ~QtBooster();

    /*!
     * \brief Return the socket name common to all QtBooster objects.
     * \return Path to the socket file.
     */
    static const string & socketName();

    //! \reimp
    virtual char boosterType() const { return type(); }

    /*!
     * \brief Return a unique character ('q') represtenting the type of QtBoosters.
     * \return Type character.
     */
    static char type();

    //! \reimp
    virtual bool preload();

protected:

    //! \reimp
    virtual const string & socketId() const;

private:

    //! Disable copy-constructor
    QtBooster(const QtBooster & r);

    //! Disable assignment operator
    QtBooster & operator= (const QtBooster & r);

    static const string m_socketId;

#ifdef UNIT_TEST
    friend class Ut_QtBooster;
#endif
};

#endif //QTBOOSTER_H
