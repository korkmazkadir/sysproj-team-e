// exception.cc 
//      Entry point into the Nachos kernel from user programs.
//      There are two kinds of things that can cause control to
//      transfer back to here from user code:
//
//      syscall -- The user code explicitly requests to call a procedure
//      in the Nachos kernel.  Right now, the only function we support is
//      "Halt".
//
//      exceptions -- The user code does something that the CPU can't handle.
//      For instance, accessing memory that doesn't exist, arithmetic errors,
//      etc.  
//
//      Interrupts (which can also cause control to transfer from user
//      code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"

//----------------------------------------------------------------------
// UpdatePC : Increments the Program Counter register in order to resume
// the user program immediately after the "syscall" instruction.
//----------------------------------------------------------------------

static void
UpdatePC() {
    int pc = machine->ReadRegister(PCReg);
    machine->WriteRegister(PrevPCReg, pc);
    pc = machine->ReadRegister(NextPCReg);
    machine->WriteRegister(PCReg, pc);
    pc += 4;
    machine->WriteRegister(NextPCReg, pc);
}

void copyStringFromMachine(int from, char *to, unsigned size) {
    int machineMemoryBuffer;
    unsigned i = 0;
    const int wordSize = 1;
    for (; i < size; i++) {
        machine->ReadMem(from + i, wordSize, &machineMemoryBuffer);
        to[i] = (char)machineMemoryBuffer;
        if(to[i] == '\0'){
            return;
        }
    }
    to[i] = '\0';
}

void putStringToMachine(char *from, int to, unsigned size) {
    const int wordSize = 1;
    for (unsigned i = 0; i < size; i++) {
        machine->WriteMem(to + i, wordSize, (int)from[i] );
    }
}

void putIntegerToMachine(int from, int to){
    const int wordSize = 4;
    machine->WriteMem(to , wordSize, from );
}

//----------------------------------------------------------------------
// ExceptionHandler
//      Entry point into the Nachos kernel.  Called when a user program
//      is executing, and either does a syscall, or generates an addressing
//      or arithmetic exception.
//
//      For system calls, the following is the calling convention:
//
//      system call code -- r2
//              arg1 -- r4
//              arg2 -- r5
//              arg3 -- r6
//              arg4 -- r7
//
//      The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//      "which" is the kind of exception.  The list of possible exceptions 
//      are in machine.h.
//----------------------------------------------------------------------

void checkSizeOfTheString(int *sizeOfTheString){
    if(*sizeOfTheString >= MAX_STRING_SIZE){
        *sizeOfTheString = MAX_STRING_SIZE - 1;
    }
}

void
ExceptionHandler(ExceptionType which) {
    int type = machine->ReadRegister(2);

    if ((which == SyscallException)) {

        switch (type) {

            case SC_Exit:
            {
                int returnValue = machine->ReadRegister(4);
                DEBUG('a', "Exiting program. Return value %d\n", returnValue);
                Exit(returnValue);
                break;
            }

            case SC_Halt:
            {
                DEBUG('a', "Shutdown, initiated by user program.\n");
                printf("Halt... %s \n", currentThread->getName());
                interrupt->Halt();
                break;
            }

            case SC_PutChar:
            {
                DEBUG('a', "Put char working.\n");
                interrupt->PutChar((char) machine->ReadRegister(4));
                break;
            }

            case SC_SynchPutString:
            {
                DEBUG('a', "Put string working.\n");
                char outBuffer [MAX_STRING_SIZE];
                int addressOfTheString = machine->ReadRegister(4);       
     
                copyStringFromMachine(addressOfTheString, outBuffer, MAX_STRING_SIZE);
                
                interrupt->SynchPutString(outBuffer);
                break;
            }

            case SC_SynchGetChar:
            {
                DEBUG('a', "Get char working.\n");
                char ch = interrupt->SynchGetChar();
                machine->WriteRegister(2,(int)ch);
                break;
            }

            case SC_SynchGetString:
            {
                DEBUG('a', "Get string working.\n");
                char inputBuffer [MAX_STRING_SIZE];
                int addressOfTheString = machine->ReadRegister(4);
                int sizeOfTheString = machine->ReadRegister(5);
                
                checkSizeOfTheString(&sizeOfTheString);
                
                interrupt->SynchGetString(inputBuffer,sizeOfTheString);
                putStringToMachine(inputBuffer, addressOfTheString, sizeOfTheString);
                break;
            }

            case SC_SynchPutInt:
            {
                DEBUG('a', "Put integer working.\n");
                int value = machine->ReadRegister(4);
                interrupt->SynchPutInt(value);
                break;
            }
            
            case SC_SynchGetInt:
            {
                DEBUG('a', "Get integer working.\n");
                int address = machine->ReadRegister(4);
                int value;
                interrupt->SynchGetInt(&value);
                putIntegerToMachine(value, address);
                break;
            }
            
            case SC_AssertionFailed:
            {
                int fileNameAddress = machine->ReadRegister(4);
                int lineNumber = machine->ReadRegister(5);
                char fileName [MAX_STRING_SIZE];
                copyStringFromMachine(fileNameAddress, fileName, MAX_STRING_SIZE);
                interrupt->AssertionFailed(fileName,lineNumber);
                break;
            }
            
            case SC_ForkExec:
            {
                int fileNameAddress = machine->ReadRegister(4);
                char fileName [MAX_STRING_SIZE];
                copyStringFromMachine(fileNameAddress, fileName, MAX_STRING_SIZE);
                int result = createProcess(fileName);
                machine->WriteRegister(2,result);
                break;
            }
                    
             case SC_Yield:
             {
                 currentThread->Yield();
                 break;
             }
            
            default:
            {
                printf("Unexpected user mode exception %d %d\n", which, type);
                ASSERT(FALSE);
            }

        }

        UpdatePC();
    }



    // LB: Do not forget to increment the pc before returning!

    // End of addition
}


