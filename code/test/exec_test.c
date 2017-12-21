

#include "syscall.h"

sem_t out_sem;

void fun(void *x) {
    int toPrint = (int)x;

    SemWait(&out_sem);
    SynchPutInt(toPrint);
    PutChar('\n');
    SemPost(&out_sem);
}

int main() {

    SemInit(&out_sem, 1);

    int tid_1 = UserThreadCreate(fun, (void *)44);
    int join_res = UserThreadJoin(tid_1);
    SemWait(&out_sem);

    SynchPutInt(join_res);
    PutChar('\n');
    SemPost(&out_sem);



    SynchPutString("\n >>> Loading Prog 2 \n");
    ForkExec("./prog_2");

    SynchPutString("\n >>> Loading Prog 1 \n");    
    ForkExec("./prog_1");
    

    SynchPutString("\n >>> Loading Prog 1 \n");
    ForkExec("./prog_1");

    while(1);
    
}

