/*
 * daemon.cpp
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

#include "daemon.h"
#include "logger.h"
#include "connection.h"
#include "booster.h"
#include "duibooster.h"
#include "qtbooster.h"

#include <cstdlib>
#include <errno.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/prctl.h>

#include <signal.h>

#include <fcntl.h>
#include <iostream>

Daemon * Daemon::m_instance = NULL;

Daemon::Daemon(int & argc, char * argv[]) :
    testMode(false),
    daemon(false),
    quiet(false)
{
    if (!Daemon::m_instance)
    {
        Daemon::m_instance = this;
    }
    else
    {
        std::cerr << "Daemon already created!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Parse arguments
    parseArgs(vector<string>(argv, argv + argc));

    // Disable console output
    if (quiet)
        consoleQuiet();

    // Store arguments list
    initialArgv = argv;
    initialArgc = argc;

    // Daemonize if desired
    if (daemon)
    {
        daemonize();
    }
}

void Daemon::consoleQuiet()
{
    close(0);
    close(1);
    close(2);

    if (open("/dev/null", O_RDONLY) < 0)
        Logger::logErrorAndDie(EXIT_FAILURE, "opening /dev/null readonly");

    if (dup(open("/dev/null", O_WRONLY)) < 0)
        Logger::logErrorAndDie(EXIT_FAILURE, "opening /dev/null writeonly");
}


Daemon * Daemon::instance()
{
    return Daemon::m_instance;
}

Daemon::~Daemon()
{}

void Daemon::run()
{
    // create sockets for each of the boosters
    Connection::initSocket(DuiBooster::socketName());
    Connection::initSocket(QtBooster::socketName());

    // Pipe used to tell the parent that a new
    // booster is needed
    int pipefd[2];
    if (pipe(pipefd) == -1)
    {
        Logger::logErrorAndDie(EXIT_FAILURE, "Creating a pipe failed!!!\n");
    }

    forkBooster(DuiBooster::type(), pipefd);
    forkBooster(QtBooster::type(),  pipefd);

    while (true)
    {
        // Wait for something appearing in the pipe
        char msg;
        read(pipefd[0], reinterpret_cast<void *>(&msg), 1);

        // Guarantee some time for the just launched application to
        // start up before forking new booster. Not doing this would
        // slow down the start-up significantly on single core CPUs.
        sleep(2);

        // Fork a new booster of the given type
        forkBooster(msg, pipefd);
    }
}

bool Daemon::forkBooster(char type, int pipefd[2])
{
    // Fork a new process
    pid_t newPid = fork();

    if (newPid == -1)
        Logger::logErrorAndDie(EXIT_FAILURE, "Forking while invoking");

    if (newPid == 0) /* Child process */
    {
        // Reset used signal handlers
        signal(SIGCHLD, SIG_DFL);

        // Will get this signal if applauncherd dies
        prctl(PR_SET_PDEATHSIG, SIGHUP);

        // Close unused read end
        close(pipefd[0]);

        if (setsid() < 0)
        {
            Logger::logError("Setting session id\n");
        }

        Logger::logNotice("Running a new Booster of %c type...", type);

        // Create a new booster and initialize it
        Booster* booster;
        if (DuiBooster::type() == type)
        {
            booster = new DuiBooster();
        }
        else if (QtBooster::type() == type)
        {
            booster = new QtBooster();
        }
        else
        {
            Logger::logErrorAndDie(EXIT_FAILURE, "Unknown booster type \n");
        }

        // Preload stuff
        booster->preload();

        // Clean-up all the env variables
        clearenv();

        // Rename launcher process to booster
        booster->renameProcess(initialArgc, initialArgv);

        Logger::logNotice("Wait for message from invoker");

        // Wait and read commands from the invoker
        booster->readCommand();

        // Give to the process an application specific name
        booster->renameProcess(initialArgc, initialArgv);

        // Signal the parent process that it can create a new
        // waiting booster process and close write end
        const char msg = booster->boosterType();
        write(pipefd[1], reinterpret_cast<const void *>(&msg), 1);
        close(pipefd[1]);

        // Don't care about fate of parent applauncherd process any more
        prctl(PR_SET_PDEATHSIG, 0);

        // Run the current Booster
        booster->run();

        // Finish
        delete booster;
        exit(EXIT_SUCCESS);
    }
    else /* Parent process */
    {
        // Store the pid so that we can reap it later
        m_children.push_back(newPid);
    }

    return true;
}

void Daemon::reapZombies()
{
    vector<pid_t>::iterator i(m_children.begin());
    while (i != m_children.end())
    {
        if (waitpid(*i, NULL, WNOHANG))
        {
            i = m_children.erase(i);
        }
        else
        {
            i++;
        }
    }
}

void Daemon::daemonize()
{
    // Our process ID and Session ID
    pid_t pid, sid;

    // Fork off the parent process: first fork
    pid = fork();
    if (pid < 0)
    {
        Logger::logError("Unable to fork daemon, code %d (%s)", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    // If we got a good PID, then we can exit the parent process.
    if (pid > 0)
    {
        exit(EXIT_SUCCESS);
    }

    // Fork off the parent process: second fork
    pid = fork();
    if (pid < 0)
    {
        Logger::logError("Unable to fork daemon, code %d (%s)", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    // If we got a good PID, then we can exit the parent process.
    if (pid > 0)
    {
        exit(EXIT_SUCCESS);
    }

    // Change the file mode mask
    umask(0);

    // Open any logs here

    // Create a new SID for the child process
    sid = setsid();
    if (sid < 0)
    {
        Logger::logError("Unable to create a new session, code %d (%s)", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Change the current working directory
    if ((chdir("/")) < 0)
    {
        Logger::logError("Unable to change directory to %s, code %d (%s)", "/", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Open file descriptors pointing to /dev/null
    const int new_stdin  = open("/dev/null", O_RDONLY);
    const int new_stdout = open("/dev/null", O_WRONLY);
    const int new_stderr = open("/dev/null", O_WRONLY);

    // Redirect standard file descriptors to /dev/null
    dup2(new_stdin,  STDIN_FILENO);
    dup2(new_stdout, STDOUT_FILENO);
    dup2(new_stderr, STDERR_FILENO);

    // Close new file descriptors
    close(new_stdin);
    close(new_stdout);
    close(new_stderr);
}

void Daemon::usage() const
{
    std::cout << "Usage: "<< PROG_NAME << " [options]\n"
              << "\n"
              << "Options:\n"
              << "  --daemon            Fork and go into the background.\n"
              //<< "  --pidfile FILE      Specify a different pid file (default " << LAUNCHER_PIDFILE << " ).\n"
              //<< "  --send-app-died     Send application died signal.\n"
              << "  --quiet             Do not print anything.\n"
              << "  --help              Print this help message.\n"
              << "\n"
              << "Use the invoker to start a <shared object> from the launcher.\n"
              << "Where <shared object> is a binary including a 'main' symbol.\n"
              << "Note that the binary needs to be linked with -shared or -pie.\n";

    exit(EXIT_SUCCESS);
}

void Daemon::parseArgs(const vector<string> & args)
{
    for (vector<string>::const_iterator i(args.begin()); i != args.end(); i++)
    {
        if ((*i) == "--help")
        {
            usage();
        }
        else if ((*i) == "--daemon")
        {
            daemon = true;
        }
        else if  ((*i) ==  "--quiet")
        {
            quiet = true;
        }
        else if ((*i) == "--test")
        {
            testMode = true;
        }
    }
}
