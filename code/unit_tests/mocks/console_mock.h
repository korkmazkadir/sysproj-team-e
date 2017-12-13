
#ifndef CONSOLE_MOCK_H
#define CONSOLE_MOCK_H

#include "utility.h"


class SemaphoreMock
{
  public:
    SemaphoreMock (const char *, int ) { }
    ~SemaphoreMock () { }	// de-allocate semaphore
        // debugging assist

    void P () { }			// these are the only operations on a semaphore
    void V () { }			// they are both *atomic*
};


class ConsoleMock {
  public:
    ConsoleMock(char *, char *, VoidFunctionPtr ,
    VoidFunctionPtr , int ):
    nextExpectedChar(0)
  , lastPutChar(0)
    {  }

    ~ConsoleMock() { }

    void PutChar(char ch) { lastPutChar = ch; }

    char GetChar() { return nextExpectedChar; }



public:
    char nextExpectedChar;
    char lastPutChar;
};

#endif // CONSOLE_H
