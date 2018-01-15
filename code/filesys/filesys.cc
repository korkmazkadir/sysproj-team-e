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
    if (directoryFile == NULL) {
        printf("FS dbgchecks NULL directoryFile = %x\n", (unsigned int) directoryFile);
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
    
    //allocate space for openFiles table
    openFiles = (FileInfo**) malloc(sizeof(FileInfo*) * 10);
    if (openFiles == NULL) {
        printf("FileSystem::constructor malloc for openFiles[] failed\n");
        Exit(1);
    }
    for (int i = 0; i < 10; i++) {
        openFiles[i] = (FileInfo*) malloc(sizeof(struct S_fileInfo));
        openFiles[i]->file = NULL;
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
        directoryFile = new OpenFile(DirectorySector);
        if (directoryFile == NULL) {
            printf("FileSystem::FileSystem() failed to create directoryFile\n");
            Exit(-1);
        }
        openFiles[0]->file = directoryFile;
        openFiles[0]->nbOpens = 1;
        openFiles[0]->name = "/";

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
        directory->WriteBack(directoryFile);
        
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
        directoryFile = new OpenFile(DirectorySector);
        if (directoryFile == NULL) {
            printf("FileSystem::FileSystem() failed to create directoryFile\n");
            Exit(-1);
        }
        openFiles[0]->file = directoryFile;
        openFiles[0]->nbOpens = 1;
    }
        
    
    dbgChecks();
}


FileSystem::~FileSystem() {
    int nbOpenFiles = 0;
    for (int i = 0; i < 10; i++) {
        if (openFiles[i] != NULL) {
            if (openFiles[i]->file != NULL) {
                delete openFiles[i]->file;
                nbOpenFiles ++;
                //printf("xxxxx %s is still open\n", openFiles[i]->name.c_str());
            }
            free (openFiles[i]);
        }
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
    if (!path.empty())
        backTrackPath = Chdir(path);
    
    
    Directory *directory;
    BitMap *freeMap;
    FileHeader *hdr;
    int sector;
    int success;

    DEBUG('f', "Creating file %s, size %d\n", name.c_str(), initialSize);

    directory = new Directory(NumDirEntries);
    if (directoryFile == NULL) {
        printf("FileSystem::Create: directoryFile is NULL!\n");
        success = -1;
    }
    else {
        directory->FetchFrom(directoryFile);
    }

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
                    directory->WriteBack(directoryFile);
                    freeMap->WriteBack(freeMapFile);
                }
                delete hdr;
            }
            delete freeMap;
    }
    delete directory;
    //go back to initial dir
    if (!backTrackPath.empty())
        Chdir(backTrackPath);
    dbgChecks();
    return success;
}


/*
 * Returns -1 on failure, and an integer index corresponding to openFile ID otherwise
 */
