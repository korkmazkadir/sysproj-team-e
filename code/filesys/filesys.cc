// filesys.cc 
//	Routines to manage the overall operation of the file system.
//	Implements routines to map from textual file names to files.
//
//	Each file in the file system has:
//	   A file header, stored in a sector on disk 
//		(the size of the file header data structure is arranged
//		to be precisely the size of 1 disk sector)
//	   A number of data blocks
//	   An entry in the file system directory
//
// 	The file system consists of several data structures:
//	   A bitmap of free disk sectors (cf. bitmap.h)
//	   A directory of file names and file headers
//
//      Both the bitmap and the directory are represented as normal
//	files.  Their file headers are located in specific sectors
//	(sector 0 and sector 1), so that the file system can find them 
//	on bootup.
//
//	The file system assumes that the bitmap and directory files are
//	kept "open" continuously while Nachos is running.
//
//	For those operations (such as Create, Remove) that modify the
//	directory and/or bitmap, if the operation succeeds, the changes
//	are written immediately back to disk (the two files are kept
//	open during all this time).  If the operation fails, and we have
//	modified part of the directory and/or bitmap, we simply discard
//	the changed version, without writing it back to disk.
//
// 	Our implementation at this point has the following restrictions:
//
//	   there is no synchronization for concurrent accesses
//	   files have a fixed size, set when the file is created
//	   files cannot be bigger than about 3KB in size
//	   there is no hierarchical directory structure, and only a limited
//	     number of files can be added to the system
//	   there is no attempt to make the system robust to failures
//	    (if Nachos exits in the middle of an operation that modifies
//	    the file system, it may corrupt the disk)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#include "disk.h"
#include "bitmap.h"
#include "directory.h"
#include "filehdr.h"
#include "filesys.h"
#include "synch.h"
#include "system.h"


// Sectors containing the file headers for the bitmap of free sectors,
// and the directory of files.  These file headers are placed in well-known 
// sectors, so that they can be located on boot-up.
#define FreeMapSector 		0
#define DirectorySector 	1

// Initial file sizes for the bitmap and directory; until the file system
// supports extensible files, the directory size sets the maximum number 
// of files that can be loaded onto the disk.
#define FreeMapFileSize 	(NumSectors / BitsInByte)
#define NumDirEntries 		10
#define DirectoryFileSize 	(sizeof(DirectoryEntry) * NumDirEntries)



void
FileSystem::dbgChecks() {
    if (directoryFile == -1) {
        printf("FS dbgchecks -1 directoryFile = %d\n",  directoryFile);
        Exit(-1);
    }
}
//----------------------------------------------------------------------
// FileSystem::FileSystem
// 	Initialize the file system.  If format = TRUE, the disk has
//	nothing on it, and we need to initialize the disk to contain
//	an empty directory, and a bitmap of free sectors (with almost but
//	not all of the sectors marked as free).  
//
//	If format = FALSE, we just have to open the files
//	representing the bitmap and the directory.
//
//	"format" -- should we initialize the disk?
//----------------------------------------------------------------------

