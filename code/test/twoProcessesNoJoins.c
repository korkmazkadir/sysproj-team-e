#include "syscall.h"
int main()
{
	ForkExec("up0");//../../build/userprog0
	ForkExec("up1");//../../build/userprog1
	return 0;
}
