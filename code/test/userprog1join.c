#include "syscall.h"

void writechars(void *_) {
    PutChar(*((char*)_));
}

int main() {
	char a = 'c';
	char b = 'd';
    int pid = UserThreadCreate(&writechars, (void *)&a);
    int pid2 = UserThreadCreate(&writechars, (void *)&b);
    UserThreadJoin(pid);
    UserThreadJoin(pid2);
    return 0;
}