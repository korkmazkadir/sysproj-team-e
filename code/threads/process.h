/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   process.h
 * Author: kadir
 *
 * Created on December 19, 2017, 9:59 AM
 */

#ifndef PROCESS_H
#define PROCESS_H

#include "linkedlist.h"
#include "thread.h"

class Process {
public:
    
    Process(Thread *t, Process *parent);
    virtual ~Process();
    Thread * getKernelThread();
    void addChild(Process *process);
    int getPID();
    void kill();
    
    Process* findProcess(Process *rootProcess, Thread *t);
    
private:
    Process *parentProcess;
    int pid;
    Thread *kernelthread;
    LinkedList *childs;
};

#endif /* PROCESS_H */

