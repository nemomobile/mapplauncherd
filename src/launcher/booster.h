/*
 * booster.h
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

#ifndef BOOSTER_H
#define BOOSTER_H

#include <cstdlib>
#include <string>

using std::string;

#include "appdata.h"
#include "connection.h"

/*!
    \class Booster
    \brief Base class for all boosters (Qt-booster, DUI-booster and so on..)

    Booster is a class that is used to initialize certain resources in libraries
    common to all applications of a type.

    Booster also communicates with the invoker process and handles the actual
    jump to the main() -method of the application to be launched.

    Booster instance dies with the launched application and a new one must be created
    in advance so as to launch a new application.
 */
class Booster
{
public:

    /*!
     * \brief Constructor
     */
    Booster();

    /*!
     * \brief Destructor
     */
    virtual ~Booster();

    /*!
     * \brief Run the application to be invoked
     */
    void run();

    /*!
     * \brief Wait for connection from invoker and read the input
     */
    bool readCommand();

    /*!
     * \brief Initialize and preload stuff
     */
    virtual bool preload();

    /*!
     * \brief Rename process
     */
    void renameProcess(int initialArgc, char** initialArgv);

    /*! \brief Return booster type common to all instances
     *  This is used in the simple communication between booster process
     *  and the daemon.
     */
    virtual char boosterType() const = 0;

protected:

    virtual const string & socketId() const = 0;

private:

    void complainAndExit();
    void launchProcess();
    void loadMain();

    //! Application data read from the invoker
    AppData m_app;
};

#endif // BOOSTER_H
