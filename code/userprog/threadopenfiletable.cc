/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ThreadOpenFileTable.cc
 * Author: kadir
 * 
 * Created on January 16, 2018, 7:25 PM
 */

#include "threadopenfiletable.h"

ThreadOpenFileTable::ThreadOpenFileTable() {

    freeMap = new BitMap(NumEntries);

    //Default Input file console
    fileTable[0].fileDescriptor = 0;
    fileTable[0].id = 0;
    fileTable[0].type = CONSOLE;
    freeMap->Mark(0);
  
    //Default Output file console
    fileTable[1].fileDescriptor = 1;
    fileTable[1].id = 1;
    fileTable[1].type = CONSOLE;
    freeMap->Mark(1);
}

int ThreadOpenFileTable::AddEntry(int fileDesciptor, FileType fileType){

    int index = freeMap->Find();
    fileTable[index].fileDescriptor = fileDesciptor;
    fileTable[index].id = index;
    fileTable[index].type = fileType;
    
    return index;
}


int ThreadOpenFileTable::getFileDescriptor(int threadFileDescriptor){

    if(freeMap->Test(threadFileDescriptor)){
        return fileTable[threadFileDescriptor].fileDescriptor;
    }
    return -1;
}


FileType ThreadOpenFileTable::getFileType(int threadFileDescriptor){

    if(freeMap->Test(threadFileDescriptor)){
        return fileTable[threadFileDescriptor].type;
    }
    return UNDEFINED;
}

void ThreadOpenFileTable::SetStandartInput(int fileDesciptor){
    fileTable[0].fileDescriptor = fileDesciptor;
    fileTable[0].type = NORMAL_FILE;
}

void ThreadOpenFileTable::SetStandartOutput(int fileDesciptor){
    fileTable[1].fileDescriptor = fileDesciptor;
    fileTable[1].type = NORMAL_FILE;
}
