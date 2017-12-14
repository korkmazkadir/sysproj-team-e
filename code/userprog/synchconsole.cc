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

/*!
 * A version of put char that grabs a lock on write operations to console
 * Should only be used from outside (i.e. not from this class)
 */
void SynchConsole::SynchPutChar(char ch)
{
    writeMutex.P();
    {
        synchPutChar(ch);
    }
    writeMutex.V();
}

/*!
 * A version of get char that grabs a lock on read operations to console
 * Should only be used from outside (i.e. not from this class)
 */
int SynchConsole::SynchGetChar()
{
    int retVal;
    readMutex.P();
    {
        retVal = synchGetChar();
    }
    readMutex.V();
    return retVal;
}

void SynchConsole::SynchPutString(const char *s)
{
    writeMutex.P();
    {
        while (0 != *s) {
            synchPutChar(*s);
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
            int nextChar = synchGetChar();
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

/*!
 * Private version of putchar function that contains
 * actual put char functionality without using any mutexes
 */
void SynchConsole::synchPutChar(char ch)
{
    m_Console->PutChar(ch);
    writeAvail.P();
}

int SynchConsole::synchGetChar()
{
    readAvail.P();

    int retVal = (int)m_Console->GetChar();
    return retVal;
}
