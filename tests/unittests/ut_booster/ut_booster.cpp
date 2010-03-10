/*
 * ut_booster.cpp
 *
 * This file is part of applauncherd
 *
 * Copyright (C) 2010 Nokia Corporation. All rights reserved.
 *
 * This software, including documentation, is protected by copyright
 * controlled by Nokia Corporation. All rights are reserved.
 * Copying, including reproducing, storing, adapting or translating,
 * any or all of this material requires the prior written consent of
 * Nokia Corporation. This material also contains confidential
 * information which may not be disclosed to others without the prior
 * written consent of Nokia.
 */

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

char ** Ut_Booster::packTwoArgs(const char * arg0, const char * arg1)
{
    char ** argv  = new char * [2];
    char * result = new char[strlen(arg0) + strlen(arg1) + 2];
    memset(result, '\0', strlen(arg0) + strlen(arg1) + 2);

    strcat(result, arg0);
    strcat(result, " ");
    strcat(result, arg1);

    // Arguments are allocated consecutively in Linux
    argv[0] = result;
    argv[1] = argv[0] + strlen(arg0) + 1;
    argv[0][strlen(arg0)] = '\0';

    return argv;
}

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

    m_subject->m_app.appName = string(m_subject->m_app.argv[0]);

    // 20 chars dummy buffer used to fool ps to show correct process name with args
    const int INIT_ARGS = 2;
    char ** initialArgv = packTwoArgs("oldName", "                    ");
    m_subject->renameProcess(INIT_ARGS, const_cast<char **>(initialArgv));

    // New name and arguments fit and are correct
    QVERIFY2(strcmp(initialArgv[0], "newName --foo --bar") == 0, "Failure");

    delete initialArgv[0];
    delete [] initialArgv;
}

void Ut_Booster::testRenameProcessNotEnoughSpace()
{
    m_subject.reset(new MyBooster);

    const int ARGS = 3;
    m_subject->m_app.argc    = ARGS;
    m_subject->m_app.argv    = new char * [ARGS];
    m_subject->m_app.argv[0] = strdup("newNameLong");
    m_subject->m_app.argv[1] = strdup("--foo");
    m_subject->m_app.argv[2] = strdup("--bar");

    m_subject->m_app.appName = string(m_subject->m_app.argv[0]);

    const int INIT_ARGS = 2;
    char ** initialArgv = packTwoArgs("oldName", "   ");
    int initLen = strlen(initialArgv[0]);
    m_subject->renameProcess(INIT_ARGS, initialArgv);

    // Not enough space for the new name nor the arguments:
    // name should be cut
    QVERIFY2(strncmp(initialArgv[0], m_subject->m_app.argv[0], initLen - 1) == 0, "Failure");

    delete [] initialArgv[0];
    delete [] initialArgv;
}

void Ut_Booster::testRenameProcessNotEnoughSpace2()
{
    m_subject.reset(new MyBooster);

    const int ARGS = 3;
    m_subject->m_app.argc    = ARGS;
    m_subject->m_app.argv    = new char * [ARGS];
    m_subject->m_app.argv[0] = strdup("newName");
    m_subject->m_app.argv[1] = strdup("--foo");
    m_subject->m_app.argv[2] = strdup("--bar");

    m_subject->m_app.appName = string(m_subject->m_app.argv[0]);

    const int INIT_ARGS = 2;
    char ** initialArgv = packTwoArgs("oldName", "        ");
    m_subject->renameProcess(INIT_ARGS, initialArgv);

    // Not enough space for the second argument:
    // second argument should be left out
    QVERIFY2(strcmp(initialArgv[0], "newName --foo") == 0, "Failure");

    delete initialArgv[0];
    delete [] initialArgv;
}

void Ut_Booster::testRenameProcessNotEnoughSpace3()
{
    m_subject.reset(new MyBooster);

    const int ARGS = 3;
    m_subject->m_app.argc    = ARGS;
    m_subject->m_app.argv    = new char * [ARGS];
    m_subject->m_app.argv[0] = strdup("newName");
    m_subject->m_app.argv[1] = strdup("--foo");
    m_subject->m_app.argv[2] = strdup("--bar");

    m_subject->m_app.appName = string(m_subject->m_app.argv[0]);

    const int INIT_ARGS = 2;
    char ** initialArgv = packTwoArgs("app", "....");

    m_subject->renameProcess(INIT_ARGS, initialArgv);

    // Not enough space for arguments but just enough space
    // for the new name
    QVERIFY2(strcmp(initialArgv[0], "newName") == 0, "Failure");

    delete initialArgv[0];
    delete [] initialArgv;
}

void Ut_Booster::testRenameProcessNotEnoughSpace4()
{
    m_subject.reset(new MyBooster);

    const int ARGS = 3;
    m_subject->m_app.argc    = ARGS;
    m_subject->m_app.argv    = new char * [ARGS];
    m_subject->m_app.argv[0] = strdup("newNameLongLong");
    m_subject->m_app.argv[1] = strdup("--foo");
    m_subject->m_app.argv[2] = strdup("--bar");

    m_subject->m_app.appName = string(m_subject->m_app.argv[0]);

    const int INIT_ARGS = 2;
    char ** initialArgv = packTwoArgs("app", "   ");
    m_subject->renameProcess(INIT_ARGS, initialArgv);

    // Not enough space for newName, but dummy space exist: should be cut
    QVERIFY2(strcmp(initialArgv[0], "newName") == 0, "Failure");

    delete initialArgv[0];
    delete [] initialArgv;
}

void Ut_Booster::testRenameProcessNoArgs()
{
    m_subject.reset(new MyBooster);

    const int ARGS = 2;
    m_subject->m_app.argv    = new char * [ARGS];
    m_subject->m_app.argc    = ARGS;
    m_subject->m_app.argv[0] = strdup("newName");
    m_subject->m_app.argv[1] = strdup("--foo");

    m_subject->m_app.appName = string(m_subject->m_app.argv[0]);

    const int INIT_ARGS = 1;
    char ** initialArgv = new char * [INIT_ARGS];
    initialArgv[0]      = strdup("oldName");

    m_subject->renameProcess(INIT_ARGS, initialArgv);

    // No dummy space argument at all, only name fits

    QVERIFY2(strcmp(initialArgv[0], m_subject->m_app.argv[0]) == 0, "Failure");

    delete initialArgv[0];
    delete [] initialArgv;
}

QTEST_APPLESS_MAIN(Ut_Booster);

