#include <stdlib.h>
#include "test_utils.h"

#include "console_mock.h"

#define CONSOLE_H
#define SYNCH_H
#define Console ConsoleMock
#define Semaphore SemaphoreMock

#include "synchconsole.cc"

int main () {
    SynchConsole testConsole(NULL, NULL);
    testConsole.m_Console->nextExpectedChar = 't';
    char obtainedChar = testConsole.SynchGetChar();

    EXPECT_EQ(testConsole.m_Console->nextExpectedChar, obtainedChar);
}
