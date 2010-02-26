/*
 * qtbooster.h
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

#ifndef QTBOOSTER_H
#define QTBOOSTER_H

#include "booster.h"

/*!
    \class QtBooster
    \brief Qt-specific version of the Booster

 */
class QtBooster : public Booster
{
public:
    /*!
     * \brief Construct QtBooster object.
     */
    QtBooster();

    /*!
     * \brief Destroy QtBooster object.
     */
    virtual ~QtBooster();

    //! Return the socket name
    static const string socketName();

    //! \reimp
    virtual char boosterType() const { return type(); }

    //! Static method for getting the type
    static char type();

    /*!
     * \brief Initialize and preload stuff
     */
    virtual bool preload();

protected:

    virtual const string socketId() const;

private:

    static const string soketId;
};

#endif //QTBOOSTER_H
