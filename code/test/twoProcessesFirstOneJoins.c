#include "syscall.h"
int main()
{
	ForkExec("../../build/userprog0join");
	ForkExec("../../build/userprog1");
	return 0;
}