FileSystem::FileSystem(bool format)
{ 
    DEBUG('f', "Initializing the file system.\n");
    
    //allocate space for entries in openFiles[]
    for (int i = 0; i < 10; i++) {
        openFiles[i] = (FileInfo*) malloc(sizeof(struct S_fileInfo));
        openFiles[i]->sector = -1;
        openFiles[i]->toBeRemoved = 0;
        openFiles[i]->lck = new Lock("file lock");
    }
    //set up synch
    fsLock = new Lock("fileSysLock");

    std::string *initialWP = new std::string("/");
    std::string *initialWDN = new std::string("/");
    workingPath = initialWP;
    workingDirName = initialWDN;
    
    if (format) {
        BitMap *freeMap = new BitMap(NumSectors);
        Directory *directory = new Directory(NumDirEntries);
        FileHeader *mapHdr = new FileHeader;
        FileHeader *dirHdr = new FileHeader;

        DEBUG('f', "Formatting the file system.\n");

        // First, allocate space for FileHeaders for the directory and bitmap
        // (make sure no one else grabs these!)
        freeMap->Mark(FreeMapSector);	    
        freeMap->Mark(DirectorySector);

        // Second, allocate space for the data blocks containing the contents
        // of the directory and bitmap files.  There better be enough space!
        ASSERT(mapHdr->Allocate(freeMap, FreeMapFileSize));
        ASSERT(dirHdr->Allocate(freeMap, DirectoryFileSize));

        // Flush the bitmap and directory FileHeaders back to disk
        // We need to do this before we can "Open" the file, since open
        // reads the file header off of disk (and currently the disk has garbage
        // on it!).
        DEBUG('f', "Writing headers back to disk.\n");
        mapHdr->WriteBack(FreeMapSector);    
        dirHdr->WriteBack(DirectorySector);

        // OK to open the bitmap and directory files now
        // The file system operations assume these two files are left open
        // while Nachos is running.
        freeMapFile = new OpenFile(FreeMapSector);
        currentThread->directoryFile = 0;
        
        //add to global file table
        openFiles[0]->sector = DirectorySector;
        openFiles[0]->nbOpens = 0;
        openFiles[0]->toBeRemoved = 0;
        openFiles[0]->name = "/";
        
        //add to thread table
        currentThread->openFiles[0]->file = new OpenFile(DirectorySector);
        currentThread->openFiles[0]->systemInfo = openFiles[0];
        
        currentThread->openFiles[0]->systemInfo->nbOpens++;
        //printf("    FILESYS CREATION : ORIGINAL / OPENFILE IS %x\n", (unsigned int)currentThread->openFiles[0]->file); 
        
        //add the two dirs . and ..
        directory->Add(".", DirectorySector, 1);
        directory->Add("..", DirectorySector, 1);
     
        // Once we have the files "open", we can write the initial version
        // of each file back to disk.  The directory at this point is completely
        // empty; but the bitmap has been changed to reflect the fact that
        // sectors on the disk have been allocated for the file headers and
        // to hold the file data for the directory and bitmap.

        DEBUG('f', "Writing bitmap and directory back to disk.\n");
        freeMap->WriteBack(freeMapFile);	 // flush changes to disk
        directory->WriteBack(currentThread->openFiles[0]->file);
        
        if (DebugIsEnabled('f')) {
            freeMap->Print();
            directory->Print();
            delete freeMap; 
            delete directory; 
            delete mapHdr; 
            delete dirHdr;
        }
    } else {
    // if we are not formatting the disk, just open the files representing
    // the bitmap and directory; these are left open while Nachos is running
        freeMapFile = new OpenFile(FreeMapSector);
        currentThread->directoryFile = 0;

        //add to global file table
        openFiles[0]->sector = DirectorySector;
        openFiles[0]->nbOpens = 0;
        openFiles[0]->toBeRemoved = 0;
        openFiles[0]->name = "/";
        
        //add to thread table
        currentThread->openFiles[0]->file = new OpenFile(DirectorySector);
        currentThread->openFiles[0]->systemInfo = openFiles[0];
        currentThread->openFiles[0]->systemInfo->nbOpens++;
    }
        
    
    dbgChecks();
}


FileSystem::~FileSystem() {
    int nbOpenFiles = 0;
    for (int i = 0; i < 10; i++) {
        if (openFiles[i]->sector != -1) {
                nbOpenFiles ++;
                //printf("DESTRUCTOR %s is still open in system %d\n", openFiles[i]->name.c_str(), i);
        }
        delete openFiles[i]->lck;
        free(openFiles[i]);
    }
    //printf("FileSystem::~FileSystem : %d files still open at exit.\n", nbOpenFiles);
    delete fsLock;
}

//----------------------------------------------------------------------
// FileSystem::Create
// 	Create a file in the Nachos file system (similar to UNIX create).
//	Since we can't increase the size of files dynamically, we have
//	to give Create the initial size of the file.
//
//	The steps to create a file are:
//	  Make sure the file doesn't already exist
//        Allocate a sector for the file header
// 	  Allocate space on disk for the data blocks for the file
//	  Add the name to the directory
//	  Store the new file header on disk 
//	  Flush the changes to the bitmap and the directory back to disk
//
//	Return 0 if everything goes ok, otherwise, return -1.
//
// 	Create fails if:
//   		file is already in directory
//	 	no free space for file header
//	 	no free entry for file in directory
//	 	no free space for data blocks for the file 
//
// 	Note that this implementation assumes there is no concurrent access
//	to the file system!
//
//	"name" -- name of file to be created
//	"initialSize" -- size of file to be created
//----------------------------------------------------------------------

