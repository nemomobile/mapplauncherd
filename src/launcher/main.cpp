/*
 * main.cpp
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

    if(!getLock())
    {
        Logger::logErrorAndDie(EXIT_FAILURE, "try to launch second instance");
    }

    // Install signal handler
    signal(SIGCHLD, reapZombies);

    Daemon myDaemon(argc, argv);

    // Run the main loop
    myDaemon.run();

    // Close the log
    Logger::closeLog();

    return EXIT_SUCCESS;
}

