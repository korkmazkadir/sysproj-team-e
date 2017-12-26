
#include "syscall.h"

void beingFun(void *param) {
    SynchPutString("Being Funny "); SynchPutInt((int)param); PutChar('\n');
}

int main() {
    
    int tid = UserThreadCreate(beingFun, (void *)998);
    SynchPutString("\nprog 2 is working\n");
    UserThreadJoin(tid);
    return 85;
}

