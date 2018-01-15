/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   OpenFileTable.cc
 * Author: kadir
 * 
 * Created on January 14, 2018, 8:56 PM
 */

#include "openfiletable.h"

OpenFileTable::OpenFileTable() {
    freeMap = new BitMap(NumOpenFiles);
    printf("Open File Table is ready\n");
}


int OpenFileTable::AddEntry(OpenFile *file, const char *fileName,  int threadId, const char *threadName){
    int index = freeMap->Find(); 
    if(index != -1){
        openFiles[index].file = file;
        openFiles[index].fileName = fileName;
        openFiles[index].threadId = threadId;
        openFiles[index].threadName = threadName;
    }
    return index;
}


OpenFile *OpenFileTable::getFile(int fileDescriptor){
    //Entry must be valid
    ASSERT(freeMap->Test(fileDescriptor));
    return openFiles[fileDescriptor].file;
}

int OpenFileTable::getNumAvailable(){
    return freeMap->NumClear();
}

void OpenFileTable::RemoveEntry(int fileDescriptor){
    //Entry must be valid
    ASSERT(freeMap->Test(fileDescriptor));
    //delete openFiles[fileDescriptor].file;
    //delete openFiles[fileDescriptor].fileName;
    freeMap->Clear(fileDescriptor);
}