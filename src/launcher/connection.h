/*
 * connection.h
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

#ifndef CONNECTION_H
#define CONNECTION_H

#include <stdint.h>

#include <string>

using std::string;

#include <map>

using std::map;

#include <vector>

using std::vector;

typedef map<string, int> PoolType;

/*!
 * \class Connection.
 * \brief Wrapper class for the connection between invoker and launcher.
 *
 * This class wraps up the UNIX file socket connection between the invoker
 * and the launcher daemon.
 */
class Connection
{
public:

    /*! \brief Constructor.
     *  \param socketId Path to the UNIX file socket to be used.
     */
    explicit Connection(const string socketId);

    //! \brief Destructor
    virtual ~Connection();

    /*! \brief Accept connection.
     * Accept a socket connection from the invoker.
     * \return true on success.
     */
    bool acceptConn();

    //! \brief Close the socket connection
    void closeConn();

    /*! \brief Receive and return the magic number.
     * \return The magic number received from the invoker.
     */
    int receiveMagic();

    /*! \brief Receive and return the application name.
     * \return Name string
     */
    string receiveAppName();

    /*! \brief Receive actions.
     * This method executes the actual data-receiving loop and terminates
     * after INVOKER_MSG_END is received.
     * \return True on success
     */
    bool receiveActions();

    /*! \brief Return the file name.
     * Return the executable file name received from the invoker.
     * \return Name string
     */
    string fileName() const;

    /*! \brief Return the argument count.
     * Return the CLI argument count received from the invoker.
     * \return Argument count
     */
    int argc() const;

    /*! \brief Return the argument list.
     * Return the CLI argument list received from the invoker.
     * \return Pointer to the argument list
     */
    char** argv() const;

    /*! \brief Return I/O file descriptors.
     * Return the I/O file descriptors received from the invoker.
     * \return fd vector.
     */
    vector<int> ioDescriptors() const;

    /*! \brief Return process priority.
     * Return the process priority received from the invoker.
     * \return application process priority.
     */
    int priority() const;

    /*! \brief Initialize a file socket.
     * \param socketId Path to the socket file
     */
    static void initSocket(const string socketId);

    /*! \brief Return initialized socket.
     * \param socketId Path to the socket file
     */
    static int findSocket(const string socketId);

private:

    //! Disable copy-constructor
    Connection(const Connection & r);

    //! Disable assignment operator
    Connection & operator= (const Connection & r);

    bool  receiveExec();
    bool  receiveArgs();
    bool  receiveEnv();
    bool  receiveIO();
    bool  receivePriority();
    bool  sendMsg(uint32_t msg);
    bool  recvMsg(uint32_t *msg);
    bool  sendStr(char *str);
    char* recvStr();

    static PoolType socketPool;

    //! socket
    int         m_fd;
    int         m_curSocket;
    string      m_fileName;
    uint32_t    m_argc;
    char **     m_argv;
    int         m_io[3];
    uint32_t    m_prio;

#ifdef UNIT_TEST
    friend class Ut_Connection;
#endif
};

#endif //CONNECTION_H
