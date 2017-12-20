// frameprovider.cc 
//      Routines to manage a frameprovider.
//
#include "frameprovider.h"

//----------------------------------------------------------------------
// FrameProvider::FrameProvider
//      Initialize a frameprovider, create a bitmap with the entire physical
//      size.
//----------------------------------------------------------------------

FrameProvider::FrameProvider (int size)
{
    physPages = new BitMap(size); //change it so it uses constant
}

//----------------------------------------------------------------------
// FrameProvider::~FrameProvider
//      De-allocate a frameprovider.
//----------------------------------------------------------------------
FrameProvider::~FrameProvider ()
{
  // LB: Missing [] in delete directive
  //  delete map;
   delete physPages;
  // End of modification
}

//Different allocation techniques
/*{
    return physPages->Find();
}

int RandomAlloc()
{
    return physPages->Find();
}

//Given the last page find the closest most consecutive one
int MostConsecutiveAlloc(int lastPhysPage) {
    return physPages->Find();
}*/
//end different allocation techniques
int
FrameProvider::GetEmptyFrame ()
{
    return physPages->Find();
}

void
FrameProvider::ReleaseFrame(int addr)
{
    physPages->Clear(addr);
}

int
FrameProvider::NumFramesAvailable()
{
    return physPages->NumClear();
}