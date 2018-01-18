// system.cc 
//      Nachos initialization and cleanup routines.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "userthread.h"
#include "openfiletable.h"

// This defines *all* of the global data structures used by Nachos.
// These are all initialized and de-allocated by this file.

Thread *currentThread;		// the thread we are running now
Thread *threadToBeDestroyed;	// the thread that just finished
Scheduler *scheduler;		// the ready list
Interrupt *interrupt;		// interrupt status
Statistics *stats;		// performance metrics
Timer *timer;			// the hardware timer device,
					// for invoking context switches

SynchConsole *syncConsole;
SemaphoreManager *semaphoreManager;
OpenFileTable *openFileTable;

#ifdef FILESYS_NEEDED
FileSystem *fileSystem;
#endif

#ifdef FILESYS
SynchDisk *synchDisk;
#endif

#ifdef USER_PROGRAM		// requires either FILESYS or FILESYS_STUB
Machine *machine;		// user program memory and registers
#endif

#ifdef NETWORK
SynchPost *synchPost;
#endif


// External definition, to allow us to take a pointer to this function
extern void Cleanup ();


//----------------------------------------------------------------------
// TimerInterruptHandler
//      Interrupt handler for the timer device.  The timer device is
//      set up to interrupt the CPU periodically (once every TimerTicks).
//      This routine is called each time there is a timer interrupt,
//      with interrupts disabled.
//
//      Note that instead of calling Yield() directly (which would
//      suspend the interrupt handler, not the interrupted thread
//      which is what we wanted to context switch), we set a flag
//      so that once the interrupt handler is done, it will appear as 
//      if the interrupted thread called Yield at the point it is 
//      was interrupted.
//
//      "dummy" is because every interrupt handler takes one argument,
//              whether it needs it or not.
//----------------------------------------------------------------------
static void TimerInterruptHandler (int dummy)
{
    (void)dummy;

    if (interrupt->getStatus () != IdleMode) {
        interrupt->YieldOnReturn ();
    }
}

//----------------------------------------------------------------------
// Initialize
//      Initialize Nachos global data structures.  Interpret command
//      line arguments in order to determine flags for the initialization.  
// 
//      "argc" is the number of command line arguments (including the name
//              of the command) -- ex: "nachos -d +" -> argc = 3 
//      "argv" is an array of strings, one for each command line argument
//              ex: "nachos -d +" -> argv = {"nachos", "-d", "+"}
//----------------------------------------------------------------------
void
Initialize (int argc, char **argv)
{
    int argCount;
    const char *debugArgs = "";
    bool randomYield = FALSE;

#ifdef USER_PROGRAM
    bool debugUserProg = FALSE;	// single step user program
#endif
#ifdef FILESYS_NEEDED
    bool format = FALSE;	// format disk
#endif
#ifdef NETWORK
    double rely = 1;		// network reliability
    int netname = 0;		// UNIX socket name
#endif

    for (argc--, argv++; argc > 0; argc -= argCount, argv += argCount)
      {
	  argCount = 1;
	  if (!strcmp (*argv, "-d"))
	    {
		if (argc == 1)
		    debugArgs = "+";	// turn on all debug flags
		else
		  {
		      debugArgs = *(argv + 1);
		      argCount = 2;
		  }
	    }
	  else if (!strcmp (*argv, "-rs"))
	    {
		ASSERT (argc > 1);
		RandomInit (atoi (*(argv + 1)));	// initialize pseudo-random
		// number generator
		randomYield = TRUE;
		argCount = 2;
	    }
#ifdef USER_PROGRAM
	  if (!strcmp (*argv, "-s"))
	      debugUserProg = TRUE;
#endif
#ifdef FILESYS_NEEDED
	  if (!strcmp (*argv, "-f"))
	      format = TRUE;
#endif
#ifdef NETWORK
	  if (!strcmp (*argv, "-l"))
	    {
		ASSERT (argc > 1);
		rely = atof (*(argv + 1));
		argCount = 2;
	    }
	  else if (!strcmp (*argv, "-m"))
	    {
		ASSERT (argc > 1);
		netname = atoi (*(argv + 1));
		argCount = 2;
	    }
#endif
      }

    DebugInit (debugArgs);	// initialize DEBUG messages
    stats = new Statistics ();	// collect statistics
    interrupt = new Interrupt;	// start up interrupt handling
    scheduler = new Scheduler ();	// initialize the ready queue
    if (randomYield)		// start the timer (if needed)
	timer = new Timer (TimerInterruptHandler, 0, randomYield);

    threadToBeDestroyed = NULL;

    // We didn't explicitly allocate the current thread we are running in.
    // But if it ever tries to give up the CPU, we better have a Thread
    // object to save its state. 
    currentThread = new Thread ("main");
    currentThread->setStatus (RUNNING);

    syncConsole = new SynchConsole(NULL, NULL);
    semaphoreManager = new SemaphoreManager();

    openFileTable = new OpenFileTable();
    
    interrupt->Enable ();
    CallOnUserAbort (Cleanup);	// if user hits ctl-C

#ifdef USER_PROGRAM
    machine = new Machine (debugUserProg);	// this must come first
#endif

#ifdef FILESYS
    synchDisk = new SynchDisk ("DISK");
#endif

#ifdef FILESYS_NEEDED
    fileSystem = new FileSystem (format);
    //fileSystem->CreateDirectory(DEVICE_FILE_FOLDER);
    //fileSystem->CreateUserFile(CONSOLE_FILE_PATH);
    //syncConsole = new SynchConsole((char*)CONSOLE_FILE_PATH, (char*)CONSOLE_FILE_PATH);
#endif

#ifdef NETWORK
    synchPost = new SynchPost(netname, rely, NUM_MAIL_BOXES);
#endif
}

