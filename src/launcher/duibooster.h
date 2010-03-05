/*
 * duibooster.h
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

    static const string m_socketId;

#ifdef UNIT_TEST
    friend class Ut_DuiBooster;
#endif
};

#endif //DUIBOOSTER_H
