#include "syscall.h"
#include "nachos_stdio.h"
#include "utility.h"

#define NUM_THREADS 4
#define MAX_STRING_SIZE 99

/*
 * earlyHalt: Halt main thread before created threads end, 
 * expect: created threads still run to completion, then program terminates ( NUM_THREADS hellos printed )
 */
int tids[NUM_THREADS];



void fun(void *_) {
    char *expect = "hello\n";
    char got[MAX_STRING_SIZE];
    (void)_;
    int numIter = 10000;
    while(numIter > 0) {
        numIter--;
    }
    SynchPutString("hello");
    SynchGetString(got, MAX_STRING_SIZE);
    int v = stringCompare(got, expect);
    _ASSERT( v == 0);
}

int main()
{
    char *expect = "hello";
    char got[MAX_STRING_SIZE];
    
    /*SynchPutString("hello");
    SynchGetString(got, MAX_STRING_SIZE);
    int v = stringCompare(got, expect);
    _ASSERT( v == 0);*/

    
    tids[0] = 1;
    int i = 0;
    for (; i < NUM_THREADS; i++) {
        tids[i] = UserThreadCreate(&fun, 0);
        if (tids[i] <= 0) {
            _printf("earlyHalt unexpected error. UTC return %d\n", tids[i]);
        }
    }
    Halt();
    int ret = 0;
    for (i = 0; i < NUM_THREADS; i++) {
        ret = UserThreadJoin(tids[i]);
        if (ret < 0) {
            _printf("ThreadsCreateThreads unexpected error. UTJ return %d\n", ret);
        }
    }
    
    for (i = 0; i < NUM_THREADS; i++) {
        SynchGetString(got, MAX_STRING_SIZE);
        int v = stringCompare(got, expect);
        _ASSERT( v == 0);
    }
    Halt();

    return 0;
}

