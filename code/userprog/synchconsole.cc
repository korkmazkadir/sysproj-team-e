/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   synchconsole.c
 * Author: kadir
 *
 * Created on December 12, 2017, 1:12 PM
 */

#include "copyright.h"
#include "system.h"
#include "synchconsole.h"
#include "synch.h"

#define READ_ACCESS  0
#define WRITE_ACCESS  1

static Semaphore *readAvail;
static Semaphore *writeDone;
static Semaphore *readAccessConsole;
static Semaphore *writeAccessConsole;

//----------------------------------------------------------------------
// ConsoleInterruptHandlers
//      Wake up the thread that requested the I/O.
//----------------------------------------------------------------------

static void
ReadAvail(int arg) {
    readAvail->V();
}

static void
WriteDone(int arg) {
    writeDone->V();
}

SynchConsole::SynchConsole(char *readFile, char *writeFile) {
    console = new Console(readFile, writeFile, ReadAvail, WriteDone, 0);
    readAvail = new Semaphore("read avail", 0);
    writeDone = new Semaphore("write done", 0);
    readAccessConsole = new Semaphore("Console Semaphore", 0);
    writeAccessConsole = new Semaphore("Console Semaphore", 0);
}

SynchConsole::~SynchConsole() {
    delete console;
    delete writeDone;
    delete readAvail;
    delete readAccessConsole;
    delete writeAccessConsole;
}

void SynchConsole::SynchPutChar(const char ch) {
    
    getAccessToConsole(WRITE_ACCESS);
    
    _SynchPutChar(ch);
    
    endAccessToConsole(WRITE_ACCESS);
    
}

char SynchConsole::SynchGetChar() {
    
    getAccessToConsole(READ_ACCESS);
    
    return _SynchGetChar();
    
    endAccessToConsole(READ_ACCESS);
    
}

void SynchConsole::_SynchPutChar(const char ch){
    console->PutChar(ch);
    writeDone->P();
}
char SynchConsole::_SynchGetChar(){
    readAvail->P();
    return console->GetChar();
}

void SynchConsole::SynchPutString(const char *s) {
    
    getAccessToConsole(WRITE_ACCESS);
            
    for (int i = 0; s[i] != '\0'; i++) {
        _SynchPutChar(s[i]);
    }
    
    endAccessToConsole(WRITE_ACCESS);
}

void SynchConsole::SynchGetString(char *s, int n) {
    
    getAccessToConsole(READ_ACCESS);
    
    int i=0;
    for (; i < n; i++) {
                
        s[i] = _SynchGetChar();
        
        if(s[i] == EOF){
            s[i] = '\0';
            break;
        }
        
        if(s[i] == '\n')
            break;
    }
    
    if(s[i] != '\0'){
        s[i+1] = '\0';
    }

    endAccessToConsole(READ_ACCESS);
    
}


void SynchConsole::SynchPutInt(int n){
    char buffer[MAX_INT_STRING_SIZE];
    snprintf(buffer,MAX_INT_STRING_SIZE,"%d",n);
    SynchPutString(buffer);
}

void SynchConsole::SynchGetInt(int *n){
    char buffer[MAX_INT_STRING_SIZE];
    SynchGetString(buffer,MAX_INT_STRING_SIZE);
    sscanf(buffer,"%d",n);
}

void SynchConsole::getAccessToConsole(int type){
    if(type == READ_ACCESS){
        readAccessConsole->V();
    }else if(type == WRITE_ACCESS){
        writeAccessConsole->V();
    }
}

void SynchConsole::endAccessToConsole(int type){
    if(type == READ_ACCESS){
        readAccessConsole->P();
    }else if(type == WRITE_ACCESS){
        writeAccessConsole->P();
    }
}