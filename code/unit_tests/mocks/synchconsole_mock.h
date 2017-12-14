#ifndef SYNCHCONSOLE_MOCK_H
#define SYNCHCONSOLE_MOCK_H

#include <string>

class SynchConsoleMock {

public:
    SynchConsoleMock(char *, char *) { }
    // initialize the hardware console device
    ~SynchConsoleMock() { }

    void SynchPutChar(char ch) {
        lastPutChar = ch;
    }

    int SynchGetChar() {
        return (int)charToGet;
    }

    void SynchPutString(const char *s) {
        putString.append(s);
    }

    void SynchGetString(char *s, int n) {
        strncpy(s, stringToGet.c_str(), n - 1);
    }

    char lastPutChar;
    char charToGet;

    std::string stringToGet;
    std::string putString;

};

#endif // SYNCHCONSOLE_MOCK_H