int
FileSystem::Create(std::string fileName, int initialSize, bool isDir)
{
    if ((unsigned)initialSize > MaxFileSize) {
        printf("fileSystem::Create file %s is too big\n", fileName.c_str());
        return -1;
    }
    //separate path and name
    std::string path(fileName);
    std::string name;
    std::string backTrackPath;
    int pos = path.find_last_of("/"); 
    if (pos == -1) {
        //no / in path so whole path is a name
        pos = 0;
        name = path;
        path.erase(0, std::string::npos);
    }
    else {
         name = path.substr(pos+1, std::string::npos);
         path.erase(pos, std::string::npos);
    }
    //change to the right dir, and keep opposite path to be able to come back
    if (!path.empty()) {
        backTrackPath = Chdir(path);
        if (backTrackPath == "!") {
            printf("FileSystem::Create Chrdir failed\n");
            return -1;
        }
    }
    
    
    Directory *directory;
    BitMap *freeMap;
    FileHeader *hdr;
    int sector;
    int success;

    DEBUG('f', "Creating file %s, size %d\n", name.c_str(), initialSize);

    directory = new Directory(NumDirEntries);
    if (currentThread->directoryFile == -1) {
        printf("FileSystem::Create: currentThread->directoryFile is -1!\n");
        //go back to initial dir
        if (!backTrackPath.empty()) {
            if (Chdir(backTrackPath) == "!") return -1;
        }
        return -1;
    }
   
    if (currentThread->directoryFile == -1) {
        printf("FileSystem::Create currentThread->directoryFile is -1\n");
        //go back to initial dir
        if (!backTrackPath.empty()) {
            if (Chdir(backTrackPath) == "!") return -1;
        }
        return -1;
    }
    OpenFile *dirFile = currentThread->openFiles[currentThread->directoryFile]->file;
    if (dirFile == NULL) {
        printf("FileSystem::Create dirFile at index fs df %d  ct df %d is NULL\n", currentThread->directoryFile, currentThread->directoryFile);
        //go back to initial dir
        if (!backTrackPath.empty()) {
            if (Chdir(backTrackPath) == "!") return -1;
        }
        return -1;
    }
    directory->FetchFrom(dirFile);

    

    if (directory->Find(name.c_str()) != -1) {
        printf("FileSystem::Create file %s already exists\n", name.c_str());
        success = -1;
    }
    else {	
        freeMap = new BitMap(NumSectors);
        freeMap->FetchFrom(freeMapFile);
        sector = freeMap->Find();	// find a sector to hold the file header
    	if (sector == -1) {		
            printf("FileSystem::Create for file %s : no free block for file header\n", name.c_str());
            success = -1;	        // no free block for file header 
        }	
        else if (!directory->Add(name.c_str(), sector, isDir)) {
                printf("FileSystem::Create for file %s : no space in directory\n", name.c_str());
                success = -1;	    // no space in directory
             }
             else {
                hdr = new FileHeader;
                if (!hdr->Allocate(freeMap, initialSize)) {
                    printf("FileSystem::Create for file %s : no space on disk\n", name.c_str());
                    success = -1;	// no space on disk for data
                }
                else {	
                    success = 0;
                    // everthing worked, flush all changes back to disk
                    hdr->WriteBack(sector); 		
                    directory->WriteBack(dirFile);
                    freeMap->WriteBack(freeMapFile);
                }
                delete hdr;
            }
            delete freeMap;
    }
    delete directory;
    //go back to initial dir
    if (!backTrackPath.empty()) {
        if (Chdir(backTrackPath) == "!") return -1;
    }
    dbgChecks();
    return success;
}



//----------------------------------------------------------------------
// FileSystem::Open
// 	Open a file for reading and writing.
//  Multiple Threads can open the same file, and will use the same OpenFile object.
//  a thread cannot access a file opened by another thread
//	To open a file:
//	  Find the location of the file's header, using the directory 
//	  Bring the header into memory
//
//	"name" -- the text name of the file to be opened
//----------------------------------------------------------------------

