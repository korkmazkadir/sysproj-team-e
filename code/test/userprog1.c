#include "syscall.h"

void writechars(void *_) {
    //for (int i = 0; i < 100; ++i) {
	//SynchPutString("Entering user thread\n");
    PutChar(*((char*)_));
    PutChar('\n');
    	//PutChar('\n');
    //}
}

int main() {

    SynchPutString("starting userprog1\n");
	char a = 'c';
	char b = 'd';
    int pid = UserThreadCreate(&writechars, (void *)&a);
    int pid2 = UserThreadCreate(&writechars, (void *)&b);

    UserThreadJoin(pid);
    UserThreadJoin(pid2);
    SynchPutString("end userprog1\n");
    return 0;
}