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

Ut_Booster::Ut_Booster()
{}

Ut_Booster::~Ut_Booster()
{}

void Ut_Booster::initTestCase()
{}

void Ut_Booster::cleanupTestCase()
{}

#include <iostream>

void Ut_Booster::testRenameProcess()
{
    m_subject.reset(new MyBooster);

    // Define and copy args because it's assumed that they are allocated in the heap
    // (AppData deletes the argv on exit)
    const int ARGS = 3;
    m_subject->m_app.argc    = ARGS;
    m_subject->m_app.argv    = new char * [ARGS];
    m_subject->m_app.argv[0] = strdup("newName");
    m_subject->m_app.argv[1] = strdup("--foo");
    m_subject->m_app.argv[2] = strdup("--bar");

    // 50 chars buffer used to fool ps and top to show correct process name
    // of the launched process
    const int INIT_ARGS = 2;
    char ** initialArgv = new char * [INIT_ARGS];
    initialArgv[0]      = strdup("applauncherd");
    initialArgv[1]      = strdup("                                                  ");

    m_subject->renameProcess(INIT_ARGS, const_cast<char **>(initialArgv));

    QVERIFY2(strcmp(initialArgv[0], m_subject->m_app.argv[0]) == 0, "Failure");
    QVERIFY2(strcmp(initialArgv[1], " --foo --bar") == 0, "Failure");

    delete initialArgv[0];
    delete initialArgv[1];
    delete [] initialArgv;
}

void Ut_Booster::testRenameProcessNotEnoughSpace()
{
    m_subject.reset(new MyBooster);

    // Define and copy args because it's assumed that they are allocated in the heap
    // (AppData deletes the argv on exit)
    const int ARGS = 3;
    m_subject->m_app.argc    = ARGS;
    m_subject->m_app.argv    = new char * [ARGS];
    m_subject->m_app.argv[0] = strdup("newNameLongLong");
    m_subject->m_app.argv[1] = strdup("--foo");
    m_subject->m_app.argv[2] = strdup("--bar");

    const int INIT_ARGS = 2;
    char ** initialArgv = new char * [INIT_ARGS];
    initialArgv[0]      = strdup("applauncherd");
    initialArgv[1]      = strdup("");

    int initLen = strlen(initialArgv[0]);
    m_subject->renameProcess(INIT_ARGS, initialArgv);

    QVERIFY2(strncmp(initialArgv[0], m_subject->m_app.argv[0], initLen - 1) == 0, "Failure");
    QVERIFY2(strcmp(initialArgv[1], "") == 0, "Failure");

    delete initialArgv[0];
    delete initialArgv[1];
    delete [] initialArgv;
}

void Ut_Booster::testRenameProcessNotEnoughSpace2()
{
    m_subject.reset(new MyBooster);

    // Define and copy args because it's assumed that they are allocated in the heap
    // (AppData deletes the argv on exit)
    const int ARGS = 3;
    m_subject->m_app.argc    = ARGS;
    m_subject->m_app.argv    = new char * [ARGS];
    m_subject->m_app.argv[0] = strdup("newNameLongLong");
    m_subject->m_app.argv[1] = strdup("--foo");
    m_subject->m_app.argv[2] = strdup("--bar");

    const int INIT_ARGS = 2;
    char ** initialArgv = new char * [INIT_ARGS];
    initialArgv[0]      = strdup("applauncherd");
    initialArgv[1]      = strdup("      ");

    m_subject->renameProcess(INIT_ARGS, initialArgv);

    QVERIFY2(strcmp(initialArgv[1], " --foo") == 0, "Failure");

    delete initialArgv[0];
    delete initialArgv[1];
    delete [] initialArgv;
}

void Ut_Booster::testRenameProcessNoArgs()
{
    m_subject.reset(new MyBooster);

    // Define and copy args because it's assumed that they are allocated in the heap
    // (AppData deletes the argv on exit)
    const int ARGS = 2;
    m_subject->m_app.argv    = new char * [ARGS];
    m_subject->m_app.argc    = ARGS;
    m_subject->m_app.argv[0] = strdup("newName");
    m_subject->m_app.argv[1] = strdup("--foo");

    const int INIT_ARGS = 1;
    char ** initialArgv = new char * [INIT_ARGS];
    initialArgv[0]      = strdup("applauncherd");

    m_subject->renameProcess(INIT_ARGS, initialArgv);

    QVERIFY2(strcmp(initialArgv[0], m_subject->m_app.argv[0]) == 0, "Failure");

    delete initialArgv[0];
    delete [] initialArgv;
}

QTEST_APPLESS_MAIN(Ut_Booster);

