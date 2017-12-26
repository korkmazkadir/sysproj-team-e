
#include "frameprovider.h"
#include "system.h"

#include <cstdlib> 

FrameProvider::FrameProvider() {
    numberOfPhysicalPages = NumPhysPages;
    physicalPageBitmap = new BitMap(NumPhysPages);
}

FrameProvider::~FrameProvider() {
    delete physicalPageBitmap;
}

int FrameProvider::NumAvailFrame() {
    return physicalPageBitmap->NumClear();
}


int FrameProvider::GetEmptyFrame() {
    for (int i = 0; i < numberOfPhysicalPages; i++) {
        if(physicalPageBitmap->Test(i) == FALSE){
            physicalPageBitmap->Mark(i);
            // zero out the entire page, to zero the unitialized data segment
            //printf("Provided frame is %d \n",i);
            bzero (&(machine->mainMemory[PageSize * i]), PageSize);
            return i;
        }
    }
    return -1;
}

void FrameProvider::ReleaseFrame(int frameIndex) {
    physicalPageBitmap->Clear(frameIndex);
    //printf("Released frame is %d \n",frameIndex);
}


