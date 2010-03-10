/*
 * connection.cpp
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

#include "connection.h"
#include "logger.h"

#include <sys/socket.h>
#include <cstring>
#include <cstdlib>
#include <errno.h>

const unsigned int  Connection::INVOKER_MSG_MASK;
const unsigned int  Connection::INVOKER_MSG_MAGIC;
const unsigned int  Connection::INVOKER_MSG_MAGIC_VERSION_MASK;
const unsigned int  Connection::INVOKER_MSG_MAGIC_VERSION;
const unsigned int  Connection::INVOKER_MSG_MAGIC_OPTION_MASK;
const unsigned int  Connection::INVOKER_MSG_MAGIC_OPTION_WAIT;
const unsigned int  Connection::INVOKER_MSG_NAME;
const unsigned int  Connection::INVOKER_MSG_EXEC;
const unsigned int  Connection::INVOKER_MSG_ARGS;
const unsigned int  Connection::INVOKER_MSG_ENV;
const unsigned int  Connection::INVOKER_MSG_PRIO;
const unsigned int  Connection::INVOKER_MSG_IO;
const unsigned int  Connection::INVOKER_MSG_END;
const unsigned int  Connection::INVOKER_MSG_PID;
const unsigned int  Connection::INVOKER_MSG_EXIT;
const unsigned int  Connection::INVOKER_MSG_ACK;

poolType Connection::socketPool;

Connection::Connection(const string socketId) :
    m_fd(-1),
    m_curSocket(getSocket(socketId))
{
    m_io[0] = -1;
    m_io[1] = -1;
    m_io[2] = -1;

    if (m_curSocket == -1)
    {
        Logger::logErrorAndDie(EXIT_FAILURE, "socket isn't initialized\n");
    }
}

Connection::~Connection()
{
    closeConn();
}

string Connection::fileName() const
{
    return m_fileName;
}

int Connection::argc() const
{
    return static_cast<int>(m_argc);
}

char** Connection::argv() const
{
    return m_argv;
}

int* Connection::ioDescriptors()
{
    return m_io;
}

int Connection::prio()
{
    return m_prio;
}

int Connection::getSocket(const string socketId)
{
    poolType::iterator it;
    it = socketPool.find(socketId);
    return it == socketPool.end() ? -1 : it->second;
}

void Connection::initSocket(const string socketId)
{
    poolType::iterator it;
    it = socketPool.find(socketId);
    if (it == socketPool.end())
    {
        Logger::logInfo("%s: init socket '%s'", __FUNCTION__, socketId.c_str());

        int sockfd = socket(PF_UNIX, SOCK_STREAM, 0);
        if (sockfd < 0)
            Logger::logErrorAndDie(EXIT_FAILURE, "opening invoker socket\n");

        unlink(socketId.c_str());

        struct sockaddr sun;
        sun.sa_family = AF_UNIX;
        strcpy(sun.sa_data, socketId.c_str());

        if (bind(sockfd, &sun, sizeof(sun)) < 0)
            Logger::logErrorAndDie(EXIT_FAILURE, "binding to invoker socket\n");

        if (listen(sockfd, 10) < 0)
            Logger::logErrorAndDie(EXIT_FAILURE, "listening to invoker socket\n");

        socketPool[socketId] = sockfd;
    }
}

bool Connection::acceptConn()
{
    m_fd = accept(m_curSocket, NULL, NULL);

    // Minimal error handling.
    if (m_fd < 0)
    {
        if (errno != EINTR)
        {
            Logger::logError("accepting connections (%s)\n", strerror(errno));
            return false;
        }
    }
    return true;
}

void Connection::closeConn()
{
    if (m_fd != -1)
    {
        close(m_fd);
        m_fd = -1;
    }
}

bool Connection::sendMsg(uint32_t msg)
{
    Logger::logInfo("%s: %08x", __FUNCTION__, msg);
    return write(m_fd, &msg, sizeof(msg)) != -1;
}

bool Connection::recvMsg(uint32_t *msg)
{
    ssize_t ret = read(m_fd, msg, sizeof(*msg));
    Logger::logInfo("%s: %08x", __FUNCTION__, *msg);
    return ret != -1;
}

bool Connection::sendStr(char *str)
{
    // Send size.
    uint32_t size = strlen(str) + 1;
    sendMsg(size);

    Logger::logInfo("%s: '%s'", __FUNCTION__, str);

    // Send the string.
    return write(m_fd, str, size) != -1;
}

char* Connection::recvStr()
{
    // Get the size.
    uint32_t size;
    recvMsg(&size);

    char *str = (char*)malloc(size);
    if (!str)
    {
        Logger::logError("mallocing in %s", __FUNCTION__);
        return NULL;
    }

    // Get the string.
    uint32_t ret = read(m_fd, str, size);
    if (ret < size)
    {
        Logger::logError("getting string, got %u of %u bytes", ret, size);
        free(str);
        return NULL;
    }
    str[size - 1] = '\0';
    Logger::logInfo("%s: '%s'", __FUNCTION__, str);
    return str;
}


int Connection::getMagic()
{
    uint32_t magic;

    // Receive the magic.
    recvMsg(&magic);

    if ((magic & INVOKER_MSG_MASK) == INVOKER_MSG_MAGIC)
    {
        if ((magic & INVOKER_MSG_MAGIC_VERSION_MASK) == INVOKER_MSG_MAGIC_VERSION)
            sendMsg(INVOKER_MSG_ACK);
        else
        {
            Logger::logError("receiving bad magic version (%08x)\n", magic);
            return -1;
        }
    }
    return magic & INVOKER_MSG_MAGIC_OPTION_MASK;
}

string Connection::getAppName()
{
    uint32_t msg;

    // Get the action.
    recvMsg(&msg);
    if (msg != INVOKER_MSG_NAME)
    {
        Logger::logError("receiving invalid action (%08x)", msg);
        return string();
    }

    char* name = recvStr();
    if (!name)
    {
        Logger::logError("receiving application name");
        return string();
    }
    sendMsg(INVOKER_MSG_ACK);

    string appName(name);
    free(name);

    return appName;
}

bool Connection::getExec()
{
    char* filename = recvStr();
    if (!filename)
        return false;

    sendMsg(INVOKER_MSG_ACK);

    m_fileName = filename;
    free(filename);

    return true;
}

bool Connection::getPrio()
{
    recvMsg(&m_prio);
    sendMsg(INVOKER_MSG_ACK);

    return true;
}

bool Connection::getArgs()
{
    uint i;
    size_t size;

    // Get argc
    recvMsg(&m_argc);
    size = m_argc * sizeof(char *);
    if (size < m_argc)
    {
        Logger::logError("on buggy or malicious invoker code, heap overflow avoided\n");
        return false;
    }

    m_argv = static_cast<char**>(malloc(size));
    if (!m_argv)
    {
        Logger::logError("mallocing\n");
        return false;
    }

    // Get argv
    for (i = 0; i < m_argc; i++)
    {
        m_argv[i] = recvStr();
        if (!m_argv[i])
        {
            Logger::logError("receiving argv[%i]\n", i);
            return false;
        }
    }
    sendMsg(INVOKER_MSG_ACK);

    return true;
}

bool Connection::getEnv()
{
    uint i;
    uint32_t n_vars;

    // Get number of environment variables.
    recvMsg(&n_vars);

    // Get environment variables
    for (i = 0; i < n_vars; i++)
    {
        char *var;

        var = recvStr();
        if (var == NULL)
        {
            Logger::logError("receiving environ[%i]", i);
            return false;
        }

        //Logger::logNotice("setting environment variable '%s'", var);

        // In case of error, just warn and try to continue, as the other side is
        // going to keep sending the reset of the message.
        // String pointed to by var shall become part of the environment, so altering
        // the string shall change the environment, don't free it
        if (putenv(var) != 0)
            Logger::logWarning("allocating environment variable");
    }

    return true;
}

bool Connection::getIo()
{
    struct msghdr msg;
    struct cmsghdr *cmsg;
    char buf[CMSG_SPACE(sizeof(m_io))];
    struct iovec iov;
    int dummy;

    iov.iov_base = &dummy;
    iov.iov_len = 1;

    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = buf;
    msg.msg_controllen = sizeof(buf);

    cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_len = CMSG_LEN(sizeof(m_io));
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;

    memcpy(CMSG_DATA(cmsg), m_io, sizeof(m_io));

    if (recvmsg(m_fd, &msg, 0) < 0)
    {
        Logger::logWarning("recvmsg failed in invoked_get_io: %s", strerror(errno));
        return false;
    }

    if (msg.msg_flags)
    {
        Logger::logWarning("unexpected msg flags in invoked_get_io");
        return false;
    }

    cmsg = CMSG_FIRSTHDR(&msg);
    if (cmsg == NULL || cmsg->cmsg_len != CMSG_LEN(sizeof(m_io)) ||
        cmsg->cmsg_level != SOL_SOCKET || cmsg->cmsg_type != SCM_RIGHTS)
    {
        Logger::logWarning("invalid cmsg in invoked_get_io");
        return false;
    }

    memcpy(m_io, CMSG_DATA(cmsg), sizeof(m_io));

    return true;
}


bool Connection::receiveActions()
{
    Logger::logInfo("enter: %s", __FUNCTION__);

    while (1)
    {
        uint32_t action;

        // Get the action.
        recvMsg(&action);

        switch (action)
        {
        case INVOKER_MSG_EXEC:
            getExec();
            break;
        case INVOKER_MSG_ARGS:
            getArgs();
            break;
        case INVOKER_MSG_ENV:
            getEnv();
            break;
        case INVOKER_MSG_PRIO:
            getPrio();
            break;
        case INVOKER_MSG_IO:
            getIo();
            break;
        case INVOKER_MSG_END:
            sendMsg(INVOKER_MSG_ACK);
            return true;
        default:
            Logger::logError("receiving invalid action (%08x)\n", action);
            return false;
        }
    }
}


