#include "ut_connection.h"
#include "connection.h"

Ut_Connection::Ut_Connection()
{
}

Ut_Connection::~Ut_Connection()
{
}

void Ut_Connection::initTestCase()
{
    Connection::initSocket("testAccept");
    m_subject.reset(new Connection("testAccept"));
}

void Ut_Connection::cleanupTestCase()
{
}

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

}

void  Ut_Connection::testAcceptConnection()
{
    /*
    QVERIFY2(m_subject->acceptConn() == true,  "Failure");
    QVERIFY2(m_subject->m_fd > 0, "Failure");

    m_subject->closeConn();
    QVERIFY2(m_subject->m_fd == -1, "Failure");

    QVERIFY2(m_subject->acceptConn() == true,  "Failure");

    m_subject->closeConn();
    */
}

QTEST_APPLESS_MAIN(Ut_Connection);
