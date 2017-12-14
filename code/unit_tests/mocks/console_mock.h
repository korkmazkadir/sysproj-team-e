
#ifndef CONSOLE_MOCK_H
#define CONSOLE_MOCK_H

#include "utility.h"
#include <string>

class SemaphoreMock
{
  public:
    SemaphoreMock (const char *, int ) { }
    ~SemaphoreMock ()
         { }


    void P () { ++pCalledTimes; }
    void V () { ++vCalledTimes; }

    /*!
     * These two static variables are used as global counters of semaphore usage
     * It allows to intercept all the calls to Semaphore P and V function
     * and ensure that a particular function was using synchronization
     */
    static int pCalledTimes;
    static int vCalledTimes;
};

int SemaphoreMock::pCalledTimes = 0;
int SemaphoreMock::vCalledTimes = 0;

class ConsoleMock {
  public:
    ConsoleMock(char *, char *, VoidFunctionPtr ,
    VoidFunctionPtr , int ):
    nextExpectedChar(0)
  , lastPutChar(0)
    {  }

    ~ConsoleMock() { }

    void PutChar(char ch) {
        lastPutChar = ch;
        allPutChar.push_back(lastPutChar);
    }

    /*!
     * If expected string is non-empty, returns a first character from there and removes it
     * Otherwise returns the value of nextExpectedChar
     */
    char GetChar() {
        char retVal;
        if (expectedString.empty()) {
            retVal = nextExpectedChar;
        } else {
            retVal = expectedString.front();
            expectedString.erase(0, 1);
        }
        return retVal;
    }



public:
    char nextExpectedChar;
    char lastPutChar;

    std::string allPutChar;
    std::string expectedString;
};

#endif // CONSOLE_H
