// frameprovider.h 
//      Data structure defining a FrameProvider -- contains all possible physical pages
//      and determines it's availability or not.
//      Every physical page is represented by a bit in the bitmap physpages.
//

#ifndef FRAMEPROVIDER_H
#define FRAMEPROVIDER_H

#include "bitmap.h"


class FrameProvider
{
  public:
    FrameProvider (int size); //allocate Frame provider of size 
     ~FrameProvider ();		// De-allocate frameprovider

    int GetEmptyFrame ();	//get addr of empty frame
    void ReleaseFrame(int addr); //
    int NumFramesAvailable();

  private:
    BitMap *physPages;

};

#endif // FRAMEPROVIDER_H