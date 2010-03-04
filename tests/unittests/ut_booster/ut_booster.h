#ifndef UT_BOOSTER_H
#define UT_BOOSTER_H

#include<QtTest/QtTest>
#include<QObject>

#include <tr1/memory>

#define UNIT_TEST

class Booster;

class Ut_Booster : public QObject
{
    Q_OBJECT

public:
    Ut_Booster();
    virtual ~Ut_Booster();

private Q_SLOTS:
    void initTestCase();
    void testRenameProcess();
    void testRenameProcessNotEnoughSpace();
    void testRenameProcessNotEnoughSpace2();
    void testRenameProcessNotEnoughSpace3();
    void testRenameProcessNotEnoughSpace4();
    void testRenameProcessNoArgs();
    void cleanupTestCase();

private:
    std::tr1::shared_ptr<Booster> m_subject;
};

#endif // UT_BOOSTER_H