int
FileSystem::Open(std::string fileName)
{   
    int index;
    int i;
    //printf("FileSystem::Open(%s)\n", fileName.c_str());
    if (fileName.empty()) {
        printf("FileSystem called with empty filename\n");
        return -1;
    }
    
    //separate path and name
    std::string path(fileName);
    std::string name;
    std::string backTrackPath;
    int pos = path.find_last_of("/"); 
    if (pos == -1) {
        //no / in path so whole path is a name
        pos = 0;
        name = path;
        path.erase(0, std::string::npos);
    }
    else {
         name = path.substr(pos+1, std::string::npos);
         path.erase(pos, std::string::npos);
    }
    //change to the right dir, and keep opposite path to be able to come back
    if (!path.empty()) {
        backTrackPath = Chdir(path);
        if (backTrackPath == "!") {
            printf("FileSystem::Open Chrdir failed\n");
            return -1;
        }
    }
    
        
    Directory *directory = new Directory(NumDirEntries);
    int sector;

    DEBUG('f', "Opening file %s\n", name.c_str());
    OpenFile *dirFile = currentThread->openFiles[currentThread->directoryFile]->file;
    if (dirFile == NULL) {
        printf("FileSystem::Open dirFile %d %s is null!\n", currentThread->directoryFile, currentThread->openFiles[currentThread->directoryFile]->systemInfo->name.c_str()); 
        //go back to initial dir
        if (!backTrackPath.empty()) {
            if (Chdir(backTrackPath) == "!") return -1;
        }
        return -1;
    }
    directory->FetchFrom(dirFile);

    sector = directory->Find(name.c_str()); 
    if (sector < 0) {	
        // name was not found in directory 
        printf("FileSystem::Open didnt find file %s in dir %s\n", name.c_str(), GetWorkingPath().c_str());
        //go back to initial dir
        if (!backTrackPath.empty()) {
            if (Chdir(backTrackPath) == "!") return -1;
        }
        return -1;
    }
    delete directory;
    
    
    //check if is already opened in system
    //printf("Open: looking for sector %d in global table\n", sector);
    for (i = 0; i < 10; i++) {
        //printf("sectors: %d", openFiles[i]->sector);
        int tmp = openFiles[i]->sector;
        if (sector == tmp) {
            break;
        }
    }
    
    if (i == 10) { //if not opened in system yet, open it
        //add to openFiles table
        for (i = 0; i < 10 && openFiles[i]->sector != -1; i++);
        if (i != 10) {
            openFiles[i]->sector = sector;
            openFiles[i]->nbOpens = 0;
            openFiles[i]->toBeRemoved = 0;
            openFiles[i]->name = fileName;
        }
        else {
            printf("FileSystem::Open max open file number reached. Cannot open %s\n", name.c_str());
            //go back to initial dir
            if (!backTrackPath.empty()) {
                if (Chdir(backTrackPath) == "!") return -1;
            }
            return -1;
        }
    }
    else { // file was already opened before; check for remove
        if (openFiles[i]->toBeRemoved) {
            //cannot open as file is waiting to be destroyed
            printf("FileSystem::Open file %s is waiting for destruction, cannot open\n", name.c_str());
            //go back to initial dir
            if (!backTrackPath.empty()) {
                if (Chdir(backTrackPath) == "!") return -1;
            }
            return -1;
        }
    }
    
    //add to thread table 
    //find free slot
    int j;
    for (j = 0; j < 10; j++) {
        if (currentThread->openFiles[j]->file == NULL) {
            index = j;
            break;
        }
    }
    if (j == 10) {
        printf("Filesystem Open: max files open. failed to open\n");
        return -1;
    }
    currentThread->openFiles[index]->systemInfo = openFiles[i];
    currentThread->openFiles[index]->systemInfo->nbOpens++;
    currentThread->openFiles[index]->systemInfo->lck->Acquire();
    currentThread->openFiles[index]->file = new OpenFile(sector);
    currentThread->openFiles[index]->systemInfo->lck->Release();
    //printf("    Open: file %s %x nbopens =%d\n",
//            name.c_str(), (unsigned int)currentThread->openFiles[index]->file, currentThread->openFiles[index]->systemInfo->nbOpens);


    //printf("(re-)Open: file %s %x nbopens =%d\n",name.c_str(), (unsigned int)openFiles[i]->file, openFiles[i]->nbOpens);

    
    //go back to initial dir
    if (!backTrackPath.empty()) {
        if (Chdir(backTrackPath) == "!") return -1;
    }
        
    //printf("filesys::open opened file ptr is %d\n", (int)openFile);
    dbgChecks();
    return index;				// return -1 if not found
}




// FileSystem::Close
// Closes the file specified by index. 

int
FileSystem::Close(int index) {
    //check if is opened by current thread
    if (currentThread->openFiles[index]->file == NULL) {
        printf("FileSystem::Close the file of index %d is not open in this thread\n", index);
        return -1;
    }
    //check if is open in system
    if (currentThread->openFiles[index]->systemInfo->sector == -1) {
        printf("FileSystem::Close the file of index %d name %s is not open in the file system\n", index, currentThread->openFiles[index]->systemInfo->name.c_str());
        return -1;
    }
    
    //close for the thread
    OpenFile *ofptr = currentThread->openFiles[index]->file;
    delete ofptr;
    currentThread->openFiles[index]->file = NULL;
    //update global info
    currentThread->openFiles[index]->systemInfo->nbOpens--;
    
    printf("FileSystem::Close called on index %d, for file %s openfile %x, after close has %d remaining openers\n"
    , index, currentThread->openFiles[index]->systemInfo->name.c_str(), (unsigned int)ofptr, currentThread->openFiles[index]->systemInfo->nbOpens);

    
    //check if we were last one to have it open
    if (currentThread->openFiles[index]->systemInfo->nbOpens == 0) {
        //printf("last person to open File. Closing %d %s in filesys\n", index, currentThread->openFiles[index]->systemInfo->name.c_str());
        //no one else has this file open, do global Close
        currentThread->openFiles[index]->systemInfo->sector = -1;
        if (currentThread->openFiles[index]->systemInfo->toBeRemoved) {
            Remove(currentThread->openFiles[index]->systemInfo->name);
            currentThread->openFiles[index]->systemInfo->toBeRemoved = 0;
        }
    }
    dbgChecks();
    return 0;
}

