/*
 * daemon.h
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
    bool testMode;

    //! Daemonize flag
    bool daemon;

    //! Debug print flag
    bool quiet;

    //! Vector of current child PID's
    vector<pid_t> m_children;

    int    initialArgc;
    char** initialArgv;

    static Daemon * m_instance;

#ifdef UNIT_TEST
    friend class Ut_Daemon;
#endif
};

#endif // DAEMON_H
