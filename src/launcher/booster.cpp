/*
 * booster.cpp
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

#include "booster.h"
#include "connection.h"
#include "logger.h"

#include <cstdlib>
#include <dlfcn.h>
#include <cerrno>
#include <unistd.h>
#include <sys/user.h>
#include <sys/prctl.h>
#include <sys/resource.h>

#ifdef HAVE_CREDS
    #include <sys/creds.h>
#endif

Booster::Booster() : m_argvArraySize(0)
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
    if (conn.acceptConn())
    {
        if (conn.receiveApplicationData(m_app))
        {
            return true;
        }

        // Close connection
        conn.closeConn();
    }

    return false;
}

void Booster::run()
{
    if (!m_app.fileName().empty())
    {
        Logger::logInfo("invoking '%s' ", m_app.fileName().c_str());
        launchProcess();
    }
    else
    {
        Logger::logError("nothing to invoke\n");
    }
}

void Booster::renameProcess(int parentArgc, char** parentArgv)
{
    if (m_argvArraySize == 0)
    {
        // rename process for the first time
        // calculate and store size of parentArgv array

        for (int i = 0; i < parentArgc; i++)
            m_argvArraySize += strlen(parentArgv[i]) + 1;

        m_argvArraySize--;
    }

    if (m_app.appName().empty())
    {
        // application name isn't known yet, let's give to the process
        // temporary booster name

        string newProcessName("booster-");
        newProcessName.append(1, boosterType());

        m_app.setAppName(newProcessName);
    }

    const char* newProcessName = m_app.appName().c_str();
    Logger::logNotice("set new name for process: %s", newProcessName);
    
    // This code copies all the new arguments to the space reserved
    // in the old argv array. If an argument won't fit then the algorithm
    // leaves it fully out and terminates.
    
    int spaceAvailable = m_argvArraySize;
    if (spaceAvailable > 0)
    {
        memset(parentArgv[0], '\0', spaceAvailable);
        strncat(parentArgv[0], newProcessName, spaceAvailable);
        
        spaceAvailable -= strlen(parentArgv[0]);
        
        for (int i = 1; i < m_app.argc(); i++)
        {
            if (spaceAvailable > static_cast<int>(strlen(m_app.argv()[i])) + 1)
            {
                strncat(parentArgv[0], " ", 1);
                strncat(parentArgv[0], m_app.argv()[i], spaceAvailable);
                spaceAvailable -= strlen(m_app.argv()[i] + 1);
            }
            else
            {
                break;
            }
        }
    }

    // Set the process name using prctl, killall and top use it
    if ( prctl(PR_SET_NAME, basename(newProcessName)) == -1 )
        Logger::logError("on set new process name: %s ", strerror(errno));

    setenv("_", newProcessName, true);
}

void Booster::launchProcess()
{
    // Possibly restore process priority
    errno = 0;
    const int cur_prio = getpriority(PRIO_PROCESS, 0);
    if (!errno && cur_prio < m_app.priority())
      setpriority(PRIO_PROCESS, 0, m_app.priority());

    // Load the application and find out the address of main()
    void* handle = loadMain();

    for (unsigned int i = 0; i < m_app.ioDescriptors().size(); i++)
      if (m_app.ioDescriptors()[i] > 0)
        dup2(m_app.ioDescriptors()[i], i);

    Logger::logNotice("launching process: '%s' ", m_app.fileName().c_str());

    // Jump to main()
    const int retVal = m_app.entry()(m_app.argc(), const_cast<char **>(m_app.argv()));
    m_app.deleteArgv();
    dlclose(handle);
    exit(retVal);
}

void* Booster::loadMain()
{
#ifdef HAVE_CREDS
    // Set application's platform security credentials
    creds_confine(m_app.fileName().c_str());
#endif

    // Load the application as a library
    void * module = dlopen(m_app.fileName().c_str(), RTLD_LAZY | RTLD_GLOBAL);

    if (!module)
        Logger::logErrorAndDie(EXIT_FAILURE, "loading invoked application: '%s'\n", dlerror());

    // Find out the address for symbol "main".
    dlerror();
    m_app.setEntry(reinterpret_cast<entry_t>(dlsym(module, "main")));

    const char * error_s = dlerror();
    if (error_s != NULL)
        Logger::logErrorAndDie(EXIT_FAILURE, "loading symbol 'main': '%s'\n", error_s);

    return module;
}
