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
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "userthread.h"
#include "addrspace.h"
#include "usersemaphore.h"
#include "SynchPost.h"

Semaphore haltSync("HaltSync", 1);


//----------------------------------------------------------------------
// UpdatePC : Increments the Program Counter register in order to resume
// the user program immediately after the "syscall" instruction.
//----------------------------------------------------------------------
static void
UpdatePC ()
{
    int pc = machine->ReadRegister (PCReg);
    machine->WriteRegister (PrevPCReg, pc);
    pc = machine->ReadRegister (NextPCReg);
    machine->WriteRegister (PCReg, pc);
    pc += 4;
    machine->WriteRegister (NextPCReg, pc);
}




/*!
 * \fn copyStringFromMachine
 * \param from  <- MIPS virtual address
 * \param to    <- x86 'physical' address
 * \param size  <- max number of bytes to copy
 * \brief Copies up to \a size - 1 characters from virtual address \a from to 'physical'
 *        address \a to
 *        The last character (at pos size) of \a to is always set to 0
 */
static void copyStringFromMachine(int from, char *to, unsigned size) {
    --size;
    while (size--) {
        machine->ReadMem(from, 1, (int *)to);
        if (*to == 0) {
            break;
        }
        ++from;
        ++to;
    }
    *to = 0;
}

