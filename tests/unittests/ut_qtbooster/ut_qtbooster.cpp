#include "ut_qtbooster.h"
#include "qtbooster.h"

Ut_QtBooster::Ut_QtBooster() :
    m_subject(new QtBooster)
{}

Ut_QtBooster::~Ut_QtBooster()
{}

void Ut_QtBooster::initTestCase()
{}

void Ut_QtBooster::cleanupTestCase()
{}

void Ut_QtBooster::testSocketName()
{
    QVERIFY2(QtBooster::socketName() == QtBooster::m_socketId, "Failure");
    QVERIFY2(m_subject->socketId() == QtBooster::m_socketId, "Failure");
}

void Ut_QtBooster::testType()
{
    QVERIFY2(QtBooster::type() == 'q', "Failure");
    QVERIFY2(m_subject->boosterType() == 'q', "Failure");
}

QTEST_APPLESS_MAIN(Ut_QtBooster);

