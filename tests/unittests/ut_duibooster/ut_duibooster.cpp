#include "ut_duibooster.h"
#include "duibooster.h"

Ut_DuiBooster::Ut_DuiBooster() :
    m_subject(new DuiBooster)
{}

Ut_DuiBooster::~Ut_DuiBooster()
{}

void Ut_DuiBooster::initTestCase()
{}

void Ut_DuiBooster::cleanupTestCase()
{}

void Ut_DuiBooster::testSocketName()
{
    QVERIFY2(DuiBooster::socketName() == DuiBooster::m_socketId, "Failure");
    QVERIFY2(m_subject->socketId() == DuiBooster::m_socketId, "Failure");
}

void Ut_DuiBooster::testType()
{
    QVERIFY2(DuiBooster::type() == 'd', "Failure");
    QVERIFY2(m_subject->boosterType() == 'd', "Failure");
}

QTEST_APPLESS_MAIN(Ut_DuiBooster);

