#include "syscall.h"
#include "nachos_stdio.h"
#include "utility.h"

#define NUM_THREADS 2

/*
 * userThreadExit: check threads terminate on UserThreadExit call
 * expect: print nothing
 */
//got big data ?   make it global! otherwise overflow thread user stack! :D
int tids[NUM_THREADS];
int globalFlag = 0;

void fun2(void *_) {
    (void)_;
    int numIter = 10000;
    while(numIter > 0) {
        numIter--;
    }
    _printf("hello\n");
}



void fun(void *_) {
    (void)_;
    //Threads should not go past this point
    UserThreadExit();
    _printf("userThreadExit failed, ran past call to UserThreadExit");
    globalFlag = 1;
    _ASSERT(0);
}

int main()
{   
    int i = 0;
    for (; tids[i] >= 0 && i < NUM_THREADS; i++) {
        tids[i] = UserThreadCreate(&fun, 0);
    }
    --i;
    if (tids[i] < 0) {
        _printf("ThreadsCreateThreads unexpected error. UTC return %d\n", tids[i]);
        return -1;
    }
    
    int ret = 1;
    int j = 0;
    for (; ret > 0 &&  j < NUM_THREADS; j++) {
        ret = UserThreadJoin(tids[j]);
    }
    _ASSERT(globalFlag == 0);
    return 0;
}

