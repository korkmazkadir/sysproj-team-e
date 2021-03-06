/* Start.s 
 *	Assembly language assist for user programs running on top of Nachos.
 *
 *	Since we don't want to pull in the entire C library, we define
 *	what we need for a user program here, namely Start and the system
 *	calls.
 */

#include "syscall.h"

        .text   
        .align  2

/* -------------------------------------------------------------
 * __start
 *	Initialize running a C program, by calling "main". 
 *
 * 	NOTE: This has to be first, so that it gets loaded at location 0.
 *	The Nachos kernel always starts a program by jumping to location 0.
 * -------------------------------------------------------------
 */

	.globl __start
	.ent	__start
__start:
	jal	main
    move	$4,$2
    jal	Exit	 /* exit with the value returned from main */
	.end __start

/* -------------------------------------------------------------
 * System call stubs:
 *	Assembly language assist to make system calls to the Nachos kernel.
 *	There is one stub per system call, that places the code for the
 *	system call into register r2, and leaves the arguments to the
 *	system call alone (in other words, arg1 is in r4, arg2 is 
 *	in r5, arg3 is in r6, arg4 is in r7)
 *
 * 	The return value is in r2. This follows the standard C calling
 * 	convention on the MIPS.
 * -------------------------------------------------------------
 */

	.globl Halt
	.ent	Halt
Halt:
	addiu $2,$0,SC_Halt
	syscall
	j	$31
	.end Halt

	.globl Exit
	.ent	Exit
Exit:
	addiu $2,$0,SC_Exit
	syscall
	j	$31
	.end Exit

	.globl Exec
	.ent	Exec
Exec:
	addiu $2,$0,SC_Exec
	syscall
	j	$31
	.end Exec

	.globl Join
	.ent	Join
Join:
	addiu $2,$0,SC_Join
	syscall
	j	$31
	.end Join

	.globl Create
	.ent	Create
Create:
	addiu $2,$0,SC_Create
	syscall
	j	$31
	.end Create

	.globl Open
	.ent	Open
Open:
	addiu $2,$0,SC_Open
	syscall
	j	$31
	.end Open

	.globl Read
	.ent	Read
Read:
	addiu $2,$0,SC_Read
	syscall
	j	$31
	.end Read

	.globl Write
	.ent	Write
Write:
	addiu $2,$0,SC_Write
	syscall
	j	$31
	.end Write

	.globl Close
	.ent	Close
Close:
	addiu $2,$0,SC_Close
	syscall
	j	$31
	.end Close

	.globl Fork
	.ent	Fork
Fork:
	addiu $2,$0,SC_Fork
	syscall
	j	$31
	.end Fork

	.globl Yield
	.ent	Yield
Yield:
	addiu $2,$0,SC_Yield
	syscall
	j	$31
	.end Yield

    .globl PutChar
    .ent PutChar
PutChar:
    addiu $2,$0,SC_PutChar
    syscall
    j   $31
    .end PutChar

    .globl SynchPutString
    .ent SynchPutString
SynchPutString:
    addiu $2,$0,SC_SynchPutString
    syscall
    j   $31
    .end SynchPutString

    .globl SynchGetChar
    .ent SynchGetChar
SynchGetChar:
    addiu $2,$0,SC_SynchGetChar
    syscall
    j   $31
    .end SynchGetChar

    .globl SynchGetString
    .ent SynchGetString
SynchGetString:
    addiu $2,$0,SC_SynchGetString
    syscall
    j   $31
    .end SynchGetString

    .globl SynchPutInt
    .ent SynchPutInt
SynchPutInt:
    addiu $2,$0,SC_SynchPutInt
    syscall
    j   $31
    .end SynchPutInt

    .globl SynchGetInt
    .ent SynchGetInt
SynchGetInt:
    addiu $2,$0,SC_SynchGetInt
    syscall
    j   $31
    .end SynchGetInt

    .globl AssertionFailed
    .ent    AssertionFailed
AssertionFailed:
    addiu $2,$0,SC_AssertionFailed
    syscall
    j	$31
    .end AssertionFailed

    .global UserThreadCreate
    .ent UserThreadCreate
