#include "synchconsole.h"

static Semaphore *readAvail = NULL;
static Semaphore *writeAvail = NULL;

SynchConsole::SynchConsole(char *readFile, char *writeFile)
{
    readAvail = new Semaphore("synch_console_read_avail", 0);
    writeAvail = new Semaphore("synch_console_write_avail", 0);
    m_Console = new Console(readFile, writeFile, &SynchConsole::ReadAvail, &SynchConsole::WriteDone, 0);
}

SynchConsole::~SynchConsole()
{
    delete m_Console;
}

void SynchConsole::SynchPutChar(char ch)
{
    m_Console->PutChar(ch);
    writeAvail->P();
}

char SynchConsole::SynchGetChar()
{
    char retVal = EOF;
    readAvail->P();
    retVal = m_Console->GetChar();
    return retVal;
}

void SynchConsole::SynchPutString(const char *s)
{
    while (*s != 0) {
        SynchPutChar(*s);
        ++s;
    }
}

void SynchConsole::SynchGetString(char *s, int n)
{
    int ii = 0;
    for (ii = 0; ii < n - 1; ++ii) {
        char nextChar = SynchGetChar();
        if ((EOF == nextChar) || ('\n' == nextChar)) {
            break;
        }
        *(s + ii) = nextChar;
    }
    *(s + ii) = 0;
}

void SynchConsole::ReadAvail(int arg)
{
    (void)arg;
    readAvail->V ();
}

void SynchConsole::WriteDone(int arg)
{
    (void)arg;
    writeAvail->V();
}
