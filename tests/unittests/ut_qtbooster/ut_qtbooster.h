#ifndef UT_QTBOOSTER_H
#define UT_QTBOOSTER_H

#include<QtTest/QtTest>
#include<QObject>

#include <tr1/memory>

#define UNIT_TEST

class QtBooster;

class Ut_QtBooster : public QObject
{
    Q_OBJECT

public:
    Ut_QtBooster();
    virtual ~Ut_QtBooster();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testSocketName();
    void testType();

private:
    std::tr1::shared_ptr<QtBooster> m_subject;
};

#endif // UT_QTBOOSTER_H