int createProcess(char *filename){

    OpenFile *executable = fileSystem->Open(filename);
    OpenFile *workingDirectoryFile = fileSystem->GetDirectoryFile(filename);

    
    AddrSpace *space;

    if (executable == NULL) {
        printf("Unable to open file %s\n", filename);
        return -1;
    }
    

    space = new AddrSpace(executable);

    if (!currentThread->space) {
        currentThread->space = space;
    }

    int retVal = do_KernelThreadCreate(space,workingDirectoryFile);

    delete executable; // close file

    return retVal;
}

//----------------------------------------------------------------------
// Lists directory content ls
//----------------------------------------------------------------------
void listDirectoryContent(char *name){
    fileSystem->ListDirectoryContent(name);
}

//----------------------------------------------------------------------
// Creates new directory in the current director mkdir
//----------------------------------------------------------------------
int createDirectory(char *name){
    return fileSystem->CreateDirectory(name);
}

//----------------------------------------------------------------------
// Changes directory cd
//----------------------------------------------------------------------
int changeDirectory(char *name){
    return fileSystem->ChangeDirectory(name);
}


//----------------------------------------------------------------------
// Removes directory if it is empty rm
//----------------------------------------------------------------------
int removeDirectory(char *name){
    //return fileSystem->RemoveDirectory(name);
    return fileSystem->Remove(name);
}

//----------------------------------------------------------------------
// Opens a file and returns file descriptior
//----------------------------------------------------------------------
int openFile(char *name){
    
    if(openFileTable->getNumAvailable() == 0){
        return -6;
    }

    OpenFile *file = fileSystem->Open(name);
    if(file == NULL){
        bool result = fileSystem->CreateUserFile(name);
        if(result != 0){
            return result;
        }
    }
    
    file = fileSystem->Open(name);
    
        
    int fileDescriptor = openFileTable->AddEntry(file,name,currentThread->Tid(), currentThread->getName());
    ThreadOpenFileTable *perThreadTable = currentThread->getOpenFileTable();
    int descriptor = perThreadTable->AddEntry(fileDescriptor,NORMAL_FILE);
    
    return descriptor;
}
    
// writes to file
// if there is no file than returns -1
// if it is console returns size
// if write successful, returns number of bytes written to disk
int writeToFile (char *buffer, int size, int fileDescriptor){
    
    ThreadOpenFileTable *perThreadTable = currentThread->getOpenFileTable();

    int systemFileDescriptor = perThreadTable->getFileDescriptor(fileDescriptor);

    if(systemFileDescriptor == -1){
        return -1;
    }
    
    //If it is console than write to console
    if(perThreadTable->getFileType(fileDescriptor) == CONSOLE){
        syncConsole->SynchPutString(buffer,size);
        return size;
    }

    OpenFile *file = openFileTable->getFile(systemFileDescriptor);
    Lock *lock = openFileTable->getLock(systemFileDescriptor);
    lock->Acquire();
    
    
    int result = file->Write(buffer,size);
    
    lock->Release();
    return result;
}

int readFromFile (char *buffer, int size, int fileDescriptor){
    ThreadOpenFileTable *perThreadTable = currentThread->getOpenFileTable();

    int systemFileDescriptor = perThreadTable->getFileDescriptor(fileDescriptor);

    if(systemFileDescriptor == -1){
        //printf("Read No file :( \n");
        return -1;
    }
    
    //If it is console than write to console
    if(perThreadTable->getFileType(fileDescriptor) == CONSOLE){
        syncConsole->SynchGetString(buffer,size);
        return size;
    }
    
    
    OpenFile *file = openFileTable->getFile(systemFileDescriptor);
    Lock *lock = openFileTable->getLock(systemFileDescriptor);
    lock->Acquire();
    
    int readSize = file->Read(buffer,size);
    
    //printf(">>> Read From file : %s\n",buffer);
    
    lock->Release();
    
    return readSize;
}

void closeFile(int fileDescriptor){
    
    ThreadOpenFileTable *perThreadTable = currentThread->getOpenFileTable();

    int systemFileDescriptor = perThreadTable->getFileDescriptor(fileDescriptor);

    if(systemFileDescriptor == -1){
        return;
    }
    
    if(perThreadTable->getFileType(fileDescriptor) == CONSOLE){
        //Do nothing
        return;
    }
    
    openFileTable->RemoveEntry(systemFileDescriptor);
}

//----------------------------------------------------------------------
// Cleanup
//      Nachos is halting.  De-allocate global data structures.
//----------------------------------------------------------------------
void
Cleanup ()
{
    printf ("\nCleaning up...\n");
#ifdef NETWORK
    delete synchPost;
#endif

#ifdef USER_PROGRAM
    delete machine;
#endif

#ifdef FILESYS_NEEDED
    delete fileSystem;
#endif

#ifdef FILESYS
    delete synchDisk;
#endif

    delete semaphoreManager;
    delete syncConsole;
    delete timer;
    delete scheduler;
    delete interrupt;

    Exit (0);
}
