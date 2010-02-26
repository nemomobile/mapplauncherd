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

    Essentially DuiBooster initializes DuiComponentCache with fresh objects.
 */
class DuiBooster : public Booster
{
public:

    /*!
     * \brief Constructor
     */
    DuiBooster();

    /*!
     * \brief Destructor
     */
    virtual ~DuiBooster();

    //! \reimp
    virtual bool preload();

    //! Return the socket name
    static const string socketName();

    //! \reimp
    virtual char boosterType() const { return type(); }

    //! Static method for getting the type
    static char type();

protected:

    virtual const string socketId() const;

private:

    static const string soketId;

};

#endif //DUIBOOSTER_H
