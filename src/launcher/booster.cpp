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
    }
    else
        return false;

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

    //modify cmdline of the process, add only application name
    int argvlen = 0;
    int proglen = strlen(m_app.argv[0]) + 1;

    for (int i = 0; i < parentArgc; i++)
      argvlen += strlen(parentArgv[i]) + 1;

    if (proglen > argvlen)
        proglen = argvlen;

    memmove(parentArgv[0], m_app.argv[0], proglen);

    parentArgv[1] = parentArgv[0] + proglen;

    int paramSpace = argvlen - proglen;

    if (paramSpace > 0)
    {
        memset(parentArgv[1], '\0', paramSpace);
        for (int i = 1; i < m_app.argc; i++)
        {
            int freeSpace = paramSpace - strlen(parentArgv[1]) - 1;
            if (freeSpace <= 1)
                break;

            strncat(parentArgv[1], " ", freeSpace--);
            strncat(parentArgv[1], m_app.argv[i], freeSpace);
        }
    }

    if ( prctl(PR_SET_NAME, basename(m_app.argv[0])) == -1 )
        Logger::logError("on set new process name: %s ", strerror(errno));

    setenv("_", m_app.argv[0], true);
}

void Booster::launchProcess()
{
    // Load the application and find out the address of main()
    loadMain();

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
