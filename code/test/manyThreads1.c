#include "syscall.h"
#include "nachos_stdio.h"
#include "utility.h"

#define BIG_THREAD_NUM 1000

/*
 * Creation of large number of threads.
 * run with fun1: expect: userThreadCreate returns -2 (reached limit of number of threads)
 */

void fun1(void *ptr) {
    (void)ptr;
    //_printf("thread %d\n", UserThreadSelfId());
}


int manyThreads1(void (*fun) (void*)) {
    int i = 0;
    int tid = 0;
    for (; tid != -1 && i < BIG_THREAD_NUM; i++) {
        tid = UserThreadCreate(fun, 0);
    }
    if (tid < 0) {
        //_printf("manyThreads1 failed code %d\n", tid);
        return tid;
    }
    
    return 0;
}

int main() {
    int ret = manyThreads1(&fun1);
    _ASSERT(ret == -2);
    return 0;
}