//----------------------------------------------------------------------
// FileSystem::Remove
// 	Delete a file from the file system.  This requires:
//	    Remove it from the directory
//	    Delete the space for its header
//	    Delete the space for its data blocks
//	    Write changes to directory, bitmap back to disk
//
//	Return 0 if the file was deleted, -1 if the file wasn't
//	in the file system.
//  A call to Remove() on an open file will set flag so that no other can open it.
//  When the last thread closes the file, this flag will trigger a call to Remove, which will then be able to run.
//	"name" -- the text name of the file to be removed
//  The calling thread must have called Close either before or after, otherwise will never remove
//----------------------------------------------------------------------

int
FileSystem::Remove(std::string fileName)
{ 
    //printf("FileSystem::Remove called on %s\n", fileName.c_str());
    //separate path and name
    std::string path(fileName);
    std::string name;
    std::string backTrackPath;
    int pos = path.find_last_of("/"); 
    if (pos == -1) {
        //no / in path so whole path is a name
        pos = 0;
        name = path;
        path.erase(0, std::string::npos);
    }
    else {
         name = path.substr(pos+1, std::string::npos);
         path.erase(pos, std::string::npos);
    }
    //change to the right dir, and keep opposite path to be able to come back
    if (!path.empty()) {
        backTrackPath = Chdir(path);
        if (backTrackPath == "!") {
            printf("FileSystem::Create Chrdir failed\n");
            return -1;
        }
    }
        

        
    Directory *directory;
    BitMap *freeMap;
    FileHeader *fileHdr;
    int sector;
    
    directory = new Directory(NumDirEntries);
    OpenFile *dirFile = currentThread->openFiles[currentThread->directoryFile]->file;
    directory->FetchFrom(dirFile);

    sector = directory->Find(name.c_str());
    if (sector == -1) {
       delete directory;
        //go back to initial dir
        //go back to initial dir
        if (!backTrackPath.empty()) {
            if (Chdir(backTrackPath) == "!") return -1;
        }
       return -1;			 // file not found 
    }
    
    //check if is opened in system
    int i;
    for (i = 0; i < 10; i++) {
        if (openFiles[i]->sector == sector)
            break;
    }
    if (i != 10) {
        //file found, can't delete it
        printf("FileSystem::Remove the file %s is still open\n", name.c_str());
        //mark for removal so no one else opens it
        openFiles[i]->toBeRemoved = 1;
        return 0;
    }
    //file not open, we can delete it
    fileHdr = new FileHeader;
    fileHdr->FetchFrom(sector);

    freeMap = new BitMap(NumSectors);
    freeMap->FetchFrom(freeMapFile);

    fileHdr->Deallocate(freeMap);  		// remove data blocks
    freeMap->Clear(sector);			// remove header block
    directory->Remove(name.c_str());

    freeMap->WriteBack(freeMapFile);		// flush to disk
    
    directory->WriteBack(dirFile);        // flush to disk

    delete fileHdr;
    delete directory;
    delete freeMap;
    //go back to initial dir
    if (!backTrackPath.empty()) {
        if (Chdir(backTrackPath) == "!") return -1;
    }
    dbgChecks();
    return 0;
} 

//----------------------------------------------------------------------
// FileSystem::List
// 	List all the files in the file system directory.
//----------------------------------------------------------------------

void
FileSystem::List()
{
    std::string wp = GetWorkingPath();
    std::string wdn = GetWorkingDir();
    printf("List: workingdir is:%s  dirname is:%s  Contents is:\n", wp.c_str(), wdn.c_str());
    Directory *directory = new Directory(NumDirEntries);
    for (int i = 0; i <10; i++) {
        if (currentThread->openFiles[i] != NULL){
            //printf("%d is: scetor %d\n", i, currentThread->openFiles[i]->systemInfo->sector);
        }
        else
            printf("%d is null\n", i);
    }
    OpenFile *dirFile = currentThread->openFiles[currentThread->directoryFile]->file;
    if (dirFile == NULL) {
        printf("List; dirFile is Null\n");
        Exit(-1);
    }
    directory->FetchFrom(dirFile); 
    directory->List();
    delete directory;
    
    int nbOpenFiles = 0;
    for (int i = 0; i < 10; i++) {
        if (openFiles[i]->sector != -1) {
            nbOpenFiles ++;
            printf("xxxxx %s is still open at system level index %d\n", openFiles[i]->name.c_str(), i);
        }
    }
    printf("%s: %d files still open at exit.\n", __FUNCTION__, nbOpenFiles);
    printf("%s: Total files and dirs Open %d\n", __FUNCTION__, nbOpenFiles);
}

