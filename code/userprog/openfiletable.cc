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
}


int OpenFileTable::AddEntry(OpenFile *file, const char *fileName,  int threadId, const char *threadName){
    int index = freeMap->Find(); 
    if(index != -1){
        int inode = file->GetInode();
        openFiles[index].inode = inode;
        openFiles[index].file = file;
        openFiles[index].fileName = fileName;
        openFiles[index].threadId = threadId;
        openFiles[index].threadName = threadName;

        if(findLock(inode) == NULL){
            std::string lockName = "file lock for inode " + std::to_string(inode);
            Lock *lock = new Lock(lockName.c_str());
            inodeToLockMap[inode] = lock;
            inodeToReferenceCountMap[inode] = 1;
        }else{
            increaseReferenceCount(inode);
        }
        
    }
    return index;
}


OpenFile *OpenFileTable::getFile(int fileDescriptor){
    //Entry must be valid
    ASSERT(freeMap->Test(fileDescriptor));
    return openFiles[fileDescriptor].file;
}

Lock *OpenFileTable::getLock(int fileDescriptor){
    //Entry must be valid
    ASSERT(freeMap->Test(fileDescriptor));
    Lock *lock = findLock(openFiles[fileDescriptor].inode);
    return lock;
}

int OpenFileTable::getNumAvailable(){
    return freeMap->NumClear();
}

void OpenFileTable::RemoveEntry(int fileDescriptor){
    //Entry must be valid
    ASSERT(freeMap->Test(fileDescriptor));
    //delete openFiles[fileDescriptor].file;
    //delete openFiles[fileDescriptor].fileName;
    decreaseReferenceCount(openFiles[fileDescriptor].inode);
    freeMap->Clear(fileDescriptor);
    if(getReferenceCount(openFiles[fileDescriptor].inode) == 0){
        delete findLock(openFiles[fileDescriptor].inode);
    }
}


Lock *OpenFileTable::findLock(int inode){
    auto it = inodeToLockMap.find(inode);
    if(it != inodeToLockMap.end()){
        return inodeToLockMap[inode];
    }
    return NULL;
}

void OpenFileTable::increaseReferenceCount(int inode){
    inodeToReferenceCountMap[inode] = inodeToReferenceCountMap[inode]++;
}

void OpenFileTable::decreaseReferenceCount(int inode){
    inodeToReferenceCountMap[inode] = inodeToReferenceCountMap[inode]--;
}

int OpenFileTable::getReferenceCount(int inode){
    return  inodeToReferenceCountMap[inode];
}