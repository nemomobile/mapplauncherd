/***************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of applauncherd
**
** If you have questions regarding the use of this file, please contact
** Nokia at directui@nokia.com.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#include "booster.h"
#include "daemon.h"
#include "connection.h"
#include "singleinstance.h"
#include "socketmanager.h"
#include "logger.h"

#include <cstdlib>
#include <dlfcn.h>
#include <cerrno>
#include <unistd.h>
#include <sys/user.h>
#include <sys/prctl.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <syslog.h>

#ifdef USE_X11
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#endif //USE_X11

#include <sys/types.h>
#include <sys/socket.h>
#include <grp.h>

#include "coverage.h"

static const int FALLBACK_GID = 126;

static gid_t getGroupId(const char *name, gid_t fallback)
{
    struct group group, *grpptr;
    size_t size = sysconf(_SC_GETGR_R_SIZE_MAX);
    char buf[size];

    if (getgrnam_r(name, &group, buf, size, &grpptr) == 0 && grpptr != NULL)
        return group.gr_gid;
    else
        return fallback;
}

Booster::Booster() :
    m_appData(new AppData),
    m_connection(NULL),
    m_oldPriority(0),
    m_oldPriorityOk(false),
    m_spaceAvailable(0),
    m_bootMode(false)
{
    m_boosted_gid = getGroupId("boosted", FALLBACK_GID);
}

Booster::~Booster()
{
    delete m_connection;
    m_connection = NULL;

    delete m_appData;
    m_appData = NULL;
}

void Booster::initialize(int initialArgc, char ** initialArgv, int newBoosterLauncherSocket,
                         int socketFd, SingleInstance * singleInstance,
                         bool newBootMode)
{
    m_bootMode = newBootMode;

    setBoosterLauncherSocket(newBoosterLauncherSocket);

    // Drop priority (nice = 10)
    pushPriority(10);

    // Preload stuff
    if (!m_bootMode)
        preload();

    // Rename process to temporary booster process name
    std::string temporaryProcessName = "booster [";
    temporaryProcessName += boosterType();
    temporaryProcessName += "]";
    const char * tempArgv[] = {temporaryProcessName.c_str()};
    renameProcess(initialArgc, initialArgv, 1, tempArgv);

    // Restore priority
    popPriority();

    while (true)
    {
        // Wait and read commands from the invoker
        Logger::logDebug("Booster: Wait for message from invoker");
        if (!receiveDataFromInvoker(socketFd))
            throw std::runtime_error("Booster: Couldn't read command\n");

        // Run process as single instance if requested
        if (m_appData->singleInstance())
        {
            // Check if instance is already running
            SingleInstancePluginEntry * pluginEntry = singleInstance->pluginEntry();
            if (pluginEntry)
            {
                if (!pluginEntry->lockFunc(m_appData->appName().c_str()))
                {
                    // Try to activate the window of the existing instance
                    if (!pluginEntry->activateExistingInstanceFunc(m_appData->appName().c_str()))
                    {
                        Logger::logWarning("Booster: Can't activate existing instance of the application!");
                        m_connection->sendExitValue(EXIT_FAILURE);
                    }
                    else
                    {
                        m_connection->sendExitValue(EXIT_SUCCESS);
                    }
                    m_connection->close();

                    // invoker requested to start an application that is already running
                    // booster is not needed this time, let's wait for the next connection from invoker
                    continue;
                }

                // Close the single-instance plugin
                singleInstance->closePlugin();
            }
            else
            {
                Logger::logWarning("Booster: Single-instance launch wanted, but single-instance plugin not loaded!");
            }
        }

        //this instance of booster will be used to start application, exit from the loop
        break;
    }

    // Send parent process a message that it can create a new booster,
    // send pid of invoker, booster respawn value and invoker socket connection.
    sendDataToParent();

    // Give the process the real application name now that it
    // has been read from invoker in receiveDataFromInvoker().
    renameProcess(initialArgc, initialArgv, m_appData->argc(), m_appData->argv());

    close(boosterLauncherSocket());

    // close invoker socket connection
    m_connection->close();

    // Don't care about fate of parent applauncherd process any more
    prctl(PR_SET_PDEATHSIG, 0);
}

bool Booster::bootMode() const
{
    return m_bootMode;
}

void Booster::sendDataToParent()
{
    // Number of data items to be sent to
    // the parent (launcher) process
    const unsigned int NUM_DATA_ITEMS = 2;

    struct iovec    iov[NUM_DATA_ITEMS];
    struct msghdr   msg;
    struct cmsghdr *cmsg;
    char buf[CMSG_SPACE(sizeof(int))];

    // Signal the parent process that it can create a new
    // waiting booster process and close write end
    // Send to the parent process pid of invoker for tracking
    pid_t pid = invokersPid();
    iov[0].iov_base = &pid;
    iov[0].iov_len  = sizeof(pid_t);

    // Send to the parent process booster respawn delay value
    int delay = m_appData->delay();
    iov[1].iov_base = &delay;
    iov[1].iov_len  = sizeof(int);

    msg.msg_iov     = iov;
    msg.msg_iovlen  = NUM_DATA_ITEMS;
    msg.msg_name    = NULL;
    msg.msg_namelen = 0;

    // Set special control fields if exit status of the launched
    // application is needed. In this case we want to give the fd of the
    // invoker <-> booster socket connection to the parent process (launcher)
    // so that it can send the exit status back to invoker. It'd be impossible
    // from the booster process if exec() was used.
    if (m_connection->isReportAppExitStatusNeeded())
    {
        // Send socket file descriptor to parent
        int fd = m_connection->getFd();
        msg.msg_control    = buf;
        msg.msg_controllen = sizeof(buf);
        cmsg               = CMSG_FIRSTHDR(&msg);
        cmsg->cmsg_level   = SOL_SOCKET;
        cmsg->cmsg_type    = SCM_RIGHTS;
        cmsg->cmsg_len     = CMSG_LEN(sizeof(int));
        memcpy(CMSG_DATA(cmsg), &fd, sizeof(int));
    }
    else
    {
        msg.msg_control    = NULL;
        msg.msg_controllen = 0;
    }

    if (sendmsg(boosterLauncherSocket(), &msg, 0) < 0)
    {
        Logger::logError("Booster: Couldn't send data to launcher process\n");
    }
}

bool Booster::receiveDataFromInvoker(int socketFd)
{
    // delete previous connection instance because booster can
    // connect with several invokers due to single-instance feature
    if (m_connection != NULL)
    {
        delete  m_connection;
        m_connection = NULL;
    }

    // Setup the conversation channel with the invoker.
    m_connection = new Connection(socketFd);

    // Accept a new invocation.
    if (m_connection->accept(m_appData))
    {
        // Receive application data from the invoker
        if(!m_connection->receiveApplicationData(m_appData))
        {
            m_connection->close();
            return false;
        }

        // Close the connection if exit status doesn't need
        // to be sent back to invoker
        if (!m_connection->isReportAppExitStatusNeeded())
        {
            m_connection->close();
        }

        return true;
    }

    return false;
}

int Booster::run(SocketManager * socketManager)
{
    if (!m_appData->fileName().empty())
    {
        // We can close sockets here because
        // socket FD is passed to daemon already
        if (socketManager)
        {
            socketManager->closeAllSockets();
        }

        // Execute the binary
        Logger::logDebug("Booster: invoking '%s' ", m_appData->fileName().c_str());
        return launchProcess();
    }
    else
    {
        Logger::logError("Booster: nothing to invoke\n");
        return EXIT_FAILURE;
    }
}

void Booster::renameProcess(int parentArgc, char** parentArgv,
                            int sourceArgc, const char** sourceArgv)
{
    if (sourceArgc > 0 && parentArgc > 0)
    {
        // Calculate original space reserved for arguments, if not
        // already calculated
        if (!m_spaceAvailable)
            for (int i = 0; i < parentArgc; i++)
                m_spaceAvailable += strlen(parentArgv[i]) + 1;

        if (m_spaceAvailable)
        {
            // Build a contiguous, NULL-separated block for the new arguments.
            // This is how Linux puts them.
            std::string newArgv;
            for (int i = 0; i < sourceArgc; i++)
            {
                newArgv += sourceArgv[i];
                newArgv += '\0';
            }

            const int spaceNeeded = std::min(m_spaceAvailable,
                                             static_cast<int>(newArgv.size()));

            // Reset the old space
            memset(parentArgv[0], '\0', m_spaceAvailable);

            if (spaceNeeded > 0)
            {
                // Copy the argument data. Note: if they don't fit, then
                // they are just cut off.
                memcpy(parentArgv[0], newArgv.c_str(), spaceNeeded);

                // Ensure NULL at the end
                parentArgv[0][spaceNeeded - 1] = '\0';
            }
        }

        // Set the process name using prctl, 'killall' and 'top' use it
        if ( prctl(PR_SET_NAME, basename(sourceArgv[0])) == -1 )
            Logger::logError("Booster: on set new process name: %s ", strerror(errno));

        setenv("_", sourceArgv[0], true);
    }
}

void Booster::requestSplash(const int pid, const std::string &wmclass, 
                            const std::string &portraitSplash, const std::string &landscapeSplash,
                            const std::string &pixmapId)
{
#ifdef USE_X11

    std::stringstream st;
    st << pid;
    std::string pidStr = st.str();

    // set error handler for all Xlib calls
    XErrorHandler oldHandler = XSetErrorHandler(Booster::handleXError);

    Display * dpy = XOpenDisplay(NULL);
    if (dpy) 
    {
        const char *compositorWindowIdProperty =  "_NET_SUPPORTING_WM_CHECK";
        Atom compositorWindowIdAtom = XInternAtom(dpy, compositorWindowIdProperty, False);
        Atom type;
        int format;
        unsigned long nItems;
        unsigned long bytesAfter;
        unsigned char *prop = 0;

        // Get the compositor window id
        Window rootWin = XDefaultRootWindow(dpy);
        int retval = XGetWindowProperty(dpy, rootWin, compositorWindowIdAtom,
                                        0, 0x7fffffff, False, XA_WINDOW,
                                        &type, &format, &nItems, &bytesAfter, &prop);

        // Check not only return value but also make sure
        // that property was found so pointer isn't NULL
        if ((retval == Success) && (prop != NULL))
        {
            // Package up the data and set the property
            int len = pidStr.length() + 1
                + wmclass.length() + 1
                + portraitSplash.length() + 1
                + landscapeSplash.length() + 1
                + pixmapId.length() + 1;

            char *data = new char[len];
            char *d = data;

            strcpy(d, pidStr.c_str());
            d = d + pidStr.length() + 1;
            strcpy(d, wmclass.c_str());
            d = d + wmclass.length() + 1;
            strcpy(d, portraitSplash.c_str());
            d = d + portraitSplash.length() + 1;
            strcpy(d, landscapeSplash.c_str());
            d = d + landscapeSplash.length() + 1;
            strcpy(d, pixmapId.c_str());

            Window compositorWindow = *reinterpret_cast<Window *>(prop);
            const char* splashProperty =  "_MEEGO_SPLASH_SCREEN";
            Atom splashPropertyAtom = XInternAtom(dpy, splashProperty, False);
 
            XChangeProperty(dpy, compositorWindow, splashPropertyAtom, XA_STRING,
                            8, PropModeReplace, (unsigned char*) data, len);

            // Without flushing, the change seems to loiter in X's queue
            XFlush(dpy);
            delete[] data;
            XFree(prop);
        }
        // close connection to X server
        XCloseDisplay(dpy);
        XSetErrorHandler(oldHandler);
    }
#else //USE_X11
    // prevent compilation warnings
    (void) pid;                                                                                     
    (void) wmclass;
    (void) portraitSplash;
    (void) landscapeSplash;
    (void) pixmapId;
#endif //USE_X11
}

#ifdef USE_X11
int Booster::handleXError(Display *, XErrorEvent *)
{
    return 0;
}
#endif //USE_X11

void Booster::setEnvironmentBeforeLaunch()
{
    // Possibly restore process priority
    errno = 0;
    const int cur_prio = getpriority(PRIO_PROCESS, 0);
    if (!errno && cur_prio < m_appData->priority())
        setpriority(PRIO_PROCESS, 0, m_appData->priority());

    // Set user ID and group ID of calling process if differing
    // from the ones we got from invoker

    if (getuid() != m_appData->userId())
        setuid(m_appData->userId());

    if (getgid() != m_appData->groupId())
        setgid(m_appData->groupId());

    // Flip the effective group ID forth and back to a dedicated group
    // id to generate an event for policy (re-)classification.
    gid_t orig = getegid();
      
    setegid(m_boosted_gid);
    setegid(orig);

    // Reset out-of-memory killer adjustment
    if (!m_appData->disableOutOfMemAdj())
        resetOomAdj();

    // Request splash screen from mcompositor if needed
    if (m_appData->splashFileName().length() > 0 || m_appData->landscapeSplashFileName().length() > 0)
    {
        // Construct WM_CLASS from the app absolute path
        std::string wmclass(m_appData->appName());
        size_t pos = wmclass.rfind('/');
        wmclass.erase(0, pos + 1);
        wmclass[0] = toupper(wmclass[0]);

        // Communicate splash data to compositor
        requestSplash(getpid(), wmclass,
                      m_appData->splashFileName(),
                      m_appData->landscapeSplashFileName(),

                      // Compositor can also show an X pixmap as splash,
                      // but this feature is currently not used.
                      std::string(""));
    }

    // Make sure that boosted application can dump core. This must be
    // done after set[ug]id().
    prctl(PR_SET_DUMPABLE, 1);

    // Duplicate I/O descriptors
    for (unsigned int i = 0; i < m_appData->ioDescriptors().size(); i++)
    {
        if (m_appData->ioDescriptors()[i] > 0)
        {
            dup2(m_appData->ioDescriptors()[i], i);
            close(m_appData->ioDescriptors()[i]);
        }
    }

    // Set PWD
    const char * pwd = getenv("PWD");
    if (pwd) chdir(pwd);

    Logger::logDebug("Booster: launching process: '%s' ", m_appData->fileName().c_str());
}

int Booster::launchProcess()
{
    setEnvironmentBeforeLaunch();

    // Load the application and find out the address of main()
    loadMain();

    // make booster specific initializations unless booster is in boot mode
    if (!m_bootMode)
        preinit();

#ifdef WITH_COVERAGE
    __gcov_flush();
#endif

    // Close syslog
    closelog();

    // Jump to main()
    const int retVal = m_appData->entry()(m_appData->argc(), const_cast<char **>(m_appData->argv()));

#ifdef WITH_COVERAGE
    __gcov_flush();
#endif

    return retVal;
}

void* Booster::loadMain()
{
    // Setup flags for dlopen

    int dlopenFlags = RTLD_LAZY;

    if (m_appData->dlopenGlobal())
        dlopenFlags |= RTLD_GLOBAL;
    else
        dlopenFlags |= RTLD_LOCAL;

#if (PLATFORM_ID == Linux)
    if (m_appData->dlopenDeep())
        dlopenFlags |= RTLD_DEEPBIND;
#endif

    // Load the application as a library
    void * module = dlopen(m_appData->fileName().c_str(), dlopenFlags);

    if (!module)
        throw std::runtime_error(std::string("Booster: Loading invoked application failed: '") +
                                 dlerror() + "'\n");

    // Find out the address for symbol "main". dlerror() is first used to clear any old error conditions,
    // then dlsym() is called, and then dlerror() is checked again. This procedure is documented
    // in dlsym()'s man page.

    dlerror();
    m_appData->setEntry(reinterpret_cast<entry_t>(dlsym(module, "main")));

    const char * error_s = dlerror();
    if (error_s != NULL)
        throw std::runtime_error(std::string("Booster: Loading symbol 'main' failed: '") +
                                 error_s + "'\n");

    return module;
}

bool Booster::pushPriority(int nice)
{
    errno = 0;
    m_oldPriorityOk = true;
    m_oldPriority   = getpriority(PRIO_PROCESS, getpid());

    if (errno)
    {
        m_oldPriorityOk = false;
    }
    else
    {
        return setpriority(PRIO_PROCESS, getpid(), nice) != -1;
    }

    return false;
}

bool Booster::popPriority()
{
    if (m_oldPriorityOk)
    {
        return setpriority(PRIO_PROCESS, getpid(), m_oldPriority) != -1;
    }

    return false;
}

pid_t Booster::invokersPid()
{
    if (m_connection->isReportAppExitStatusNeeded())
    {
        return m_connection->peerPid();
    }
    else
    {
        return 0;
    }
}

void Booster::setBoosterLauncherSocket(int newBoosterLauncherSocket)
{
    m_boosterLauncherSocket = newBoosterLauncherSocket;
}

int Booster::boosterLauncherSocket() const
{
    return m_boosterLauncherSocket;
}

Connection* Booster::connection() const
{
    return m_connection;
}

void Booster::setConnection(Connection * newConnection)
{
    delete m_connection;
    m_connection = newConnection;
}

AppData* Booster::appData() const
{
    return m_appData;
}

void Booster::resetOomAdj()
{
    const char * PROC_OOM_ADJ_FILE = "/proc/self/oom_adj";
    int fd = open(PROC_OOM_ADJ_FILE, O_WRONLY);
    if (fd != -1)
    {
        if (write(fd, "0", sizeof(char)) == -1)
        {
            Logger::logError("Couldn't write to '%s': %s", PROC_OOM_ADJ_FILE,
                             strerror(errno));
        }

        close(fd);
    }
    else
    {
        Logger::logError("Couldn't open '%s' for write: %s", PROC_OOM_ADJ_FILE,
                         strerror(errno));
    }
}
