#include "syscall.h"
int main()
{
	ForkExec("./userprog0");
	ForkExec("./userprog1");
	//SynchPutString("end test\n");
	return 0;
}