//----------------------------------------------------------------------
// FileSystem::Print
// 	Print everything about the file system:
//	  the contents of the bitmap
//	  the contents of the directory
//	  for each file in the specified directory,
//	      the contents of the file header
//	      the data in the file
//----------------------------------------------------------------------

void
FileSystem::Print()
{
    FileHeader *bitHdr = new FileHeader;
    FileHeader *dirHdr = new FileHeader;
    BitMap *freeMap = new BitMap(NumSectors);
    Directory *directory = new Directory(NumDirEntries);

    printf("\nBit map file header:\n");
    bitHdr->FetchFrom(FreeMapSector);
    bitHdr->Print();

    printf("\nDirectory file header:\n");
    dirHdr->FetchFrom(DirectorySector);
    dirHdr->Print();

    freeMap->FetchFrom(freeMapFile);
    freeMap->Print();
    
    printf("\n");
    OpenFile *dirFile = currentThread->openFiles[currentThread->directoryFile]->file;
    directory->FetchFrom(dirFile);        // flush to disk
    directory->Print();

    delete bitHdr;
    delete dirHdr;
    delete freeMap;
    delete directory;
} 


//Mkdir
//creates an empty directory called name in the current directory

int
FileSystem::Mkdir(const char* dirName) {
    //separate path and name
    std::string path(dirName);
    std::string name;
    std::string backTrackPath;
    int pos = path.find_last_of("/"); 
    if (pos == -1) {
        //no / in path so whole path is a name
        pos = 0;
        name = path;
        path.erase(0, std::string::npos);
    }
    else {
        name = path.substr(pos+1, std::string::npos);
        path.erase(pos, std::string::npos);
    }
    //change to the right dir, and keep opposite path to be able to come back
    if (!path.empty()) {
        backTrackPath = Chdir(path);
        if (backTrackPath == "!") {
            printf("FileSystem::Create Chrdir failed\n");
            return -1;
        }
    }

    //create file representing the new dir in our current directory
    //printf("creating file with name %s size %d\n", name.c_str(), currentThread->directoryFileSize);
    if (-1 == Create(name.c_str(), DirectoryFileSize, 1))  {
        printf("fileSys::mkdir: could not create newDir file %s\n", dirName);
        //go back to initial dir
        if (!backTrackPath.empty()) {
            if (Chdir(backTrackPath) == "!") return -1;
        }
        return -1;
    }
    
    //get relevant info on our current directory
    Directory *currentDir = new Directory(NumDirEntries);
    OpenFile *dirFile = currentThread->openFiles[currentThread->directoryFile]->file;
    currentDir->FetchFrom(dirFile);

    int cDSector = dirFile->getSector();
    if (cDSector < 0) {
        printf("fileSys::Mkdir: currentDir sector is bad\n");
        //go back to initial dir
        if (!backTrackPath.empty()) {
            if (Chdir(backTrackPath) == "!") return -1;
        }
        return -1;
    }
    
    //get sector of our newly created directory file
    int nDSector = currentDir->Find(name.c_str());
    if (nDSector < 0) {
        printf("fileSys::Mkdir: newDir sector is bad\n");
        //go back to initial dir
        if (!backTrackPath.empty()) {
            if (Chdir(backTrackPath) == "!") return -1;
        }
        return -1;
    }
    
    //create directory object for the new dir in memory
    Directory *newDir = new Directory(NumDirEntries);
    //add the two dirs . and ..
    newDir->Add(".", nDSector, 1);
    newDir->Add("..", cDSector, 1);

    //write changes back to current directory
    currentDir->WriteBack(dirFile);

    int index = Open(name.c_str());
    if (index == -1) {
        printf("fileSys::Mkdir: could not open newDirFile %s\n", dirName);
        //go back to initial dir
        if (!backTrackPath.empty()) {
            if (Chdir(backTrackPath) == "!") return -1;
        }
        return -1;
    }
    OpenFile *newDirFile = currentThread->openFiles[index]->file;
    newDir->WriteBack(newDirFile);
    Close(index);
    
    if (!backTrackPath.empty())
        Chdir(backTrackPath);
    return 0;
 }
 
 
