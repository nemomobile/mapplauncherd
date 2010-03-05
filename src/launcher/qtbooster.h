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
