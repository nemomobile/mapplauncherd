/*
 * daemon.h
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

#ifndef DAEMON_H
#define DAEMON_H

#include <vector>
#include <string>
#include <sys/types.h>

using std::vector;
using std::string;

/*!
 * \class Daemon.
 * \brief Daemon wraps up the daemonizing functionality.
 *
 * Daemon wraps up the daemonizing functionality and is the
 * main object of the launcher program. It runs the main loop of the
 * application, listens connections from the invoker and forks Booster
 * processes.
 */
class Daemon
{
public:

    /*!
     * \brief Constructor
     * \param argc Argument count delivered to main()
     * \param argv Argument array delivered to main()
     *
     * Supported arguments:
     * --daemon == daemonize
     * --quiet  == quiet
     * --help   == print usage
     */
    Daemon(int & argc, char * argv[]);

    /*!
     * \brief Destructor
     */
    virtual ~Daemon();

    /*!
     * \brief Run main loop and fork Boosters.
     */
    void run();

    /*! \brief Return the one-and-only Daemon instance.
     * \return Pointer to the Daemon instance.
     */
    static Daemon * instance();

    //! \brief Reapes children processes gone zombies (finished Boosters).
    void reapZombies();

private:

    //! Disable copy-constructor
    Daemon(const Daemon & r);

    //! Disable assignment operator
    Daemon & operator= (const Daemon & r);

    //! Parse arguments
    void parseArgs(const vector<string> & args);

    //! Fork to a daemon
    void daemonize();

    //! Print usage
    void usage() const;

    //! Forks and initializes a new Booster
    bool forkBooster(char type, int pipefd[2]);

    //! Don't use console for output
    void consoleQuiet();

    //! Test mode flag
    bool m_testMode;

    //! Daemonize flag
    bool m_daemon;

    //! Debug print flag
    bool m_quiet;

    //! Vector of current child PID's
    vector<pid_t> m_children;

    int    m_initialArgc;
    char** m_initialArgv;

    static Daemon * m_instance;

#ifdef UNIT_TEST
    friend class Ut_Daemon;
#endif
};

#endif // DAEMON_H
