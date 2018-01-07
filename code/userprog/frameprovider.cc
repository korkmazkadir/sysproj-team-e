
#include "frameprovider.h"
#include "system.h"
#include <time.h>
#include <stdlib.h>

#include <cstdlib> 

FrameProvider::FrameProvider() {
    numberOfPhysicalPages = NumPhysPages;
    physicalPageBitmap = new BitMap(NumPhysPages);

    srand(time(NULL));
}

FrameProvider::~FrameProvider() {
    delete physicalPageBitmap;
}

int FrameProvider::NumAvailFrame() {
    return physicalPageBitmap->NumClear();
}

//Different allocation techniques
int FrameProvider::LinearAlloc()
{
    for (int i = 0; i < numberOfPhysicalPages; i++) {
        if(physicalPageBitmap->Test(i) == FALSE){
            physicalPageBitmap->Mark(i);
            // zero out the entire page, to zero the unitialized data segment
            //printf("Provided frame is %d \n",i);
            //bzero (&(machine->mainMemory[PageSize * i]), PageSize);
            return i;
        }
    }
    return -1;
}
int FrameProvider::RandomAlloc()
{
    int r, i = 0;
    do {
        r = rand() % numberOfPhysicalPages;

        i++;
    } while((physicalPageBitmap->Test(r) == TRUE) && (i < 1000));
    physicalPageBitmap->Mark(r);

    return r;
}
//Given the last page find the closest most consecutive one
int FrameProvider::MostConsecutiveAlloc(int lastPhysPage) {
    return physicalPageBitmap->Find();
}
//end different allocation techniques


int FrameProvider::GetEmptyFrame() {
    /*for (int i = 0; i < numberOfPhysicalPages; i++) {
        if(physicalPageBitmap->Test(i) == FALSE){
            physicalPageBitmap->Mark(i);
            // zero out the entire page, to zero the unitialized data segment
            //printf("Provided frame is %d \n",i);
            //bzero (&(machine->mainMemory[PageSize * i]), PageSize);
            return i;
        }
    }
    return -1;*/
    return FrameProvider::RandomAlloc();
}

void FrameProvider::ReleaseFrame(int frameIndex) {
    physicalPageBitmap->Clear(frameIndex);
    //printf("Released frame is %d \n",frameIndex);
}


