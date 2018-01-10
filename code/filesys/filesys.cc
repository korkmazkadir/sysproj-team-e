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
    }
    
    //set currentDirectory name
    workingDirName = "/";
    workingPath = "/";
    
    //allocate space for openFiles table
    openFiles = (OpenFile**) malloc(sizeof(OpenFile*) * 10);
    if (openFiles == NULL) {
        printf("FileSystem::constructor malloc for openFiles[] failed\n");
        Exit(1);
    }
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
    bool success;

    DEBUG('f', "Creating file %s, size %d\n", name.c_str(), initialSize);

    directory = new Directory(NumDirEntries);
    directory->FetchFrom(directoryFile);

    if (directory->Find(name.c_str()) != -1) {
        success = -1;			// file is already in directory
    }
    else {	
        freeMap = new BitMap(NumSectors);
        freeMap->FetchFrom(freeMapFile);
        sector = freeMap->Find();	// find a sector to hold the file header
    	if (sector == -1) {		
            success = -1;	        // no free block for file header 
        }	
        else if (!directory->Add(name.c_str(), sector, isDir)) {
                success = -1;	    // no space in directory
             }
             else {
                hdr = new FileHeader;
                if (!hdr->Allocate(freeMap, initialSize))
                    success = -1;	// no space on disk for data
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
    return success;
}

//----------------------------------------------------------------------
// FileSystem::Open
// 	Open a file for reading and writing.  
//	To open a file:
//	  Find the location of the file's header, using the directory 
//	  Bring the header into memory
//
//	"name" -- the text name of the file to be opened
//----------------------------------------------------------------------


//write a close function...??
//TODO: convert Open Close etc to use paths
OpenFile *
FileSystem::Open(std::string fileName)
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
    
        
    Directory *directory = new Directory(NumDirEntries);
    OpenFile *openFile = NULL;
    int sector;

    DEBUG('f', "Opening file %s\n", name.c_str());
    directory->FetchFrom(directoryFile);
    sector = directory->Find(name.c_str()); 
    
    //check if is already opened
    int i;
    for (i = 0; i < 10; i++) {
        if (openFiles[i] != NULL) {
            int tmp = openFiles[i]->getSector();
            if (sector == tmp) {
                break;
            }
        }
    }
    
    
    if (i != 10) {
        //file found
        printf("FileSystem::Open the file %s is already open\n", name.c_str());
        return NULL;
    }
    
    if (sector >= 0) 		
        openFile = new OpenFile(sector);	// name was found in directory 
    delete directory;
    
    //add to openFiles table
    for (i = 0; i < 10 && openFiles[i] != NULL; i++);
    if (i != 10) {
        openFiles[i] = openFile;
    }
    else {
        printf("FileSystem::Open max open file number reached. Cannot open %s\n", name.c_str());
        return NULL;
    }
        
    //go back to initial dir
    if (!backTrackPath.empty())
        Chdir(backTrackPath);
        
    printf("filesys::open opened file ptr is %d\n", (int)openFile);
    return openFile;				// return NULL if not found
}


// FileSystem::Close
// 
int
FileSystem::Close(OpenFile *ofid) {
    //check if is opened
    int i;
    bool found = FALSE;
    for (i = 0; i < 10; i++) {
        if (openFiles[i] == ofid) {
            found = TRUE;
            break;
        }
    }
    
    
    if (!found) {
        //file not found
        printf("FileSystem::Close the file of id %d is not open\n", (int)ofid);
        return -1;
    }
    
    delete openFiles[i];
    openFiles[i] = NULL;
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
//	Return TRUE if the file was deleted, FALSE if the file wasn't
//	in the file system.
//
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
    return 0;
} 

//----------------------------------------------------------------------
// FileSystem::List
// 	List all the files in the file system directory.
//----------------------------------------------------------------------

void
FileSystem::List()
{
    printf("List: workingdir is:%s  dirname is:%s  Contents is:\n", GetWorkingPath(), GetWorkingDir());
    Directory *directory = new Directory(NumDirEntries);
    directory->FetchFrom(directoryFile);
    directory->List();
    delete directory;
}

//----------------------------------------------------------------------
// FileSystem::Print
// 	Print everything about the file system:
//	  the contents of the bitmap
//	  the contents of the directory
//	  for each file in the directory,
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
    printf("creating file with name %s\n", name.c_str());
    if (-1 == Create(name.c_str(), DirectoryFileSize, 1))  {
        printf("fileSys::mkdir: could not create newDir file %s\n", dirName);
        return -1;
    }
    
    //get relevant info on our current directory
    Directory *currentDir = new Directory(NumDirEntries);
    currentDir->FetchFrom(directoryFile);
    int cDSector = directoryFile->getSector();
    if (cDSector <= 0) {
        printf("fileSys::Mkdir: currentDir sector is bad\n");
        return -1;
    }
    
    //get sector of our newly created directory file
    int nDSector = currentDir->Find(name.c_str());
    if (nDSector <= 0) {
        printf("fileSys::Mkdir: newDir sector is bad\n");
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
 
 
 //TODO: maybe change so that on failure, doesn't change working dir?
 
std::string
FileSystem::Chdir(std::string path) {
        
    std::string oppositePath(".");
    do {
        int pos = path.find_first_of("/"); 
        std::string name = path.substr(0, pos);
        path.erase(0, pos);
        path.erase(0, 1); //remove /
        
        printf("chdir remaining path is %s    changing to name:%s\n", path.c_str(), name.c_str());

        
        // TODO: unsure of this...   what to do for "." ?
        if (name == ".") {
           continue;
        }
        
        OpenFile *destDirFile = Open(name.c_str());
        if (destDirFile == NULL) {
           printf("filesys::Chdir: cannot open directory %s\n", name.c_str());
           return NULL;
        }
        Close(directoryFile);
        directoryFile = destDirFile;

        //update current working path and directory name
        if (name =="..") {
            oppositePath.append("/");
            oppositePath.append(workingDirName);
            
            pos = workingPath.find_last_of('/');
            workingPath.erase(pos, std::string::npos);
            pos = workingPath.find_last_of('/');
            workingDirName = workingPath.substr(pos+1, std::string::npos);
        }
        else {
            oppositePath.append("/..");
            
            workingPath.append("/");
            workingPath.append(name);
            workingDirName = name;
        }
    } while (!path.empty());
    return oppositePath;
}

const char *
FileSystem::GetWorkingPath() {
    return workingPath.c_str();
}

const char *
FileSystem::GetWorkingDir() {
    return workingDirName.c_str();
}
 
 
void 
FileSystem::switchProcess(OpenFile *openFileTable[10], OpenFile *currDirFile) {
    /*for (int i = 0; i < 10; i++) {
        if (openFiles[i] != NULL) {*/
            
    openFiles = openFileTable;
    directoryFile = currDirFile;
}
 

