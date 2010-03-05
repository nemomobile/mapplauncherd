/*
 * connection.h
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

#ifndef CONNECTION_H
#define CONNECTION_H

#include <string>
#include <map>

using std::string;
using std::map;

typedef map<string, int> poolType;

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

    /*! \brief Return the magic number.
     * \return The magic number received from the invoker.
     */
    int getMagic();

    /*! \brief Return the application name.
     * \return Name string
     */
    string getAppName();

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
     * \return Pointer to the three-element list of fd's.
     */
    int* ioDescriptors();

    /*! \brief Initialize a file socket.
     * \param socketId Path to the socket file
     */
    static void initSocket(const string socketId);

    /*! \brief Return initialized socket.
     * \param socketId Path to the socket file
     */
    static int getSocket(const string socketId);

private:

    //! Disable copy-constructor
    Connection(const Connection & r);

    //! Disable assignment operator
    Connection & operator= (const Connection & r);

    bool getExec();
    bool getArgs();
    bool getEnv();
    bool getIo();

    static const unsigned int INVOKER_MSG_MASK               = 0xffff0000;
    static const unsigned int INVOKER_MSG_MAGIC              = 0xb0070000;
    static const unsigned int INVOKER_MSG_MAGIC_VERSION_MASK = 0x0000ff00;
    static const unsigned int INVOKER_MSG_MAGIC_VERSION      = 0x00000300;
    static const unsigned int INVOKER_MSG_MAGIC_OPTION_MASK  = 0x000000ff;
    static const unsigned int INVOKER_MSG_MAGIC_OPTION_WAIT  = 0x00000001;
    static const unsigned int INVOKER_MSG_NAME               = 0x5a5e0000;
    static const unsigned int INVOKER_MSG_EXEC               = 0xe8ec0000;
    static const unsigned int INVOKER_MSG_ARGS               = 0xa4650000;
    static const unsigned int INVOKER_MSG_ENV                = 0xe5710000;
    static const unsigned int INVOKER_MSG_PRIO               = 0xa1ce0000;
    static const unsigned int INVOKER_MSG_IO                 = 0x10fd0000;
    static const unsigned int INVOKER_MSG_END                = 0xdead0000;
    static const unsigned int INVOKER_MSG_PID                = 0x1d1d0000;
    static const unsigned int INVOKER_MSG_EXIT               = 0xe4170000;
    static const unsigned int INVOKER_MSG_ACK                = 0x600d0000;

    bool  sendMsg(uint32_t msg);
    bool  recvMsg(uint32_t *msg);
    bool  sendStr(char *str);
    char* recvStr();

    static poolType socketPool;

    //! socket
    int      m_fd;
    int      m_curSocket;
    string   m_fileName;
    uint32_t m_argc;
    char   **m_argv;
    int      m_io[3];

#ifdef UNIT_TEST
    friend class Ut_Connection;
#endif
};

#endif //CONNECTION_H
