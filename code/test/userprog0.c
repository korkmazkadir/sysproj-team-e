#include "syscall.h"

void writechars(void *_) {
    //for (int i = 0; i < 100; ++i) {
        PutChar(*((char*)_));
    	//PutChar('\n');
    //}
}

int main() {

	char a = 'a';
	char b = 'b';
	UserThreadCreate(&writechars, (void *)&a);
    UserThreadCreate(&writechars, (void *)&b);

}