UserThreadCreate:
    addiu $2,$0,SC_UserThreadCreate
    addiu $8,$0,UserThreadExit
    syscall
    j   $31
    .end UserThreadCreate

    .globl UserThreadExit
    .ent UserThreadExit
UserThreadExit:
    addiu $2,$0,SC_UserThreadExit
    syscall
    j   $31
    .end UserThreadExit

    .globl UserThreadJoin
    .ent UserThreadJoin
UserThreadJoin:
    addiu $2,$0,SC_UserThreadJoin
    syscall
    j   $31
    .end UserThreadJoin

    .globl SemInit
    .ent SemInit
SemInit:
    addiu $2,$0,SC_SemInit
    syscall
    j   $31
    .end SemInit

    .globl SemPost
    .ent SemPost
SemPost:
    addiu $2,$0,SC_SemPost
    syscall
    j   $31
    .end SemPost

    .globl SemWait
    .ent SemWait
SemWait:
    addiu $2,$0,SC_SemWait
    syscall
    j   $31
    .end SemWait

    .globl SemDestroy
    .ent SemDestroy
SemDestroy:
    addiu $2,$0,SC_SemDestroy
    syscall
    j   $31
    .end SemDestroy

    .globl UserThreadSelfId
    .ent UserThreadSelfId
UserThreadSelfId:
    addiu $2,$0,SC_UserThreadSelfId
    syscall
    j   $31
    .end UserThreadSelfId

    .globl ForkExec
    .ent    ForkExec
ForkExec :
    addiu $2,$0,SC_ForkExec
    syscall
    j	$31
    .end ForkExec

    .globl NetworkConnectAsServer
    .ent NetworkConnectAsServer
NetworkConnectAsServer:
    addiu $2,$0,SC_NetworkConnectAsServer
    syscall
    j   $31
    .end NetworkConnectAsServer

    .globl NetworkConnectAsClient
    .ent NetworkConnectAsClient
NetworkConnectAsClient:
    addiu $2,$0,SC_NetworkConnectAsClient
    syscall
    j   $31
    .end NetworkConnectAsClient

    .globl NetworkSendToByConnId
    .ent NetworkSendToByConnId
NetworkSendToByConnId:
    addiu $2,$0,SC_NetworkSendToByConnId
    syscall
    j   $31
    .end NetworkSendToByConnId

    .globl NetworkReceiveFromByConnId
    .ent NetworkReceiveFromByConnId
NetworkReceiveFromByConnId:
    addiu $2,$0,SC_NetworkReceiveFromByConnId
    syscall
    j   $31
    .end NetworkReceiveFromByConnId

    .globl SendFile
    .ent SendFile
SendFile:
    addiu $2,$0,SC_NetworkSendFile
    syscall
    j   $31
    .end SendFile

    .globl ReceiveFile
    .ent ReceiveFile
ReceiveFile:
    addiu $2,$0,SC_NetworkReceiveFile
    syscall
    j   $31
    .end ReceiveFile

    .globl NetworkCloseConnection
    .ent NetworkCloseConnection
NetworkCloseConnection:
    addiu $2,$0,SC_NetworkCloseConnection
    syscall
    j   $31
    .end NetworkCloseConnection

    .globl ListDirectoryContent
    .ent   ListDirectoryContent
ListDirectoryContent:
    addiu $2,$0,SC_ListDirectoryContent
    syscall
    j	$31
    .end ListDirectoryContent


    .globl CreateDirectory
    .ent   CreateDirectory
CreateDirectory:
    addiu $2,$0,SC_CreateDirectory
    syscall
    j	$31
    .end CreateDirectory

    .globl ChangeDirectory
    .ent   ChangeDirectory
ChangeDirectory:
    addiu $2,$0,SC_ChangeDirectory
    syscall
    j	$31
    .end ChangeDirectory


    .globl RemoveDirectory
    .ent   RemoveDirectory
RemoveDirectory:
    addiu $2,$0,SC_RemoveDirectory
    syscall
    j	$31
    .end RemoveDirectory


/* dummy function to keep gcc happy */
        .globl  __main
        .ent    __main
__main:
        j       $31
        .end    __main

