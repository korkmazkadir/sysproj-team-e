#include "syscall.h"

void writechars(void *_) {
    //for (int i = 0; i < 100; ++i) {
	//SynchPutString("Entering user thread\n");
    PutChar(*((char*)_));
    //PutChar('\n');
    	//PutChar('\n');
    //}
}

int main() {

    //SynchPutString("starting userprog0\n");
	char a = 'a';
	char b = 'b';
    UserThreadCreate(&writechars, (void *)&a);
    UserThreadCreate(&writechars, (void *)&b);

    //SynchPutString("First tid userprog0: ");
    //SynchPutInt(pid);
    //SynchPutString("\nSecond tid userprog0: ");
    //SynchPutInt(pid2);
    //SynchPutString("\n");
    //UserThreadJoin(pid);
    //UserThreadJoin(pid2);
    //SynchPutString("end userprog0\n");
    return 0;
}