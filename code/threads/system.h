// system.h 
//      All global variables used in Nachos are defined here.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdint.h>
#include <set>
#include <functional>

#include "copyright.h"
#include "utility.h"
#include "thread.h"
#include "scheduler.h"
#include "interrupt.h"
#include "stats.h"
#include "timer.h"
#include "synchconsole.h"
#include "usersemaphore.h"
#include "SynchPost.h"
#include "openfiletable.h"


#define MAX_WRITE_BUF_SIZE 128
#define MAX_INT_LEN 11

// max number of mail boxes for network communication
// this number must be even
#define NUM_MAIL_BOXES 128

// Initialization and cleanup routines
extern void Initialize (int argc, char **argv);	// Initialization,
						// called before anything else
extern void Cleanup ();		// Cleanup, called when
						// Nachos is done.

extern Thread *currentThread;	// the thread holding the CPU
extern Thread *threadToBeDestroyed;	// the thread that just finished
extern Scheduler *scheduler;	// the ready list
extern Interrupt *interrupt;	// interrupt status
extern Statistics *stats;	// performance metrics
extern Timer *timer;		// the hardware alarm clock
extern SynchConsole *syncConsole;
extern SemaphoreManager *semaphoreManager;
extern OpenFileTable *openFileTable;

int createProcess(char *filename);

void listDirectoryContent(char *name); // list directory (ls)
int createDirectory(char *name); // change directory (cd)
int changeDirectory(char *name); // create directory (mkdir)
int removeDirectory(char *name); // remove directory (rm)


int openFile(char *name);
int writeToFile (char *buffer, int size, int fileDescriptor);
int readFromFile (char *buffer, int size, int fileDescriptor);
void closeFile(int fileDescriptor);


#ifdef USER_PROGRAM
#include "machine.h"
extern Machine *machine;	// user program memory and registers
#endif

#ifdef FILESYS_NEEDED		// FILESYS or FILESYS_STUB

#define DEVICE_FILE_FOLDER "./dev"
#define CONSOLE_FILE_PATH "./dev/console"

#include "filesys.h"
extern FileSystem *fileSystem;
extern OpenFile *consoleDeviceFile;
#endif

#ifdef FILESYS
#include "synchdisk.h"
extern SynchDisk *synchDisk;
#endif

#ifdef NETWORK
#include "post.h"
extern SynchPost *synchPost;
#endif

#endif // SYSTEM_H
