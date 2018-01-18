#include "syscall.h"
int main()
{
	ForkExec("up0");//../../build/userprog0
	ForkExec("up1j");//../../build/userprog1join
	return 0;
}
