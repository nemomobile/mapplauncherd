/*
 * connection.h
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

#ifndef CONNECTION_H
#define CONNECTION_H

#include <stdint.h>
#include <string>
#include <map>

using std::string;
using std::map;

typedef map<string, int> poolType;

//! Wrapper class for the connection between invoker and launcher
class Connection
{
public:

    //! Constructor
    Connection(const string socketId);

    //! Destructor
    virtual ~Connection();

    /*! Accept connection
     * \return True on success
     */
    bool acceptConn();

    //! Close connection
    void closeConn();

    //! Return the magic number
    int getMagic();

    //! Return the application name
    string getAppName();

    /*! Receive actions
     * \return True on success
     */
    bool receiveActions();

    //! Return the file name
    string  fileName() { return m_fileName; }

    //! Return argument count
    int argc() { return static_cast<int>(m_argc);}

    //! Return argument list
    char** argv() { return m_argv; }

    //! Initialize socket for provided socket id
    static void initSocket(const string socketId);

    //! Return initialized socket identified by socket id
    static int   getSocket(const string socketId);

private:

    bool  getExec();
    bool  getArgs();
    bool  getEnv();

    static const unsigned int  INVOKER_MSG_MASK                =    0xffff0000;
    static const unsigned int  INVOKER_MSG_MAGIC               =    0xb0070000;
    static const unsigned int  INVOKER_MSG_MAGIC_VERSION_MASK  =    0x0000ff00;
    static const unsigned int  INVOKER_MSG_MAGIC_VERSION       =    0x00000300;
    static const unsigned int  INVOKER_MSG_MAGIC_OPTION_MASK   =    0x000000ff;
    static const unsigned int  INVOKER_MSG_MAGIC_OPTION_WAIT   =    0x00000001;
    static const unsigned int  INVOKER_MSG_NAME                =    0x5a5e0000;
    static const unsigned int  INVOKER_MSG_EXEC                =    0xe8ec0000;
    static const unsigned int  INVOKER_MSG_ARGS                =    0xa4650000;
    static const unsigned int  INVOKER_MSG_ENV                 =    0xe5710000;
    static const unsigned int  INVOKER_MSG_PRIO                =    0xa1ce0000;
    static const unsigned int  INVOKER_MSG_IO                  =    0x10fd0000;
    static const unsigned int  INVOKER_MSG_END                 =    0xdead0000;
    static const unsigned int  INVOKER_MSG_PID                 =    0x1d1d0000;
    static const unsigned int  INVOKER_MSG_EXIT                =    0xe4170000;
    static const unsigned int  INVOKER_MSG_ACK                 =    0x600d0000;

    bool  sendMsg(uint32_t msg);
    bool  recvMsg(uint32_t *msg);
    bool  sendStr(char *str);
    char* recvStr();

    static poolType socketPool;

    //! socket
    int       m_fd;
    int       m_curSocket;
    string    m_fileName;
    uint32_t  m_argc;
    char    **m_argv;
};

#endif //CONNECTION_H
