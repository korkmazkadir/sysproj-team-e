#include <stdlib.h>
#include "test_utils.h"

#include "console_mock.h"
#include <limits.h>

#define CONSOLE_H
#define SYNCH_H
#define Console ConsoleMock
#define Semaphore SemaphoreMock

#include "synchconsole.cc"

static SynchConsole *testConsole = NULL;

void TestUtils_SetUp() {
    Semaphore::pCalledTimes = 0;
    Semaphore::vCalledTimes = 0;
    testConsole = new SynchConsole(NULL, NULL);
}

void TestUtils_TearDown() {
    delete testConsole;
}

TEST_LIST_BEGIN

DECLARE_TEST_BEGIN(SynchGetCharTest)
    for (int tc = (int)CHAR_MIN; tc <= (int)CHAR_MAX; ++tc) {
        testConsole->m_Console->nextExpectedChar = (char)tc;
        char obtainedChar = testConsole->SynchGetChar();

        EXPECT_EQ(testConsole->m_Console->nextExpectedChar, obtainedChar);
        EXPECT_LESS_EQUAL(1, Semaphore::pCalledTimes);
        Semaphore::pCalledTimes = 0;
    }
DECLARE_TEST_END(SynchGetCharTest)

DECLARE_TEST_BEGIN(SynchPutCharTest)
    for (int tc = (int)CHAR_MIN; tc <= (int)CHAR_MAX; ++tc) {
        testConsole->SynchPutChar(tc);
        EXPECT_EQ((char)tc, testConsole->m_Console->lastPutChar);
        EXPECT_LESS_EQUAL(1, Semaphore::pCalledTimes);
        Semaphore::pCalledTimes = 0;
    }
DECLARE_TEST_END(SynchPutCharTest)

DECLARE_TEST_BEGIN(SynchPutString)
    const char *testString = "test string\n";
    testConsole->SynchPutString(testString);
    EXPECT_STREQ(testString, testConsole->m_Console->allPutChar.c_str());
DECLARE_TEST_END(SynchPutString)

/*!
 *  Tests Get String for a string with a newline character in the middle
 */
DECLARE_TEST_BEGIN(SynchGetString_1)
    char buf[7];
    testConsole->m_Console->expectedString = "Hello\nWorld";
    testConsole->SynchGetString(buf, 90);
    EXPECT_STREQ("Hello\n", buf);
DECLARE_TEST_END(SynchGetString_1)

/*!
 *  Tests Get String for a string with a newline character in the end
 */
DECLARE_TEST_BEGIN(SynchGetString_2)
    char buf[13];
    testConsole->m_Console->expectedString = "Hello World\n";
    testConsole->SynchGetString(buf, 90);
    EXPECT_STREQ("Hello World\n", buf);
DECLARE_TEST_END(SynchGetString_2)

/*!
 *  Tests Get String for with a limited number of accepted characters
 */
DECLARE_TEST_BEGIN(SynchGetString_3)
    char buf[4];
    testConsole->m_Console->expectedString = "Hello World";
    testConsole->SynchGetString(buf, 4);
    EXPECT_STREQ("Hel", buf);
DECLARE_TEST_END(SynchGetString_3)

TEST_LIST_END

int main () {
    INITIALIZE_TESTS;
    RUN_ALL_TESTS;
}
