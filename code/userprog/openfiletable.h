/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   OpenFileTable.h
 * Author: kadir
 *
 * Created on January 14, 2018, 8:56 PM
 */

#ifndef OPENFILETABLE_H
#define OPENFILETABLE_H

#define NumOpenFiles 10

#include <string>
#include <map>
#include "bitmap.h"
#include "synch.h"

class OpenFileEntry{
    public:
        OpenFile *file;
        int inode;
        const char *fileName;
        int threadId;
        const char *threadName;
};


class OpenFileTable {
public:
    OpenFileTable();
    int AddEntry(OpenFile *file,  const char *fileName,  int threadId,  const char *threadName);
    OpenFile *getFile(int fileDescriptor);
    Lock *getLock(int fileDescriptor);
    int getNumAvailable();
    void RemoveEntry(int fileDescriptor);

   
private:
    //inode number & node
    std::map<int,Lock*> inodeToLockMap;
    std::map<int,int> inodeToReferenceCountMap;
    
    BitMap* freeMap;
    OpenFileEntry openFiles[NumOpenFiles];
    
    Lock *findLock(int inode);
    void increaseReferenceCount(int inode);
    void decreaseReferenceCount(int inode);
    int getReferenceCount(int inode);
};

#endif /* OPENFILETABLE_H */

