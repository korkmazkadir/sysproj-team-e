// filesys.h 
//	Data structures to represent the Nachos file system.
//
//	A file system is a set of files stored on disk, organized
//	into directories.  Operations on the file system have to
//	do with "naming" -- creating, opening, and deleting files,
//	given a textual file name.  Operations on an individual
//	"open" file (read, write, close) are to be found in the OpenFile
//	class (openfile.h).
//
//	We define two separate implementations of the file system. 
//	The "STUB" version just re-defines the Nachos file system 
//	operations as operations on the native UNIX file system on the machine
//	running the Nachos simulation.  This is provided in case the
//	multiprogramming and virtual memory assignments (which make use
//	of the file system) are done before the file system assignment.
//
//	The other version is a "real" file system, built on top of 
//	a disk simulator.  The disk is simulated using the native UNIX 
//	file system (in a file named "DISK"). 
//
//	In the "real" implementation, there are two key data structures used 
//	in the file system.  There is a single "root" directory, listing
//	all of the files in the file system; unlike UNIX, the baseline
//	system does not provide a hierarchical directory structure.  
//	In addition, there is a bitmap for allocating
//	disk sectors.  Both the root directory and the bitmap are themselves
//	stored as files in the Nachos file system -- this causes an interesting
//	bootstrap problem when the simulated disk is initialized. 
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef FS_H
#define FS_H

#include "copyright.h"
#include "openfile.h"
#include <string>

#ifdef FILESYS_STUB 		// Temporarily implement file system calls as 
				// calls to UNIX, until the real file system
				// implementation is available
class FileSystem {
  public:
    FileSystem(bool format) {}
    ~FileSystem();

    bool Create(const char *name, int initialSize) { 
	int fileDescriptor = OpenForWrite(name);

	if (fileDescriptor == -1) return FALSE;
	Close(fileDescriptor); 
	return TRUE; 
	}

    OpenFile* Open(char *name) {
	  int fileDescriptor = OpenForReadWrite(name, FALSE);

	  if (fileDescriptor == -1) return NULL;
	  return new OpenFile(fileDescriptor);
      }

    bool Remove(char *name) { return Unlink(name) == 0; }

};

#else // FILESYS


class Semaphore;
class Lock;

typedef struct S_fileInfo {
    OpenFile *file;
    Semaphore *sem;
    int nbOpens;
} FileInfo;

class FileSystem {
  public:
    FileSystem(bool format, std::string *initialWP, std::string *initialWDN);		                // Initialize the file system.
                                                    // Must be called *after* "synchDisk" 
                                                    // has been initialized.
                                                    // If "format", there is nothing on
                                                    // the disk, so initialize the directory
                                                    // and the bitmap of free blocks.
    ~FileSystem();
    
    int Create(std::string fileName, int initialSize, bool isDir); // Create a file (UNIX creat)
                                                    

    OpenFile* Open(std::string fileName); 	         // Open a file (UNIX open)
    
    int Close(OpenFile *ofid);
    int Remove(std::string fileName);  	         // Delete a file (UNIX unlink)

    void List();			                         // List all the files in the file system

    void Print();	                                 // List all the files and their contents
    int Mkdir(const char *dirName);
    int Rmdir(const char *dirName);
    std::string Chdir(std::string path);
    const char* GetWorkingPath();
    const char* GetWorkingDir();
    void saveThreadState();                          //handles context switch
    void restoreThreadState();
                                                   


  private:
  
    void dbgChecks(); //helper

    OpenFile* freeMapFile;		                    // Bit map of free disk blocks,
                                                    // represented as a file
    OpenFile* directoryFile;		                // current directory -- list of 
                                                     // file names, represented as a file
    //OpenFile **openFiles;                           //table of currently open files
    FileInfo **openFiles;
    int *threadOpenFiles;                           //points to table in thread class containing 
                                                    //indices referring to (global) openFiles table
    std::string *workingPath;
    std::string *workingDirName;                       //name of current directory (starts as root)
    Lock *fsLock;
};

/* ideas:
 * Global table for whole OS of 10 OpenFile ptrs.
 * Per thread table of correspondances between OpenFileIds 
 * (file descriptors, different from OpenFile ptrs) and positions in global table.
 * on Open: if already open, reference the right place in global table
 *          if not open yet, if can make new entry, make it.
 * on close: remove entry from local table, decrement counter on global table
 *          if counter = 0 remove from global table
 * on remove: if is open, set toBeDestroyed flag to stop other Open() calls
 *             wait until counter = 0, then delete
 */
 /*
  * model:
  * thread cannot open or remove file that is already open.
  * Objective: multiple threads can have same file open
  * conseq: need to give a different openFile  object, but referencing the same file on disk.
  * problems: things like mkdir need to open the directory files. Probably disallow concurrent running of that?
  * what happens on remove of a shared file? disallow further opens, do remove once all current open instances have been closed...
  * read/write will work with a rw lock per file
  * 
  * Context switching: load per thread table into filesys per thread table.
  * per thread table just tells you where to look in global table. that way you can update and see what other threads have done (numopens)
  *
  */
  /* what can get changed? Directory can be changed -> other threads need to know
   *        they will: system openFile table.
   * things like mkdir rmdir need to be run atomically otherwise incoherent.
   */

#endif // FILESYS

#endif // FS_H
