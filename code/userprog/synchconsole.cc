#include "synchconsole.h"

static Semaphore readAvail("synch_console_read_avail", 0);
static Semaphore writeAvail("synch_console_write_avail", 0);

static Semaphore readMutex("SynchConsole:ReadMutex", 1);
static Semaphore writeMutex("SynchConsole:WriteMutex", 1);


SynchConsole::SynchConsole(char *readFile, char *writeFile)
{
    m_Console = new Console(readFile, writeFile, &SynchConsole::ReadAvail, &SynchConsole::WriteDone, 0);
}

SynchConsole::~SynchConsole()
{
    delete m_Console;
}

void SynchConsole::SynchPutChar(char ch)
{
    m_Console->PutChar(ch);
    writeAvail.P();
}

int SynchConsole::SynchGetChar()
{
    int retVal = EOF;
    readAvail.P();

    retVal = (int)m_Console->GetChar();
    return retVal;
}

void SynchConsole::SynchPutString(const char *s)
{
    writeMutex.P();
    {
        while (0 != *s) {
            SynchPutChar(*s);
            ++s;
        }
    }
    writeMutex.V();
}

void SynchConsole::SynchGetString(char *s, int n)
{
    readMutex.P();
    {
        int ii = 0;
        for (ii = 0; ii < n - 1; ++ii) {
            int nextChar = SynchGetChar();
            char cvtChar = (char)nextChar;
            if ((EOF == nextChar) || ('\n' == cvtChar)) {
                break;
            }
            *(s + ii) = cvtChar;
        }
        *(s + ii) = 0;
    }
    readMutex.V();
}

void SynchConsole::ReadAvail(int arg)
{
    (void)arg;
    readAvail.V ();
}

void SynchConsole::WriteDone(int arg)
{
    (void)arg;
    writeAvail.V();
}
