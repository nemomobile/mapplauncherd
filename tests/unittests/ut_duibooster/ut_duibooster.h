#ifndef UT_DUIBOOSTER_H
#define UT_DUIBOOSTER_H

#include<QtTest/QtTest>
#include<QObject>

#include <tr1/memory>

#define UNIT_TEST

class DuiBooster;

class Ut_DuiBooster : public QObject
{
    Q_OBJECT

public:
    Ut_DuiBooster();
    virtual ~Ut_DuiBooster();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testSocketName();
    void testType();

private:
    std::tr1::shared_ptr<DuiBooster> m_subject;
};

#endif // UT_DUIBOOSTER_H


