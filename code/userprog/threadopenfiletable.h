/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ThreadOpenFileTable.h
 * Author: kadir
 *
 * Created on January 16, 2018, 7:25 PM
 */

#ifndef THREADOPENFILETABLE_H
#define THREADOPENFILETABLE_H

#define NumEntries 10

#include "bitmap.h"


enum FileType { UNDEFINED ,NORMAL_FILE, CONSOLE };

class TableEntry{
    public:
        int id;
        int fileDescriptor;
        FileType type;
};

class ThreadOpenFileTable {
public:
    ThreadOpenFileTable();
    int AddEntry(int fileDesciptor, FileType fileType);
    int getFileDescriptor(int threadFileDescriptor);
    FileType getFileType(int threadFileDescriptor);
    void SetStandartInput(int fileDesciptor);
    void SetStandartOutput(int fileDesciptor);
    
private:
    BitMap *freeMap;
    TableEntry fileTable[NumEntries];
};

#endif /* THREADOPENFILETABLE_H */

