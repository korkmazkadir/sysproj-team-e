

#include "syscall.h"

sem_t out_sem;

void fun(void *x) {
    int toPrint = (int)x;

    SemWait(&out_sem);
    SynchPutInt(toPrint);
    PutChar('\n');
    SemPost(&out_sem);

    int jj = 0;
    for (jj = 0; jj < 100000; ++jj) { ; }

}

int main() {    
    SemInit(&out_sem, 1);

    int tid_1 = UserThreadCreate(fun, (void *)44);
    int join_res = UserThreadJoin(tid_1);
    SemWait(&out_sem);

    SynchPutString("Join result: ");
    SynchPutInt(join_res);
    PutChar('\n');
    SemPost(&out_sem);

    SemDestroy(&out_sem);

    SynchPutString("\n >>> Loading Prog 2 ");
    int res = ForkExec("./prog_2");
    SynchPutInt(res); PutChar('\n');

    SynchPutString("\n >>> Loading Prog 1 \n");
    ForkExec("./prog_1");
    

    SynchPutString("\n >>> Loading Prog 1 \n");
    ForkExec("./prog_1");

    return 0;
}

