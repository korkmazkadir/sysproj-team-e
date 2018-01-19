#include "syscall.h"
//./nachos-step5 -f -cp twoProcessesFirstOneJoins tpf -cp userprog0join up0j -cp userprog1 up1 -rs 1 -x tpf
int main()
{
	ForkExec("up0j");//userprog0join
	ForkExec("up1");//userprog1
	return 0;
}
