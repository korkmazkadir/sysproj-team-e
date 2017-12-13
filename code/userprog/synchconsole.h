#ifndef SYNCHCONSOLE_H
#define SYNCHCONSOLE_H

#include "copyright.h"
#include "utility.h"
#include "console.h"
#include "synch.h"

class SynchConsole {

public:
    SynchConsole(char *readFile, char *writeFile);
    // initialize the hardware console device
    ~SynchConsole();

    void SynchPutChar(char ch);
    int SynchGetChar();
    void SynchPutString(const char *s);
    void SynchGetString(char *s, int n);
private:
    Console *m_Console;

    static void ReadAvail (int arg);
    static void WriteDone (int arg);

};
#endif // SYNCHCONSOLE_H
