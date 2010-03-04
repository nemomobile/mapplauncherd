#ifndef UT_DAEMON_H
#define UT_DAEMON_H

#include<QtTest/QtTest>
#include<QObject>

#include <tr1/memory>

#define UNIT_TEST

class Daemon;

class Ut_Daemon : public QObject
{
    Q_OBJECT

public:
    Ut_Daemon();
    ~Ut_Daemon();
private Q_SLOTS:

    void initTestCase();
    void cleanupTestCase();
    void testInitialArguments();
    void testParseArgs();
    void testVerifyInstance();
    void testReapZombies();

private:
    std::tr1::shared_ptr<Daemon> m_subject;
};

#endif // UT_DAEMON_H


