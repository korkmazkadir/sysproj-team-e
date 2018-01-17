// filehdr.cc 
//	Routines for managing the disk file header (in UNIX, this
//	would be called the i-node).
//
//	The file header is used to locate where on disk the 
//	file's data is stored.  We implement this as a fixed size
//	table of pointers -- each entry in the table points to the 
//	disk sector containing that portion of the file data
//	(in other words, there are no indirect or doubly indirect 
//	blocks). The table size is chosen so that the file header
//	will be just big enough to fit in one disk sector, 
//
//      Unlike in a real system, we do not keep track of file permissions, 
//	ownership, last modification date, etc., in the file header. 
//
//	A file header can be initialized in two ways:
//	   for a new file, by modifying the in-memory data structure
//	     to point to the newly allocated data blocks
//	   for a file already on disk, by reading the file header from disk
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#include "system.h"
#include "filehdr.h"

//----------------------------------------------------------------------
// FileHeader::Allocate
// 	Initialize a fresh file header for a newly created file.
//	Allocate data blocks for the file out of the map of free disk blocks.
//	Return FALSE if there are not enough free blocks to accomodate
//	the new file.
//
//	"freeMap" is the bit map of free disk sectors
//	"fileSize" is the bit map of free disk sectors
//----------------------------------------------------------------------

bool
FileHeader::Allocate(BitMap *freeMap, int fileSize)
{ 
    //printf("------------> NumDirect : %d\n",NumDirect);
    
    numBytes = fileSize;
    numSectors  = divRoundUp(fileSize, SectorSize);
    if (freeMap->NumClear() < numSectors)
	return FALSE;		// not enough space
    
    //printf("--> (indirect) NumSectors : %d\n",numSectors);

    int indirectSectorCount = divRoundUp(numSectors,NumIndirect);
    int arrangedSectorCount = 0;

    //printf("--> (indirect) Indirect sector count : %d\n",indirectSectorCount);

    for (int i = 0; i < indirectSectorCount; i++){
        dataSectors[i] = freeMap->Find();
        IndirectDataBlock indirect;
        for(unsigned int j=0; (j<NumIndirect && arrangedSectorCount < numSectors ) ;j++){
            indirect.dataSectors[j] = freeMap->Find();
            arrangedSectorCount++;
        }

        indirect.writeIndirectBlocks(dataSectors[i]);
    }

    return TRUE;        
}

//----------------------------------------------------------------------
// FileHeader::Deallocate
// 	De-allocate all the space allocated for data blocks for this file.
//
//	"freeMap" is the bit map of free disk sectors
//----------------------------------------------------------------------

void 
FileHeader::Deallocate(BitMap *freeMap)
{
    int indirectSectorCount = divRoundUp(numSectors,NumIndirect);
    int arrangedSectorCount = 0;

    for (int i = 0; i < indirectSectorCount; i++){
        ASSERT(freeMap->Test((int) dataSectors[i]));
        IndirectDataBlock indirect;
        indirect.fetchIndirectBlocks(dataSectors[i]);

        for(unsigned int j=0; (j<NumIndirect && arrangedSectorCount < numSectors) ;j++){

            ASSERT(freeMap->Test(indirect.dataSectors[j]));  // ought to be marked!
            freeMap->Clear(indirect.dataSectors[j]);
            arrangedSectorCount++;
        }

        freeMap->Clear((int) dataSectors[i]);
    }
}

//----------------------------------------------------------------------
// FileHeader::FetchFrom
// 	Fetch contents of file header from disk. 
//
//	"sector" is the disk sector containing the file header
//----------------------------------------------------------------------

void
FileHeader::FetchFrom(int sector)
{
    synchDisk->ReadSector(sector, (char *)this);
}

//----------------------------------------------------------------------
// FileHeader::WriteBack
// 	Write the modified contents of the file header back to disk. 
//
//	"sector" is the disk sector to contain the file header
//----------------------------------------------------------------------

void
FileHeader::WriteBack(int sector)
{
    synchDisk->WriteSector(sector, (char *)this);
}

//----------------------------------------------------------------------
// FileHeader::ByteToSector
// 	Return which disk sector is storing a particular byte within the file.
//      This is essentially a translation from a virtual address (the
//	offset in the file) to a physical address (the sector where the
//	data at the offset is stored).
//
//	"offset" is the location within the file of the byte in question
//----------------------------------------------------------------------

int
FileHeader::ByteToSector(int offset)
{
    int sectorNumber = offset / SectorSize;

    IndirectDataBlock indirect;
    indirect.fetchIndirectBlocks(dataSectors[ (sectorNumber / NumIndirect) ]);

    return indirect.dataSectors[(sectorNumber % NumIndirect)];
}

//----------------------------------------------------------------------
// FileHeader::FileLength
// 	Return the number of bytes in the file.
//----------------------------------------------------------------------

int
FileHeader::FileLength()
{
    //printf("hifrom FileHeader::FileLength\n");
    int tmp = numBytes;
    //printf("hi again from FileHeader::FileLength\n");
    return tmp;
}

//----------------------------------------------------------------------
// FileHeader::Print
// 	Print the contents of the file header, and the contents of all
//	the data blocks pointed to by the file header.
//----------------------------------------------------------------------

void
FileHeader::Print()
{
    int i, j, k;
    char *data = new char[SectorSize];

    printf("FileHeader contents.  File size: %d.  File blocks:\n", numBytes);
    int indirectSectorCount = divRoundUp(numSectors,NumIndirect);
    int readSectorCount = 0;

    for (i = 0; i < indirectSectorCount; i++){
        IndirectDataBlock indirect;
        indirect.fetchIndirectBlocks(dataSectors[i]);
        for(j = 0; ((unsigned int)j < NumIndirect) && (readSectorCount < numSectors); j++){
            printf("%d ", indirect.dataSectors[j]);
        }
    }

    printf("\nFile contents:\n");
    for (int l = 0; l < indirectSectorCount; l++){
        IndirectDataBlock indirect;
        indirect.fetchIndirectBlocks(dataSectors[l]);

        for (i = k = 0; (unsigned int)i < NumIndirect && readSectorCount < numSectors; i++) {
            synchDisk->ReadSector(indirect.dataSectors[i], data);
            for (j = 0; (j < SectorSize) && (k < numBytes); j++, k++) {
                if ('\040' <= data[j] && data[j] <= '\176')   // isprint(data[j])
                    printf("%c", data[j]);
                else
                    printf("\\%x", (unsigned char)data[j]);
            }
            printf("\n"); 
            readSectorCount++;
        }
    }

    delete [] data;
}


void IndirectDataBlock::fetchIndirectBlocks(int sector){
    synchDisk->ReadSector(sector, (char *)this);
}

void IndirectDataBlock::writeIndirectBlocks(int sector){
    synchDisk->WriteSector(sector, (char *)this);
}