static void copyStringToMachine(char *from, int to, unsigned size) {
    while (size--) {
        machine->WriteMem(to, 1, (int)(*from));
        ++to;
        ++from;
    }
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
void ExceptionHandler (ExceptionType which)
{
    int type = machine->ReadRegister (SYSCALL_ID_REGISTER);

    switch (which) {
        case SyscallException:
        {
            switch (type) {
                case SC_Halt:
                {
                    printf("SHUTDONW %d \n", currentThread->Tid());
                    interrupt->Halt ();
                } break;

                case SC_PutChar:
                {
                    int requestedCharacter = machine->ReadRegister(FIRST_PARAM_REGISTER);
                    char convertedCharacter = (char)requestedCharacter;
                    syncConsole->SynchPutChar(convertedCharacter);
                } break;

                case SC_SynchPutString:
                {
                    int fromAddress = machine->ReadRegister(FIRST_PARAM_REGISTER);
                    char local_buf[MAX_WRITE_BUF_SIZE];
                    copyStringFromMachine(fromAddress, local_buf, MAX_WRITE_BUF_SIZE);
                    syncConsole->SynchPutString(local_buf);
                } break;

                case SC_SynchGetChar:
                {
                    int retChar = syncConsole->SynchGetChar();
                    machine->WriteRegister(RET_VALUE_REGISTER, retChar);
                } break;

                //TODO: Check for correctness
                case SC_SynchGetString:
                {
                    int toVirtualAddress = machine->ReadRegister(FIRST_PARAM_REGISTER);
                    int numBytes = machine->ReadRegister(SECOND_PARAM_REGISTER);

                    if (numBytes > MAX_WRITE_BUF_SIZE) {
                        numBytes = MAX_WRITE_BUF_SIZE;
                    }

                    char local_buf[MAX_WRITE_BUF_SIZE];
                    syncConsole->SynchGetString(local_buf, numBytes);
                    copyStringToMachine(local_buf, toVirtualAddress, numBytes);

                } break;

                case SC_SynchPutInt:
                {
                    int valToPut = machine->ReadRegister(FIRST_PARAM_REGISTER);
                    char local_buf[MAX_INT_LEN];
                    (void)snprintf(local_buf, MAX_INT_LEN, "%d", valToPut);
                    syncConsole->SynchPutString(local_buf);
                } break;

                case SC_SynchGetInt:
                {
                    int resAddress = machine->ReadRegister(FIRST_PARAM_REGISTER);
                    char local_buf[MAX_INT_LEN];
                    syncConsole->SynchGetString(local_buf, MAX_INT_LEN);
                    int val;
                    sscanf(local_buf, "%d", &val);
                    machine->WriteMem(resAddress, 4, val);
                } break;

                case SC_Exit:
                {
                    machine->ReadRegister(FIRST_PARAM_REGISTER);
                    //printf("EXIT %d %d \n", currentThread->Tid(), retValue);
                    do_ExitCurrentProcess();
                } break;

                case SC_UserThreadCreate:
                {
                    //TODO: When creation can fail -- invalid parameter (e.g. NULL pointer to function)?
                    int funPtr = machine->ReadRegister(FIRST_PARAM_REGISTER);
                    int arg = machine->ReadRegister(SECOND_PARAM_REGISTER);

                    int retAddress = machine->ReadRegister(8);

                    int threadRetVal = do_UserThreadCreate(funPtr, arg, retAddress, currentThread->space, false);
                    machine->WriteRegister(RET_VALUE_REGISTER, threadRetVal);
                } break;

                case SC_UserThreadExit:
                {
                    do_UserThreadExit();
                } break;

                case SC_UserThreadJoin:
                {
                    int threadToJoin = machine->ReadRegister(FIRST_PARAM_REGISTER);
                    int retVal = do_UserThreadJoin(threadToJoin);
                    machine->WriteRegister(RET_VALUE_REGISTER, retVal);
                } break;

                case SC_Yield:
                {
                    currentThread->Yield();
                } break;

                case SC_SemInit:
                {
                    int semPtr = machine->ReadRegister(FIRST_PARAM_REGISTER);
                    int semVal = machine->ReadRegister(SECOND_PARAM_REGISTER);
                    int retVal = semaphoreManager->DoSemInit((sem_t *)semPtr, semVal);
                    machine->WriteRegister(RET_VALUE_REGISTER, retVal);
                } break;

                case SC_SemPost:
                {
                    int semPtr = machine->ReadRegister(FIRST_PARAM_REGISTER);
                    int retVal = semaphoreManager->DoSemPost((sem_t *)semPtr);
                    machine->WriteRegister(RET_VALUE_REGISTER, retVal);
                } break;

                case SC_SemWait:
                {
                    int semPtr = machine->ReadRegister(FIRST_PARAM_REGISTER);
                    int retVal = semaphoreManager->DoSemWait((sem_t *)semPtr);
                    machine->WriteRegister(RET_VALUE_REGISTER, retVal);
                } break;

                case SC_SemDestroy:
                {
                    int semPtr = machine->ReadRegister(FIRST_PARAM_REGISTER);
                    int retVal = semaphoreManager->DoSemDestroy((sem_t *)semPtr);
                    machine->WriteRegister(RET_VALUE_REGISTER, retVal);
                } break;

                case SC_UserThreadSelfId:
                {
                    int tid = currentThread->Tid();
                    machine->WriteRegister(RET_VALUE_REGISTER, tid);
                } break;

                case SC_ForkExec:
                {
                    int fileNameAddress = machine->ReadRegister(FIRST_PARAM_REGISTER);
                    char fileName [MAX_WRITE_BUF_SIZE];
                    copyStringFromMachine(fileNameAddress, fileName, MAX_WRITE_BUF_SIZE);
                    int result = createProcess(fileName);
                    machine->WriteRegister(RET_VALUE_REGISTER,result);
                    break;
                }

                case SC_AssertionFailed:
                {
                    int fileNameAddress = machine->ReadRegister(FIRST_PARAM_REGISTER);
                    int lineNumber = machine->ReadRegister(SECOND_PARAM_REGISTER);
                    char fileName [MAX_WRITE_BUF_SIZE];
                    copyStringFromMachine(fileNameAddress, fileName, MAX_WRITE_BUF_SIZE);
                    fprintf(stderr,"\nERROR : Assertion failed. FILE : %s LINE : %d\n\n", fileName, lineNumber);
                    Exit (123);
                } break;

                case SC_NetworkConnectAsServer:
                {
                    int mailbox = machine->ReadRegister(FIRST_PARAM_REGISTER);

                    int retVal = synchPost->ConnectAsServer(mailbox);
                    machine->WriteRegister(RET_VALUE_REGISTER, retVal);
                } break;

                case SC_NetworkConnectAsClient:
                {
                    int addr = machine->ReadRegister(FIRST_PARAM_REGISTER);
                    int mailbox = machine->ReadRegister(SECOND_PARAM_REGISTER);
                    int retVal = synchPost->ConnectAsClient(addr, mailbox);
                    machine->WriteRegister(RET_VALUE_REGISTER, retVal);
                } break;

                case SC_NetworkSendToByConnId:
                {
                    int connId = machine->ReadRegister(FIRST_PARAM_REGISTER);
                    int dataAddr = machine->ReadRegister(SECOND_PARAM_REGISTER);
                    int len = machine->ReadRegister(THIRD_PARAM_REGISTER);

                    if (len >= NETWORK_MAX_TRANSFER_BYTES - 1) {
                        len = NETWORK_MAX_TRANSFER_BYTES - 1;
                    }
                    char buffer[NETWORK_MAX_TRANSFER_BYTES] = { 0 };
                    copyStringFromMachine(dataAddr, buffer, len + 1);
                    int retVal = synchPost->SendToByConnId(connId, buffer, len);
                    machine->WriteRegister(RET_VALUE_REGISTER, retVal);
                } break;

                case SC_NetworkReceiveFromByConnId:
                {
                    int connId = machine->ReadRegister(FIRST_PARAM_REGISTER);
                    int dataAddr = machine->ReadRegister(SECOND_PARAM_REGISTER);
                    char dataBuffer[NETWORK_MAX_TRANSFER_BYTES];
                    //TODO: Check for length
                    int bytesReceived = synchPost->ReceiveFromByConnId(connId, dataBuffer);
                    copyStringToMachine(dataBuffer, dataAddr, (unsigned)bytesReceived);
                    machine->WriteRegister(RET_VALUE_REGISTER, bytesReceived);

                } break;

                case SC_NetworkSendFile:
                {
                    int connId = machine->ReadRegister(FIRST_PARAM_REGISTER);
                    int dataAddr = machine->ReadRegister(SECOND_PARAM_REGISTER);
                    int transferSpeedAddress = machine->ReadRegister(THIRD_PARAM_REGISTER);
                    char dataBuffer[NETWORK_MAX_FILE_LENGTH] = { 0 };
                    copyStringFromMachine(dataAddr, dataBuffer, NETWORK_MAX_FILE_LENGTH);
                    int transferSpeed;
                    int retVal = synchPost->SendFile(connId, dataBuffer, &transferSpeed);
                    if (transferSpeedAddress) {
                        machine->WriteMem(transferSpeedAddress, sizeof(int), transferSpeed);
                    }
                    machine->WriteRegister(RET_VALUE_REGISTER, retVal);
                } break;

                case SC_NetworkReceiveFile:
                {
                    int connId = machine->ReadRegister(FIRST_PARAM_REGISTER);
                    int dataAddr = machine->ReadRegister(SECOND_PARAM_REGISTER);
                    char dataBuffer[NETWORK_MAX_FILE_LENGTH] = { 0 };
                    copyStringFromMachine(dataAddr, dataBuffer, NETWORK_MAX_FILE_LENGTH);
                    int retVal = synchPost->ReceiveFile(connId, dataBuffer);
                    machine->WriteRegister(RET_VALUE_REGISTER, retVal);
                } break;

                default:
                {
                    printf ("Unexpected SYSCALL %d %d\n", which, type);
                    ASSERT (FALSE);
                } break;
            }
        } break;

        case BusErrorException:         /* NO BREAK - FALL THROUGH */
        case ReadOnlyException:         /* NO BREAK - FALL THROUGH */
        case PageFaultException:        /* NO BREAK - FALL THROUGH */
        case AddressErrorException:     /* NO BREAK - FALL THROUGH */
        case IllegalInstrException:     /* NO BREAK - FALL THROUGH */
        {
            // Terminate the process
            do_ExitCurrentProcess();
        } break;

        default:
        {
            printf ("Unexpected user mode exception %d %d\n", which, type);
            ASSERT (FALSE);
        }
    }

    // LB: Do not forget to increment the pc before returning!
    UpdatePC ();
    // End of addition
}
