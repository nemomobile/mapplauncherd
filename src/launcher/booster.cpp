/*
 * booster.cpp
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

#include "booster.h"
#include "connection.h"
#include "logger.h"

#include <cstdlib>
#include <dlfcn.h>
#include <sys/prctl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/user.h>

extern char **environ;

Booster::Booster()
{}

Booster::~Booster()
{}

bool Booster::preload()
{
    return true;
}

bool Booster::readCommand()
{
    // Setup the conversation channel with the invoker.
    Connection conn(socketId());

    // Accept a new invocation.
    if (!conn.acceptConn())
        return false;

    // Read magic number
    m_app.options = conn.getMagic();
    if (m_app.options == -1)
        return false;

    // Read application name
    m_app.appName = conn.getAppName();
    if (m_app.appName.empty())
        return false;

    // Read application parameters
    if (conn.receiveActions())
    {
        m_app.fileName = conn.fileName();
        m_app.argc     = conn.argc();
        m_app.argv     = conn.argv();

        for(int i = 0; i < 3; i++)
            m_app.ioDescriptors[i] = conn.ioDescriptors()[i];
    }
    else
    {
        return false;
    }

    // Close connection
    conn.closeConn();

    return true;
}

void Booster::run()
{
    if (!m_app.fileName.empty())
    {
        Logger::logInfo("invoking '%s' ", m_app.fileName.c_str());
        launchProcess();
    }
    else
    {
        Logger::logError("nothing to invoke\n");
    }
}

void Booster::renameProcess(int parentArgc, char** parentArgv)
{
    Logger::logInfo("set new process name: '%s' ", basename(m_app.argv[0]));

    if (parentArgc < 2)
    {
        // This code copies the new process name to the original argv[0].
        // If the new name won't fit, then it'll be cut. This is used to
        // "fool" e.g. top and ps to show the correct name. Otherwise they
        // would show the name of the launcher itself.

        int min = std::min(strlen(parentArgv[0]), strlen(m_app.argv[0]));
        if (min)
        {
            memset(parentArgv[0], '\0', strlen(parentArgv[0]));
            memmove(parentArgv[0], m_app.argv[0], min);
        }
    }
    else
    {
        // This code copies all the new arguments to the space reserved
        // in the old argv[0] + argv[1]. If an argument won't fit then the algorithm
        // leaves it fully out and terminates.

        int spaceAvailable = 0;
        for (int i = 0; i < parentArgc; i++)
            spaceAvailable += strlen(parentArgv[i]) + 1;
        spaceAvailable--;

        if (spaceAvailable > 0)
        {
            memset(parentArgv[0], '\0', spaceAvailable);
            strncat(parentArgv[0], m_app.argv[0], spaceAvailable);
            spaceAvailable -= strlen(parentArgv[0]);

            for (int i = 1; i < m_app.argc; i++)
            {
                if (spaceAvailable > static_cast<int>(strlen(m_app.argv[i])) + 1)
                {
                    strcat(parentArgv[0], " ");
                    strcat(parentArgv[0], m_app.argv[i]);
                    spaceAvailable -= strlen(m_app.argv[i] + 1);
                }
                else
                {
                    break;
                }
            }
        }
    }

    // Set the process name using prctl
    if ( prctl(PR_SET_NAME, basename(m_app.argv[0])) == -1 )
        Logger::logError("on set new process name: %s ", strerror(errno));

    setenv("_", m_app.argv[0], true);
}

void Booster::launchProcess()
{
    // Load the application and find out the address of main()
    loadMain();

    for (int i = 0; i < 3; i++)
      if (m_app.ioDescriptors[i] > 0)
        dup2(m_app.ioDescriptors[i], i);

    Logger::logNotice("launching process: '%s' ", m_app.fileName.c_str());

    // Jump to main()
    exit(m_app.entry(m_app.argc, m_app.argv));
}

void Booster::loadMain()
{
    void *module;
    char *error_s;

    // Load the application as a library
    module = dlopen(m_app.fileName.c_str(), RTLD_LAZY | RTLD_GLOBAL);

    if (!module)
        Logger::logErrorAndDie(EXIT_FAILURE, "loading invoked application: '%s'\n", dlerror());

    // Find out the address for symbol "main".
    dlerror();
    m_app.entry = (entry_t)dlsym(module, "main");
    error_s = dlerror();

    if (error_s != NULL)
        Logger::logErrorAndDie(EXIT_FAILURE, "loading symbol 'main': '%s'\n", error_s);
}