//Rmdir
//pre: directory must be empty
//removes the directory specifed by dirName
//if directory is open by another thread, will wait until thread leaves it to delete dir

int
FileSystem::Rmdir(const char* dirName) {

    //separate path and name
    std::string path(dirName);
    std::string name;
    std::string backTrackPath;
    int pos = path.find_last_of("/"); 
    if (pos == -1) {
        //no / in path so whole path is a name
        pos = 0;
        name = path;
        path.erase(0, std::string::npos);
    }
    else {
         name = path.substr(pos+1, std::string::npos);
         path.erase(pos, std::string::npos);
    }
    //change to the right dir, and keep opposite path to be able to come back
    if (!path.empty()) {
        backTrackPath = Chdir(path);
        if (backTrackPath == "!") {
            printf("FileSystem::Create Chrdir failed\n");
            return -1;
        }
    }
    
    //create directory object for the dir in memory
    Directory *rmDir = new Directory(NumDirEntries);
    int index = Open(name.c_str());
    if (index == -1) {
        printf("fileSys::Rmdir: could not open rmDirFile %s\n", dirName);
        //go back to initial dir
        if (!backTrackPath.empty()) {
            if (Chdir(backTrackPath) == "!") return -1;
        }
        return -1;
    }
    OpenFile *rmDirFile = currentThread->openFiles[index]->file;
    rmDir->FetchFrom(rmDirFile);
    Close(index);
    
    //check dir is empty
    if (!rmDir->Empty()) {
        printf("filesys::Rmdir: directory %s is not empty\n", dirName);
        return -1;
    }
    Remove(name.c_str());
    
    //go back to initial dir
    if (!backTrackPath.empty()) {
        if (Chdir(backTrackPath) == "!") return -1;
    }
        
    return 0;

 }
 
 

/*
 * returns path "!" on error
 */
std::string
FileSystem::Chdir(std::string path) {
    
    std::string oppositePath(".");
    do {
        int pos = path.find_first_of("/"); 
        std::string name = path.substr(0, pos);
        path.erase(0, pos);
        path.erase(0, 1); //remove /
        
        printf("chdir remaining path is %s    changing to name:%s\n", path.c_str(), name.c_str());

        
        if (name == ".") {
            //we're already here... do nothing
           continue;
        }
        
        int index = Open(name.c_str());
        if (index == -1) {
            printf("filesys::Chdir: cannot open directory %s\n", name.c_str());
            if (!oppositePath.empty()) {
                Chdir(oppositePath);
            }
            return "!";
        }
        Close(currentThread->directoryFile);
        currentThread->directoryFile = index;

        //update current working path and directory name
        if (name =="..") {
            oppositePath.append("/");
            oppositePath.append(*workingDirName);
            
            pos = workingPath->find_last_of('/');
            workingPath->erase(pos, std::string::npos);
            if (*workingPath != "/") {
                pos = workingPath->find_last_of('/');
                *workingDirName = workingPath->substr(pos+1, std::string::npos);
            }
            else {
                *workingDirName = "/";
            }
        }
        else {
            oppositePath.append("/..");
            
            workingPath->append("/");
            workingPath->append(name);
            *workingDirName = name;
        }
    } while (!path.empty());
    return oppositePath;
}

std::string
FileSystem::GetWorkingPath() {
    std::string tmp = *workingPath;
    return tmp;
}

std::string
FileSystem::GetWorkingDir() {
    std::string tmp = *workingDirName;
    return tmp;
}

 

int 
FileSystem::ReadAt(char *buffer, int numBytes, int index, int pos) {
    if (index < 0 || index > 9) {
        printf("WriteAt bad index\n");
        return -1;
    }
    OpenFile *ofid = currentThread->openFiles[index]->file;
    if (ofid == NULL) {
        printf("Thread %s does not have this file Open: cannot read\n", currentThread->getName());
        return -1;
    }
    currentThread->openFiles[index]->systemInfo->lck->Acquire();
    int result = ofid->ReadAt(buffer, numBytes, pos);
    currentThread->openFiles[index]->systemInfo->lck->Release();
    return result;
}

int 
FileSystem::WriteAt(char *buffer, int numBytes, int index, int pos) {
    if (index < 0 || index > 9) {
        printf("WriteAt bad index\n");
        return -1;
    }
    OpenFile *ofid = currentThread->openFiles[index]->file;
    if (ofid == NULL) {
        printf("Thread %s does not have this file Open: cannot Write\n", currentThread->getName());
        return -1;
    }
    currentThread->openFiles[index]->systemInfo->lck->Acquire();
    int result = ofid->WriteAt(buffer, numBytes, pos);
    currentThread->openFiles[index]->systemInfo->lck->Release();
    return result;
}


