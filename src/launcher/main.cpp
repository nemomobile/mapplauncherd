/*
 * main.cpp
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

#include "daemon.h"
#include "logger.h"

#include <cstdlib>
#include <signal.h>
#include <fcntl.h>
#include <sys/file.h>

//! Signal handler to reap zombies
void reapZombies(int)
{
    if (Daemon::instance())
        Daemon::instance()->reapZombies();
}

//! Signal handler to kill booster
void exitBooster(int)
{
    Logger::logErrorAndDie(EXIT_FAILURE, "due to parent process applauncherd died, booster exit too");
}

//! Lock file to prevent launch of second instance
bool getLock(void)
{
    struct flock fl;
    int fd;
    
    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 1;
    
    if((fd = open("/tmp/applauncherd.lock", O_WRONLY | O_CREAT, 0666)) == -1)
        return false;
    
    if(fcntl(fd, F_SETLK, &fl) == -1)
        return false;
   
    return true;
}


//! Main function
int main(int argc, char * argv[])
{
    // Open the log
    Logger::openLog(PROG_NAME);
    Logger::logNotice("%s starting..", PROG_NAME);

    // Check that an instance of launcher is not already running
    if(!getLock())
    {
        Logger::logErrorAndDie(EXIT_FAILURE, "try to launch second instance");
    }

    // Install signal handlers
    signal(SIGCHLD, reapZombies);
    signal(SIGHUP,  exitBooster);

    // Create main daemon instance
    Daemon myDaemon(argc, argv);

    // Run the main loop
    myDaemon.run();

    // Close the log
    Logger::closeLog();

    return EXIT_SUCCESS;
}

