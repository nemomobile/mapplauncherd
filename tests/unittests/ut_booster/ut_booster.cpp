#include "ut_booster.h"
#include "booster.h"

// Booster is an abstract base-class, so let's inherit it
class MyBooster : public Booster
{
public:
    MyBooster();
    char boosterType() const;
    const std::string & socketId() const;
private:
    const string m_socketId;
};

MyBooster::MyBooster() :
    m_socketId("/tmp/MyBooster")
{}

char MyBooster::boosterType() const
{
    return 'm';
}

const std::string & MyBooster::socketId() const
{
    return m_socketId;
}

Ut_Booster::Ut_Booster() :
    m_subject(new MyBooster)
{}

Ut_Booster::~Ut_Booster()
{}

void Ut_Booster::initTestCase()
{}

void Ut_Booster::cleanupTestCase()
{}

QTEST_APPLESS_MAIN(Ut_Booster);

