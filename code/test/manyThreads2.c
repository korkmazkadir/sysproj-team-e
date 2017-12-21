#include "syscall.h"
#include "nachos_stdio.h"
#include "utility.h"

#define BIG_THREAD_NUM 10000

/*
 * manyThreads2: Creation of large number of threads.
 * run with fun2: expect: userThreadCreate returns -3 (there is no space left on stack for a new thread)
 */
//numIter is intended to be enough so that nachOS runs out of space for thread stacks (threads stay around rather than completing too quickly)
//if this test fails, first think of increasing it.

void fun2(void *ptr) {
    (void)ptr;
    //_printf("thread %d\n", UserThreadSelfId());
    int numIter = 10000;
    while(numIter > 0) {
        numIter--;
    }
}

int test1(void (*fun) (void*)) {
    int i = 0;
    int tid = 0;
    for (; tid != -1 && i < BIG_THREAD_NUM; i++) {
        tid = UserThreadCreate(fun, 0);
    }
    if (tid < 0) {
        //_printf("manyThreads2 failed code %d\n", tid);
        return tid;
    }
    return 0;
}

int main() {
    int ret = test1(&fun2);
    _ASSERT(ret == -3);
    return 0;
}
