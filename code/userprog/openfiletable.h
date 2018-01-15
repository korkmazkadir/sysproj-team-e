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

#include "bitmap.h"




class OpenFileEntry{
    public:
        OpenFile *file;
        const char *fileName;
        int threadId;
        const char *threadName;
};


class OpenFileTable {
public:
    OpenFileTable();
    int AddEntry(OpenFile *file,  const char *fileName,  int threadId,  const char *threadName);
    OpenFile *getFile(int fileDescriptor);
    int getNumAvailable();
    void RemoveEntry(int fileDescriptor);

    
private:
    BitMap* freeMap;
    OpenFileEntry openFiles[NumOpenFiles];
    
};

#endif /* OPENFILETABLE_H */

