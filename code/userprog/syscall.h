/* syscalls.h 
 * 	Nachos system call interface.  These are Nachos kernel operations
 * 	that can be invoked from user programs, by trapping to the kernel
 *	via the "syscall" instruction.
 *
 *	This file is included by user programs and by the Nachos kernel. 
 *
 * Copyright (c) 1992-1993 The Regents of the University of California.
 * All rights reserved.  See copyright.h for copyright notice and limitation 
 * of liability and disclaimer of warranty provisions.
 */

#ifndef SYSCALLS_H
#define SYSCALLS_H

#include "copyright.h"
#include "semaphore.h"

/* system call codes -- used by the stubs to tell the kernel which system call
 * is being asked for
 */
#define SC_Halt		0
#define SC_Exit		1
#define SC_Exec		2
#define SC_Join		3
#define SC_Create	4
#define SC_Open		5
#define SC_Read		6
#define SC_Write	7
#define SC_Close	8
#define SC_Fork		9
#define SC_Yield	10
#define SC_PutChar  11
#define SC_SynchPutString 12
#define SC_SynchGetChar 13
#define SC_SynchGetString 14
#define SC_SynchPutInt 15
#define SC_SynchGetInt 16
#define SC_UserThreadCreate 17
#define SC_UserThreadExit 18
#define SC_UserThreadJoin 19
#define SC_SemInit 20
#define SC_SemPost 21
#define SC_SemWait 22
#define SC_SemDestroy 23
#define SC_UserThreadSelfId 24
#define SC_ForkExec 25

// ================= NETWORK ====================

#define SC_NetworkConnectAsServer 28
#define SC_NetworkConnectAsClient 29
#define SC_NetworkSendToByConnId 30
#define SC_NetworkReceiveFromByConnId 31
#define SC_NetworkSendFile 32
#define SC_NetworkReceiveFile 33
#define SC_NetworkCloseConnection 34

// ================= FILE SYSTEM ===============

#define SC_ListDirectoryContent 50
#define SC_CreateDirectory 51
#define SC_RemoveDirectory 52
#define SC_ChangeDirectory 53


#define SC_AssertionFailed     100


#ifdef IN_USER_MODE

// LB: This part is read only on compiling the test/*.c files.
// It is *not* read on compiling test/start.S


/* The system call interface.  These are the operations the Nachos
 * kernel needs to support, to be able to run user programs.
 *
 * Each of these is invoked by a user program by simply calling the 
 * procedure; an assembly language stub stuffs the system call code
 * into a register, and traps to the kernel.  The kernel procedures
 * are then invoked in the Nachos kernel, after appropriate error checking, 
 * from the system call entry point in exception.cc.
 */

/* Stop Nachos, and print out performance stats */
void Halt () __attribute__((noreturn));

/* Input/Output operations: PutChar SycnhPutString SynchGetChar SynchGetSrting SynchPutInt SynchGetInt */


void PutChar(char c);
void SynchPutString(const char *string);
char SynchGetChar();
void SynchGetString(char *s, int n);
void SynchPutInt(int n);
void SynchGetInt(int *n);
int UserThreadCreate(void f(void *arg), void *arg);
void UserThreadExit();
int UserThreadJoin(int tid);
int UserThreadSelfId();

int SemInit(sem_t *semPtr, int val);
int SemPost(sem_t *semPtr);
int SemWait(sem_t *semPtr);
int SemDestroy(sem_t *semPtr);

/*!
 * Awaits for incoming connection to the specified mailbox
 * As soon as there is one sends an anknowledgement and returns a connection
 * identifier
 *
 * This is guaranteed that connection identifier is unique for the process but
 * different processes can have different connection identifiers for the same connection
 *
 * \param      mailbox  < mailbox to wait on
 * \return     connection identifier which is greater or equal to 0 if connection was successful
 *             a value less than zero if connection failed
 */
int NetworkConnectAsServer(int mailbox);

/*!
 * Attempts to connect to a server at specified address using a specified mailbox
 * If there is a server with the respective address listening on a respective mailbox
 * the function will return a connection identifier. If no server responds within
 * machine-dependent timeframe, the function will return.
 *
 * \param      address <    network address of the server
 *             mailbox <    mailbox for the connection
 *
 * \return     connection identifier which is greater or equal to 0 if connection was successful
 *             a value less than zero if connection failed
 */
int NetworkConnectAsClient(int address, int mailbox);

/*!
 * Attempts to send a byte array of a specified network using the specified connection identifier
 * Connection ID must match the value returned by one of the NetworkConnect functions
 *
 * A thread that calls this function MUST match the thread that acquired the connection identifier
 *
 * This function can only send byte arrays whose size is less or equal than 2047 bytes
 * The byte array MUST contain at least the number of bytes specified by len parameter.
 * If this is not the case this function call results in an UNDEFINED BEHAVIOUR
 *
 * If len is greater than 2047, only first 2047 bytes will be transferred over the network
 *
 * \param       connId <    connection identifier
 * \param       data   <    pointer to the byte array
 * \param       len    <    number of bytes that shall be sent over the network
 *
 * \return      -10    <    if invalid connection identifier is specified
 *              -20    <    if the calling thread does not match the thread that established the
 *                          connection
 *              -1     <    if receipt was not properly acknowledged by the recepient of the data
 *                          (this usually means that not all data has been transferred)
 *               0     <    if data transfer was successful
 */
int NetworkSendToByConnId(int connId, const char *data, int len);

