#include "syscall.h"
int main()
{
	ForkExec("./userprog0");
	ForkExec("./userprog0");

	return 0;
}