int 
FileSystem::ThreadOpen(std::string fileName) {
    int index;
    int i;
    for (i = 0; i < 10; i++) {
        if (threadOpenFiles[i] == NULL) {
            index = i;
            break;
        }
    }
    if (i == 10) {
        printf("Filesystem ThreadOpen: max files open. failed to open\n");
        return -1;
    }
    OpenFile *ofid = Open(fileName);
    if (ofid == NULL) {
        printf("Filesystem ThreadOpen: failed to open\n");
        return -1;
    }
    threadOpenFiles[index] = ofid;
    return index;
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

OpenFile *
FileSystem::Open(std::string fileName)
{   
    //printf("Open(%s)\n", fileName.c_str());
    if (fileName.empty()) {
        printf("FileSystem called with empty filename\n");
        return NULL;
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
    if (!path.empty())
        backTrackPath = Chdir(path);
    
        
    Directory *directory = new Directory(NumDirEntries);
    OpenFile *openFile = NULL;
    int sector;

    DEBUG('f', "Opening file %s\n", name.c_str());
    directory->FetchFrom(directoryFile);
    sector = directory->Find(name.c_str()); 
    if (sector < 0) {	
        // name was not found in directory 
        printf("FileSystem::Open didnt find file %s in dir %s\n", name.c_str(), GetWorkingPath().c_str());
        if (!backTrackPath.empty())
            Chdir(backTrackPath);
        return NULL;
    }
    delete directory;
    
    
    //check if is already opened
    int i;
    for (i = 0; i < 10; i++) {
        if (openFiles[i]->file != NULL) {
            int tmp = openFiles[i]->file->getSector();
            if (sector == tmp) {
                break;
            }
        }
    }
   
    if (i == 10) { //if not opened yet
        //create new openfile object
        openFile = new OpenFile(sector);
        //add to openFiles table
        for (i = 0; i < 10 && openFiles[i]->file != NULL; i++);
        if (i != 10) {
            openFiles[i]->file = openFile;
            openFiles[i]->nbOpens = 1;
            openFiles[i]->toBeRemoved = 0;
            openFiles[i]->name = fileName;
            //printf("Open: file %s %x nbopens =%d\n",name.c_str(), (unsigned int)openFiles[i]->file, openFiles[i]->nbOpens);
        }
        else {
            printf("FileSystem::Open max open file number reached. Cannot open %s\n", name.c_str());
            //return to initial dir
            if (!backTrackPath.empty())
                Chdir(backTrackPath);
            return NULL;
        }
    }
    else { // if the file was already open
        if (openFiles[i]->toBeRemoved) {
            //cannot open as file is waiting to be destroyed
            printf("FileSystem::Open file %s is waiting for destruction, cannot open\n", name.c_str());
            if (!backTrackPath.empty())
                Chdir(backTrackPath);
            return NULL;
        }
        openFiles[i]->nbOpens++;
        openFile = openFiles[i]->file;
        //printf("(re-)Open: file %s %x nbopens =%d\n",name.c_str(), (unsigned int)openFiles[i]->file, openFiles[i]->nbOpens);
    }
    
    //go back to initial dir
    if (!backTrackPath.empty())
        Chdir(backTrackPath);
        
    //printf("filesys::open opened file ptr is %d\n", (int)openFile);
    dbgChecks();
    return openFile;				// return NULL if not found
}



int 
FileSystem::ThreadClose(int index) {
    
    OpenFile *ofid = threadOpenFiles[index];
    if (ofid == NULL) {
        printf("Filesystem ThreadClose: file not open\n");
        return -1;
    }
    int ret = Close(ofid);
    if (ret != -1) {
        threadOpenFiles[index] = NULL;
    }
    return ret;
}


// FileSystem::Close
// Closes the file specified by ofid. 

int
FileSystem::Close(OpenFile *ofid) {
    //check if is opened
    int i;
    bool found = FALSE;
    for (i = 0; i < 10; i++) {
        if (openFiles[i]->file == ofid) {
            found = TRUE;
            break;
        }
    }
    if (!found) {
        //file not found
        printf("FileSystem::Close the file of id %x is not open\n", (unsigned int)ofid);
        return -1;
    }
    openFiles[i]->nbOpens--;
    if (openFiles[i]->nbOpens == 0) {
        //no one else has this file open
        delete openFiles[i]->file;
        openFiles[i]->file = NULL;
        if (openFiles[i]->toBeRemoved) {
            Remove(openFiles[i]->name);
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
//----------------------------------------------------------------------

int
FileSystem::Remove(std::string fileName)
{ 
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
    if (!path.empty())
        backTrackPath = Chdir(path);
        

        
    Directory *directory;
    BitMap *freeMap;
    FileHeader *fileHdr;
    int sector;
    
    directory = new Directory(NumDirEntries);
    directory->FetchFrom(directoryFile);
    sector = directory->Find(name.c_str());
    if (sector == -1) {
       delete directory;
        //go back to initial dir
        if (!backTrackPath.empty())
        Chdir(backTrackPath);
       return -1;			 // file not found 
    }
    
    //check if is opened
    int i;
    for (i = 0; i < 10; i++) {
        if (openFiles[i]->file != NULL) {
            int tmp = openFiles[i]->file->getSector();
            if (sector == tmp) {
                break;
            }
        }
    }
    if (i != 10) {
        //file found, can't delete yet
        printf("FileSystem::Remove the file %s %x is still open\n", name.c_str(), (unsigned int)openFiles[i]->file);
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
    directory->WriteBack(directoryFile);        // flush to disk
    
    delete fileHdr;
    delete directory;
    delete freeMap;
    //go back to initial dir
    if (!backTrackPath.empty())
        Chdir(backTrackPath);
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
    directory->FetchFrom(directoryFile);
    directory->List();
    delete directory;
    
    int nbOpenFiles = 0;
    for (int i = 0; i < 10; i++) {
        if (openFiles[i] != NULL) {
            if (openFiles[i]->file != NULL) {
                nbOpenFiles ++;
                printf("xxxxx %s is still open\n", openFiles[i]->name.c_str());
            }
        }
    }
    printf("FileSystem::~FileSystem : %d files still open at exit.\n", nbOpenFiles);
    printf("List: Total files and dirs Open %d\n", nbOpenFiles);
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

    printf("Bit map file header:\n");
    bitHdr->FetchFrom(FreeMapSector);
    bitHdr->Print();

    printf("Directory file header:\n");
    dirHdr->FetchFrom(DirectorySector);
    dirHdr->Print();

    freeMap->FetchFrom(freeMapFile);
    freeMap->Print();

    directory->FetchFrom(directoryFile);
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
    if (!path.empty())
        backTrackPath = Chdir(path);

    //create file representing the new dir in our current directory
    //printf("creating file with name %s\n", name.c_str());
    if (-1 == Create(name.c_str(), DirectoryFileSize, 1))  {
        printf("fileSys::mkdir: could not create newDir file %s\n", dirName);
        //return to initial dir
        if (!backTrackPath.empty())
            Chdir(backTrackPath);
        return -1;
    }
    
    //get relevant info on our current directory
    Directory *currentDir = new Directory(NumDirEntries);
    currentDir->FetchFrom(directoryFile);
    int cDSector = directoryFile->getSector();
    if (cDSector < 0) {
        printf("fileSys::Mkdir: currentDir sector is bad\n");
        //return to initial dir
        if (!backTrackPath.empty())
            Chdir(backTrackPath);
        return -1;
    }
    
    //get sector of our newly created directory file
    int nDSector = currentDir->Find(name.c_str());
    if (nDSector < 0) {
        printf("fileSys::Mkdir: newDir sector is bad\n");
        //return to initial dir
        if (!backTrackPath.empty())
            Chdir(backTrackPath);
        return -1;
    }
    
    //create directory object for the new dir in memory
    Directory *newDir = new Directory(NumDirEntries);
    //add the two dirs . and ..
    newDir->Add(".", nDSector, 1);
    newDir->Add("..", cDSector, 1);

    //write changes back to current directory
    currentDir->WriteBack(directoryFile);

    OpenFile * newDirFile = Open(name.c_str());
    if (newDirFile == NULL) {
        printf("fileSys::Mkdir: could not open newDirFile %s\n", dirName);
        //return to initial dir
        if (!backTrackPath.empty())
            Chdir(backTrackPath);
        return -1;
    }
    newDir->WriteBack(newDirFile);
    Close(newDirFile);
    
    if (!backTrackPath.empty())
        Chdir(backTrackPath);
    return 0;
 }
 
 
//Rmdir
//pre: directory must be empty
//removes the directory specifed by dirName

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
    if (!path.empty())
        backTrackPath = Chdir(path);
    
    //create directory object for the dir in memory
    Directory *rmDir = new Directory(NumDirEntries);
    OpenFile *rmDirFile = Open(name.c_str());
    if (rmDirFile == NULL) {
        printf("fileSys::Rmdir: could not open rmDirFile %s\n", dirName);
        //return to initial dir
        if (!backTrackPath.empty())
            Chdir(backTrackPath);
        return -1;
    }
    rmDir->FetchFrom(rmDirFile);
    Close(rmDirFile);
    
    //check dir is empty
    if (!rmDir->Empty()) {
        printf("filesys::Rmdir: directory %s is not empty\n", dirName);
        return -1;
    }
    Remove(name.c_str());
    
    //return to initial dir
    if (!backTrackPath.empty())
        Chdir(backTrackPath);
        
    return 0;

 }
 
 


std::string
FileSystem::Chdir(std::string path) {
    
    std::string oppositePath(".");
    do {
        int pos = path.find_first_of("/"); 
        std::string name = path.substr(0, pos);
        path.erase(0, pos);
        path.erase(0, 1); //remove /
        
        //printf("chdir remaining path is %s    changing to name:%s\n", path.c_str(), name.c_str());

        
        if (name == ".") {
            //we're already here... do nothing
           continue;
        }
        
        OpenFile *destDirFile = Open(name.c_str());
        if (destDirFile == NULL) {
            printf("filesys::Chdir: cannot open directory %s\n", name.c_str());
            if (!oppositePath.empty()) {
                Chdir(oppositePath);
            }
            return NULL;
        }
        Close(directoryFile);
        directoryFile = destDirFile;

        //update current working path and directory name
        if (name =="..") {
            oppositePath.append("/");
            oppositePath.append(*workingDirName);
            
            pos = workingPath->find_last_of('/');
            workingPath->erase(pos, std::string::npos);
            pos = workingPath->find_last_of('/');
            *workingDirName = workingPath->substr(pos+1, std::string::npos);
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

 
 
/*
 * Save filesystem state to current thread
 */
void 
FileSystem::saveThreadState() {   
    //printf("Filesys running thread save state        to: %s\n",currentThread->getName());
    if (directoryFile == NULL) {
        printf("FileSystem::saveThreadState directoryFile is NULL\n");
        Exit(-1);
    }
    //printf("FS WP = %s FS WDN = %s FS DF = %x\n", workingPath->c_str(), workingDirName->c_str(), (unsigned int)directoryFile);
    //printf("CT WP = %x CT WDN = %x CT DF = %x\n", (unsigned int)(currentThread->workingPath), (unsigned int)(currentThread->workingDirName), (unsigned int)currentThread->directoryFile);
    //thread's openFiles table is accessed directly, no need to explicitly save
    currentThread->workingPath = workingPath;
    currentThread->workingDirName = workingDirName;
    currentThread->directoryFile = directoryFile;
    //dbgChecks();
    
}
 
 
/*
 * Load filesystem state from currentThread
 */
void 
FileSystem::restoreThreadState() {
    
    threadOpenFiles = currentThread->openFileIds;
    workingPath = currentThread->workingPath;
    workingDirName = currentThread->workingDirName;
    directoryFile = currentThread->directoryFile;
    //printf("Filesys has run thread restore state         from: %s\n",currentThread->getName());
    //printf("FS WP ptr = %x FS WDN ptr = %x FS DF = %x\n", (unsigned int)workingPath, (unsigned int)workingDirName, (unsigned int)directoryFile);
    //printf("FS WP = %s", workingPath->c_str());
    //printf("FS WDN = %s \n", workingDirName->c_str());
    //dbgChecks();
    
} 



int 
FileSystem::ReadAt(char *buffer, int numBytes, int pos, int index) {
    OpenFile *ofid = threadOpenFiles[index];
    if (ofid == NULL) {
        printf("Thread %s does not have this file Open: cannot read\n", currentThread->getName());
        return -1;
    }
    if (pos == 0) {
        return ofid->Read(buffer, numBytes);
    }
    else {
        return ofid->ReadAt(buffer, numBytes, pos);
    }
}

int 
FileSystem::WriteAt(char *buffer, int numBytes, int pos, int index) {
    OpenFile *ofid = threadOpenFiles[index];
    if (ofid == NULL) {
        printf("Thread %s does not have this file Open: cannot Write\n", currentThread->getName());
        return -1;
    }
    if (pos == 0) {
        return ofid->Write(buffer, numBytes);
    }
    else {
        return ofid->WriteAt(buffer, numBytes, pos);
    }
}


int
FileSystem::Read(char *buffer, int numBytes, int index) {
    OpenFile *ofid = threadOpenFiles[index];
    if (ofid == NULL) {
        printf("Thread %s does not have this file Open: cannot read\n", currentThread->getName());
        return -1;
    }
    return ofid->Read(buffer, numBytes);
}

int
FileSystem::Write(char *buffer, int numBytes, int index) {
    OpenFile *ofid = threadOpenFiles[index];
    if (ofid == NULL) {
        printf("Thread %s does not have this file Open: cannot Write\n", currentThread->getName());
        return -1;
    }
    return ofid->Write(buffer, numBytes);
}
