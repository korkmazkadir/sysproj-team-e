#include "syscall.h"
int main()
{
	ForkExec("../../build/userprog0");
	ForkExec("../../build/userprog1");
	return 0;
}