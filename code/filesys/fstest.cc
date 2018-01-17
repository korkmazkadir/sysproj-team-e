// fstest.cc 
//	Simple test routines for the file system.  
//
//	We implement:
//	   Copy -- copy a file from UNIX to Nachos
//	   Print -- cat the contents of a Nachos file 
//	   Perftest -- a stress test for the Nachos file system
//		read and write a really large file in tiny chunks
//		(won't work on baseline system!)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#include "utility.h"
#include "filesys.h"
#include "system.h"
#include "thread.h"
#include "disk.h"
#include "stats.h"

#define TransferSize 	10 	// make it small, just to be difficult

//----------------------------------------------------------------------
// Copy
// 	Copy the contents of the UNIX file "from" to the Nachos file "to"
//----------------------------------------------------------------------

void
Copy(const char *from, const char *to)
{
    //printf("\n*  *   *    *    * START COPY *  *   *   *   *\n\n\n");
    FILE *fp;
    int amountRead, amountWritten, fileLength;
    char *buffer;

    // Open UNIX file
    if ((fp = fopen(from, "r")) == NULL) {	 
        printf("Copy: couldn't open input file %s\n", from);
        return;
    }

    // Figure out length of UNIX file
    fseek(fp, 0, 2);		
    fileLength = ftell(fp);
    fseek(fp, 0, 0);

    // Create a Nachos file of the same length
    DEBUG('f', "Copying file %s, size %d, to file %s\n", from, fileLength, to);
    fileSystem->fsLock->Acquire();
    if (-1 == fileSystem->Create(to, fileLength, 0)) {	 // Create Nachos file
        printf("Copy: couldn't create output file %s\n", to);
        fclose(fp);
        fileSystem->fsLock->Release();
        return;
    }
    int index = fileSystem->Open(to);
    fileSystem->fsLock->Release();
    ASSERT(index != -1);
    
    

    // Copy the data in TransferSize chunks
    buffer = new char[TransferSize];
    while ((amountRead = fread(buffer, sizeof(char), TransferSize, fp)) > 0) {
        fileSystem->fsLock->Acquire();
        amountWritten = fileSystem->Write(buffer, amountRead, index);	
        fileSystem->fsLock->Release();
        if (amountWritten == -1) {
            printf("write to copy of file failed\n");
            Exit(1);
        }
    }
    delete [] buffer;
    //printf("\n*  *   *    *    * out of loop *  *   *   *   *\n\n\n");

    // Close the UNIX and the Nachos files
    fileSystem->fsLock->Acquire();
    fileSystem->Close(index);
    fileSystem->fsLock->Release();
    fclose(fp);
    printf("\n*  *   *    *    * END COPY *  *   *   *   *\n\n\n");

}

//----------------------------------------------------------------------
// Print
// 	Print the contents of the Nachos file "name".
//----------------------------------------------------------------------

void
Print(char *name)
{
    int index;    
    int i, amountRead;
    char *buffer;

    if ((index = fileSystem->Open(name)) == -1) {
        printf("Print: unable to open file %s\n", name);
        return;
    }
    
    buffer = new char[TransferSize];
    while ((amountRead = fileSystem->Read(buffer, TransferSize, index)) > 0)
        for (i = 0; i < amountRead; i++)
            printf("%c", buffer[i]);
    delete [] buffer;

    fileSystem->Close(index);
    return;
}

//----------------------------------------------------------------------
// PerformanceTest
// 	Stress the Nachos file system by creating a large file, writing
//	it out a bit at a time, reading it back a bit at a time, and then
//	deleting the file.
//
//	Implemented as three separate routines:
//	  FileWrite -- write the file
//	  FileRead -- read the file
//	  PerformanceTest -- overall control, and print out performance #'s
//----------------------------------------------------------------------

#define FileName 	"TestFile"
#define Contents 	"1234567890"
#define ContentSize 	strlen(Contents)
#define FileSize 	((int)(ContentSize * 5000))

static void 
FileWrite()
{
}
/*
    int i, numBytes, index;

    printf("Sequential write of %d byte file, in %zd byte chunks\n", 
	FileSize, ContentSize);
    if (!fileSystem->Create(FileName, 0, 0)) {
      printf("Perf test: can't create %s\n", FileName);
      return;
    }
    index = fileSystem->Open(FileName);
    if (index == -1) {
        printf("Perf test: unable to open %s\n", FileName);
        return;
    }
    for (i = 0; i < FileSize; i += ContentSize) {
        numBytes = fileSystem->Write(Contents, ContentSize, index);
        if (numBytes < 10) {
            printf("Perf test: unable to write %s\n", FileName);
            fileSystem->Close(index);
            return;
        }
    }
    fileSystem->Close(index);
}*/

static void 
FileRead()
{
    char *buffer = new char[ContentSize];
    int i, numBytes, index;

    printf("Sequential read of %d byte file, in %zd byte chunks\n", 
	FileSize, ContentSize);

    if ((index = fileSystem->Open(FileName)) == -1) {
        printf("Perf test: unable to open file %s\n", FileName);
        delete [] buffer;
        return;
    }
    for (i = 0; i < FileSize; i += ContentSize) {
        numBytes = fileSystem->Read(buffer, ContentSize, index);
        if ((numBytes < 10) || strncmp(buffer, Contents, ContentSize)) {
            printf("Perf test: unable to read %s\n", FileName);
            fileSystem->Close(index);
            delete [] buffer;
            return;
        }
    }
    delete [] buffer;
    fileSystem->Close(index);
}

void
PerformanceTest()
{
    printf("Starting file system performance test:\n");
    stats->Print();
    FileWrite();
    FileRead();
    if (!fileSystem->Remove(FileName)) {
      printf("Perf test: unable to remove %s\n", FileName);
      return;
    }
    stats->Print();
}

//----------------------------------------------------------------

void 
mkdirTest(const char* name) {
    //make a new directory
    if (!fileSystem->Mkdir(name)) {
        printf("-- MKDIR FAILED --\n");
    }
    fileSystem->Print();
}
