/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   frameprovider.h
 * Author: kadir
 *
 * Created on December 18, 2017, 2:20 PM
 */

#ifndef FRAMEPROVIDER_H
#define FRAMEPROVIDER_H

#include "bitmap.h"

class FrameProvider {
public:
    FrameProvider();
    ~FrameProvider();
    
    //Provides first empty physical page
    int GetEmptyFrame();
    void ReleaseFrame(int frameIndex);
    int NumAvailFrame();
    
private:
    int numberOfPhysicalPages;
    BitMap *physicalPageBitmap;
};

#endif /* FRAMEPROVIDER_H */

