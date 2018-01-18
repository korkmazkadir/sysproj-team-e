#include "syscall.h"

int globalInt = 0;
static int t1;
static int t2 = -1;


void fun1(void *_) {
    (void)_;
//    SynchPutString("Hello1\n");
    for (int ii = 0; ii < 10000; ++ii) {
        ++globalInt;
    }
    PutChar('\n');
//    SynchPutInt(*((int *)_));
    SynchPutInt((int)_);
//    UserThreadJoin(2);
    PutChar('\n');
}


void fun2(void *_) {

    UserThreadJoin((int)_);
    SynchPutString("2Hello\n");
    int ii = 0;
    for (ii = 0; ii < 1000; ++ii) {
        globalInt += (int)_;
    }


}


int main()
{

#if 1
    SynchPutInt((int)&fun1);
    SynchPutString("\nAddress of main \n");
    SynchPutInt((int)&main);
    SynchPutString("\nAddress of Halt \n");
    SynchPutInt((int)&Halt);
    SynchPutString("\nAddress of Exit \n");
    SynchPutInt((int)&Exit);

    SynchPutString("\nAddress of UTExit \n");
    SynchPutInt((int)&UserThreadExit);
    PutChar('\n');

    t1 = UserThreadCreate(&fun1, (void *)&t2);
    t2 = UserThreadCreate(&fun2, (void *)t1);
    SynchPutString("Second thread ID: ");
    SynchPutInt(t2);
    PutChar('\n');

    SynchPutString("First thread ID: ");
    SynchPutInt(t1);
    PutChar('\n');

//    UserThreadJoin(t2);

    SynchPutString("Global int value: ");
    SynchPutInt(globalInt);
    PutChar('\n');
    PutChar('\n');

#endif

#if 0
    (void)t1;
    (void)t2;
    int jj = 0;
    for (jj = 0; jj < 1000; ++jj) {
        UserThreadCreate(&fun1, (void *)55);
    }
#endif


    return 0;
}

