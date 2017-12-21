#ifndef USERTHREAD_H
#define USERTHREAD_H

#include "synch.h"

class Thread;
class AddrSpace;
typedef void (*ThreadFun_t)(void *);

typedef struct ThreadParam_t {
    ThreadFun_t functionPtr;
    void *functionParam;
    AddrSpace *space;
    int retAddress;
    int topOfStack;
} ThreadParam_t;

typedef struct ThreadDescriptor_t {
    Thread *threadPtr;
    int argPtr;
    Semaphore *synch;
    bool waitingForJoin;

    ThreadDescriptor_t(): threadPtr(0), argPtr(0), synch(0), waitingForJoin(false) {
        synch = new Semaphore("t", 0);
    }

    ThreadDescriptor_t(const ThreadDescriptor_t &) = delete;
    ThreadDescriptor_t &operator =(const ThreadDescriptor_t &) = delete;

    ~ThreadDescriptor_t() {
        delete synch;
    }
} ThreadDescriptor_t;

int do_UserThreadCreate(int funPtr, int arg, int retAddress, AddrSpace *space, bool kernelRequest = false);
int do_KernelThreadCreate(AddrSpace *space);
void do_UserThreadExit();
int do_UserThreadJoin(int tid);

extern Semaphore haltSync;


#endif // USERTHREAD_H

