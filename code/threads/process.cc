/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   process.cc
 * Author: kadir
 * 
 * Created on December 19, 2017, 9:59 AM
 */

#include "process.h"



static int PID_COUNTER = 0;
static int producePID(){
    PID_COUNTER++;
    return PID_COUNTER;
}


Process::Process(Thread *t, Process *parent) {
    pid = producePID();
    kernelthread = t;
    childs = new LinkedList();
    if(parent != NULL){
        parentProcess = parent;
        parentProcess->childs->add(this);
    }
}


Thread* Process::getKernelThread(){
    return kernelthread;
}

void Process::addChild(Process *process){
    childs->add(process);
}

int Process::getPID() {
    return pid;
}


void Process::kill() {
    //
    //
    //Kill All childs
    //Kill current process
    //
    //
}

//
//Recursivly looks for process
//
Process* Process::findProcess(Process *rootProcess, Thread *t){
    
    Process *process = NULL;
    
    int numChild = rootProcess->childs->getSize();
    for(int i = 0; i < numChild ; i++){
        Process *p = (Process*)rootProcess->childs->get(0);
        if(p->kernelthread == t){
            process = p;
        }
        process = p->findProcess(p,t);
    }
    
    return process;
}


Process::~Process() {
    
}


