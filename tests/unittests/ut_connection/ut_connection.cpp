/*
 * ut_connection.cpp
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

#include "ut_connection.h"
#include "connection.h"

#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>


/* redefine some methods for Connection class */
class MyConnection : public Connection
{
public:
    int nextMsg;
    char* nextStr;

    MyConnection(const string socketId);
    bool acceptConn();

private:
    virtual bool  recvMsg(uint32_t *msg);
    virtual char* recvStr();

    virtual bool  sendMsg(uint32_t msg);
    virtual bool  sendStr(char *str);
};
bool  MyConnection::acceptConn() { return true; }

MyConnection::MyConnection(const string socketId) : 
    Connection(socketId) { }

bool MyConnection::recvMsg(uint32_t *msg)
{
    *msg = nextMsg;
    return true;
}
bool MyConnection::sendMsg(uint32_t)
{ return true; }

bool MyConnection::sendStr(char*)
{ return true; }


char* MyConnection::recvStr()
{
    return nextStr;
}


Ut_Connection::Ut_Connection()
{
}

Ut_Connection::~Ut_Connection()
{
}

void Ut_Connection::initTestCase()
{
}

void Ut_Connection::cleanupTestCase()
{
}

/*
 * Check that socket initialized for provided socket id
 */
void  Ut_Connection::testInitConnection()
{
    unsigned int prevNum = Connection::socketPool.size();
    Connection::initSocket("aaa");
    Connection::initSocket("bbb");
    Connection::initSocket("bbb");
    Connection::initSocket("aaa");

    QVERIFY2(Connection::socketPool.size() == prevNum + 2,  "Failure");

    QVERIFY2(Connection::getSocket("aaa") != -1, "Failure");
    QVERIFY2(Connection::getSocket("ccc") == -1, "Failure");
    QVERIFY2(Connection::getSocket("bbb") != -1, "Failure");

    unlink("aaa");
    unlink("bbb");
}

/* 
 * Check that closeConn() reset socket connection
 */
void  Ut_Connection::testAcceptConnection()
{
    char* socketName = (char*) "testAccept";

    Connection::initSocket(socketName);
    MyConnection* conn = new MyConnection(socketName);
    conn->m_fd = 1000;

    QVERIFY2(conn->acceptConn() == true,  "Failure");
    QVERIFY2(conn->m_fd > 0, "Failure");

    conn->closeConn();
    QVERIFY2(conn->m_fd == -1, "Failure");

    unlink("testAccept");
}

/* 
 * Check that env variable passed from invoker will 
 * be set in launcher process
 */
void  Ut_Connection::testGetEnv()
{
    QVERIFY2(getenv("MY_TEST_ENV_VAR") == NULL,  "Failure");
    QVERIFY2(getenv("PATH") != NULL,  "Failure");

    char* socketName = (char*) "testGetEnv";
    Connection::initSocket(socketName);
    MyConnection* conn = new MyConnection(socketName);

    char* envVar = (char*) "MY_TEST_ENV_VAR=3";

    conn->nextMsg = 1;
    conn->nextStr = envVar; 

    QVERIFY2(conn->getEnv() == true,  "Failure");

    QVERIFY2(getenv("MY_TEST_ENV_VAR") != NULL,  "Failure");
    QVERIFY2(getenv("PATH") != NULL,  "Failure");

    unlink(socketName);
}

/*
 * Check getAppName() function correctness
 */
void Ut_Connection::testGetAppName()
{
    char* socketName = (char*) "testGetAppName";
    Connection::initSocket(socketName);
    MyConnection* conn = new MyConnection(socketName);

    // wrong type of message
    conn->nextMsg = Connection::INVOKER_MSG_EXEC;
    string wrongStr = conn->getAppName();
    QVERIFY2(wrongStr.empty(), "Failure");

    // empty app name
    conn->nextMsg = Connection::INVOKER_MSG_NAME;
    conn->nextStr = NULL;
    string emptyName = conn->getAppName();
    QVERIFY2(emptyName.empty(), "Failure");

    // real name
    string realName("looooongApplicationName");
    char* dupName = strdup(realName.c_str());

    conn->nextMsg = Connection::INVOKER_MSG_NAME;
    conn->nextStr = dupName;

    string resName = conn->getAppName();
    QVERIFY2(!resName.empty(), "Failure");
    QVERIFY2(resName.compare(realName) == 0, "Failure");

    unlink(socketName);
}

QTEST_APPLESS_MAIN(Ut_Connection);
