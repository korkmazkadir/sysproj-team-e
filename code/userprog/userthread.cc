#include "userthread.h"
#include "thread.h"
#include "system.h"

typedef struct S_data {
    VoidFunctionPtr f;
    int arg;
} Data;

static void StartUserThread(int f) {
    Data *data = (Data*)f;
    currentThread->space->InitRegisters ();	// set the initial register values
    currentThread->space->RestoreState ();	// load page table register
    
    machine->WriteRegister(PCReg, (int)data->f);
    machine->WriteRegister(NextPCReg, 4+(int)data->f);
    machine->WriteRegister(4, (int)data->arg);
    //printf("sp = %d\n", mainSp);
    mainSp = mainSp - (currentThread->tid+1) * (2*PageSize);
    //printf("sp = %d\n", mainSp);
    machine->WriteRegister(StackReg, mainSp);
    machine->Run ();		// jump to the user progam
    ASSERT (FALSE);	
}

int do_UserThreadCreate(int f, int arg) {
    //TODO: check if ptrs are valid?
    Data *data = (Data*)malloc(sizeof(struct S_data));
    if (data == NULL) {
        printf("Malloc failed in do_UserThreadCreate");
        return -1;
    }
    if (mainSp - 3*PageSize < 0) {
        return -1;
    }
    l_tT->P();
    if (totalThreads >= MAX_NUM_THREADS) {
        printf("thread limit reached\n");
        l_tT->V();
        return -1;
    } 
    else {
        l_tT->V();
    }
    data->f = (VoidFunctionPtr) f;
    data->arg = arg;
    Thread *newThread = new Thread("newThread");
    newThread->Fork(StartUserThread, (int)data);
    return newThread->tid;
}

void do_UserThreadExit() {
    //if space only has one thread using it left, post semaphore
    //this allows intial thread to run Halt
    currentThread->space->l_nbThreads->P();
    currentThread->space->nbThreads--;
    if (currentThread->space->nbThreads == 1) {
        //printf("TEAME posting halt sema\n");
        currentThread->space->spaceInUse->V();
    }
    currentThread->space->l_nbThreads->V();
    currentThread->Finish();
}

int do_UserThreadJoin(int tid) {
    //nothing
    return 5;
}