/*!
 * Receives the data over the network connection described by the specified connection identifier
 * and saves it to the specified byte array
 *
 * A thread that calls this function MUST match the thread that acquired the connection identifier
 *
 * It is guaranteed that the function will not receive more than 2047 bytes of data.
 * It is responsibility of a user to ensure that the data fits the specified byte array.
 * If this is not the case, the call to this function causes UNDEFINED BEHAVIOUR
 *
 * \param     connId   <    connection identifier
 * \param     data     <    a byte array that will be used for storing the received data
 *
 * \return      -10    <    if invalid connection identifier is specified
 *              -20    <    if the calling thread does not match the thread that established the
 *                          connection
 *              -30    <    if the connection was closed by the other side
 *  number of bytes    <    if the receipt was successful
 *         received
 */
int NetworkReceiveFromByConnId(int connId, char *data);

/*!
 * Sends a file specified by \a fileName over the network described by the connection identifier
 * Users may provide a pointer to the ingeger where transfer speed will be saved
 *
 * A thread that calls this function MUST match the thread that acquired the connection identifier
 *
 * \param      connId  <    connection identifier
 *            fileName <    relative path to the file that need to be transferred
 *       transferSpeed <    pointer to the integer where transfer speed will be saved
 *                          if this pointer is NULL transfer speed will not be saved
 *
 * \return       0     <    if file transfer was successful
 *              -1     <    if the specified file cannot be opened
 *              -2     <    if file transfer over the network failed
 */
int SendFile(int connId, const char *fileName, int *transferSpeed);

/*!
 * Receives a file over the network connection described by the connection identifier
 * and saves it as \a fileName
 *
 * A thread that calls this function MUST match the thread that acquired the connection identifier
 *
 * If the file with the given fileName already exists in the filesystem its content will be overwritten
 *
 * A thread that calls this function MUST match the thread that acquired the connection identifier
 *
 * \param   connId     <    connection identifier
 *        fileName     <    relative path to the file
 *
 * \return       0     <    if the file has been received and saved successfully
 *              -1     <    if the file cannot be created
 *              -2     <    if the error occured during network transfer
 *              -3     <    if the error occured when writing the file
 */
int ReceiveFile(int connId, const char *fileName);

/*!
 * Closes a network connection described by the connection identifier
 * Connection identifier is considered invalid as soon as this function is called
 * Attempt to use the connection identifier after call to this function results in
 * an UNDEFINED BEHAVIOUR
 *
 * If other side of the connection attempts to receive anything after this function
 * is called, it will close its connection immediately
 *
 * If other sides does not attempt to receive anything, it will close its connection
 * after machine-dependent period of time
 *
 * A thread that calls this function MUST match the thread that acquired the connection identifier
 *
 * \param   connId      < Connection Identifier
 *
 * \return   0          < if connection has been closed
 *          -1          < if invalid connection identifier is specified
 */
int NetworkCloseConnection(int connId);

/* Address space control operations: Exit, Exec, and Join */

/* This user program is done (status = 0 means exited normally). */
void Exit (int status) __attribute__((noreturn));

/* A unique identifier for an executing user program (address space) */
typedef int SpaceId;

/* Run the executable, stored in the Nachos file "name", and return the 
 * address space identifier
 */
SpaceId Exec (char *name);

/* Only return once the the user program "id" has finished.  
 * Return the exit status.
 */
int Join (SpaceId id);


/* File system operations: Create, Open, Read, Write, Close
 * These functions are patterned after UNIX -- files represent
 * both files *and* hardware I/O devices.
 *
 * If this assignment is done before doing the file system assignment,
 * note that the Nachos file system has a stub implementation, which
 * will work for the purposes of testing out these routines.
 */

/* A unique identifier for an open Nachos file. */
typedef int OpenFileId;

/* when an address space starts up, it has two open files, representing 
 * keyboard input and display output (in UNIX terms, stdin and stdout).
 * Read and Write can be used directly on these, without first opening
 * the console device.
 */

#define ConsoleInput	0
#define ConsoleOutput	1

/* Create a Nachos file, with "name" */
void Create (char *name);

/* Open the Nachos file "name", and return an "OpenFileId" that can 
 * be used to read and write to the file.
 */
OpenFileId Open (char *name);

/* Write "size" bytes from "buffer" to the open file. */
int Write (char *buffer, int size, OpenFileId id);

/* Read "size" bytes from the open file into "buffer".  
 * Return the number of bytes actually read -- if the open file isn't
 * long enough, or if it is an I/O device, and there aren't enough 
 * characters to read, return whatever is available (for I/O devices, 
 * you should always wait until you can return at least one character).
 */
int Read (char *buffer, int size, OpenFileId id);

/* Close the file, we're done reading and writing to it. */
void Close (OpenFileId id);



/* User-level thread operations: Fork and Yield.  To allow multiple
 * threads to run within a user program. 
 */

/* Fork a thread to run a procedure ("func") in the *same* address space 
 * as the current thread.
 */
void Fork (void (*func) ());

/* Yield the CPU to another runnable thread, whether in this address space 
 * or not. 
 */
void Yield ();

/* Assertion failure
 * 
 */
void AssertionFailed(char *fileName, int lineNumber);


/* Fork Exec syscal
 * 
 */
int ForkExec(char *fileName);


/* Lists content of the current directory
 * 
 */
void ListDirectoryContent(char *directoryName);

/* Lists content of the current directory
 * 
 */
int CreateDirectory(char *directoryName);

/* Changes directory
 * 
 */
int ChangeDirectory(char *directoryName);

/* Removes directory
 * 
 */
int RemoveDirectory(char *directoryName);


#endif // IN_USER_MODE

#endif /* SYSCALL_H */