int
FileSystem::Read(char *buffer, int numBytes, int index) {
    if (index < 0 || index > 9) {
        printf("WriteAt bad index\n");
        return -1;
    }
    OpenFile *ofid = currentThread->openFiles[index]->file;
    if (ofid == NULL) {
        printf("Thread %s does not have this file Open: cannot read\n", currentThread->getName());
        return -1;
    }
    currentThread->openFiles[index]->systemInfo->lck->Acquire();
    int result = ofid->Read(buffer, numBytes);
    currentThread->openFiles[index]->systemInfo->lck->Release();
    return result;
}

int
FileSystem::Write(char *buffer, int numBytes, int index) {
    if (index < 0 || index > 9) {
        printf("WriteAt bad index\n");
        return -1;
    }
    OpenFile *ofid = currentThread->openFiles[index]->file;
    if (ofid == NULL) {
        printf("Thread %s does not have this file Open: cannot Write\n", currentThread->getName());
        return -1;
    }
    currentThread->openFiles[index]->systemInfo->lck->Acquire();
    int result = ofid->Write(buffer, numBytes);
    currentThread->openFiles[index]->systemInfo->lck->Release();
    return result;
}

OpenFile *
FileSystem::getOpenFile(int index) {
    return currentThread->openFiles[index]->file;
}

//to be called by creator thread when it is in the initial working dir of new thread
//-1 on fail 0 success
int
FileSystem::InitializeThreadWorkingDir(Thread *newThread) {
    //printf("InitThrdWD called from %s for %s\n", currentThread->getName(), newThread->getName());
    int index, i;
    std::string fileName(*(newThread->workingDirName));
    std::string name("../");
    //if (fileName == "/") {
        name = ".";
    //}
    //else {
    //    name.append(fileName);
    //}
    printf("initThrdWD name of dir is %s, looking for it from %s\n", name.c_str(), fileName.c_str());
    Directory *directory = new Directory(NumDirEntries);
    int sector;

    OpenFile *dirFile = currentThread->openFiles[currentThread->directoryFile]->file;
    if (dirFile == NULL) {
        printf("FileSystem::initThrdWd dirFile %d %s is null! sector is %d \n", 
        currentThread->directoryFile, currentThread->openFiles[currentThread->directoryFile]->systemInfo->name.c_str(),
        currentThread->openFiles[currentThread->directoryFile]->systemInfo->sector); 
        return -1;
    }
    directory->FetchFrom(dirFile);

    sector = directory->Find(name.c_str()); 
    if (sector < 0) {	
        // name was not found in directory 
        printf("FileSystem::initThrdWd didnt find file %s in dir %s\n", name.c_str(), GetWorkingPath().c_str());
        return -1;
    }
    delete directory;
    
    
    //check if is already opened in system
    for (i = 0; i < 10; i++) {
        int tmp = openFiles[i]->sector;
        if (sector == tmp) {
            break;
        }
    }
    
    if (i == 10) { //if not opened in system yet, open it
        //printf("FileSystem::initThrdWd file %s in dir %s is not open in system\n", name.c_str(), GetWorkingPath().c_str());
        //add to openFiles table
        for (i = 0; i < 10 && openFiles[i]->sector != -1; i++);
        if (i != 10) {
            openFiles[i]->sector = sector;
            openFiles[i]->nbOpens = 0;
            openFiles[i]->toBeRemoved = 0;
            openFiles[i]->name = name;
        }
        else {
            printf("FileSystem::initThrdWd max open file number reached. Cannot open %s\n", name.c_str());
            return -1;
        }
    }
    else { // file was already opened before; check for remove
        if (openFiles[i]->toBeRemoved) {
            //cannot open as file is waiting to be destroyed
            printf("FileSystem::initThrdWd file %s is waiting for destruction, cannot open\n", name.c_str());
            //go back to initial dir
            return -1;
        }
    }
    
    index = 0;
    newThread->openFiles[index]->systemInfo = openFiles[i];
    newThread->openFiles[index]->systemInfo->nbOpens++;
    newThread->openFiles[index]->systemInfo->lck->Acquire();
    newThread->openFiles[index]->file = new OpenFile(sector);
    newThread->openFiles[index]->systemInfo->lck->Release();
    newThread->directoryFile = index;
    printf("    initthrdwd: done open for file %s %x sector %d nbopens =%d\n",
            name.c_str(), (unsigned int)newThread->openFiles[index]->file, sector, newThread->openFiles[index]->systemInfo->nbOpens);

    return 0;
}
    
