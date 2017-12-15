/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   synchconsole. h
 * Author: kadir
 *
 * Created on December 12, 2017, 1:06 PM
 */

#ifndef SYNCHCONSOLE_H
#define SYNCHCONSOLE_H
#include "copyright.h"
#include "utility.h"
#include "console.h"

class SynchConsole {
public:
    SynchConsole(char *readFile, char *writeFile); // initialize the hardware console device

    ~SynchConsole(); // clean up console emulation

    void SynchPutChar(const char ch); // Unix putchar(3S)
    char SynchGetChar(); // Unix getchar(3S)

    void SynchPutString(const char *s); // Unix puts(3S)
    void SynchGetString(char *s, int n); // Unix fgets(3S)

    void SynchPutInt(int n);
    void SynchGetInt(int *n);

private:
    Console *console;

    void _SynchPutChar(const char ch);
    char _SynchGetChar();
    void getAccessToConsole(int type);
    void endAccessToConsole(int type);
};
#endif // SYNCHCONSOLE_H

