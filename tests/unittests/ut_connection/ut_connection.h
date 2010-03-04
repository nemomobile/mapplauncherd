#ifndef UT_CONNECTION_H
#define UT_CONNECTION_H

#include<QtTest/QtTest>
#include<QObject>

#include <tr1/memory>

#define UNIT_TEST

class Connection;

class Ut_Connection : public QObject
{
    Q_OBJECT

public:
    Ut_Connection();
    ~Ut_Connection();

private Q_SLOTS:

    void initTestCase();
    void cleanupTestCase();
    void testInitConnection();
    void testAcceptConnection();

private:
    std::tr1::shared_ptr<Connection> m_subject;
};

#endif // UT_CONNECTION_H
