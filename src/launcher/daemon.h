/*
 * daemon.h
 *
 * This file is part of launcher
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

class Daemon
{
public:

    /*!
     * \brief Construct Daemon object in particular mode.
     */
    Daemon(int & argc, char * argv[]);

    /*!
     * \brief Destroy Daemon object.
     */
    virtual ~Daemon();

    /*!
     * \brief Run everything
     */
    void run();

    //! Return one-and-only Daemon instance
    static Daemon * instance();

    //! Reapes children gone zombies
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

    //! Test mode flag
    bool testMode;

    //! Daemonize flag
    bool daemon;

    //! Debug print flag
    bool quiet;

    //! Vector of current children PID's
    vector<pid_t> m_children;

    // main process args list
    int    initialArgc;
    char** initialArgv;

    static Daemon * m_instance;
};

#endif // DAEMON_H
