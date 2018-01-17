#include "syscall.h"

void openCloseThread(void *ptr) {
    //int l = *(int *)ptr;

    int result;

    int con = OpenConnection(1);
	if(con == -1) {
		SynchPutString("Failed opening first connection of thread\n"); 
		return;
	}
	else SynchPutString("Opened connection first\n");

	result = Send("Hola baby!! te escribia para saber como te va en esta tarde tan bonita, NOT", con);

	if(!result) {
		SynchPutString("Failed sending first package\n");
	} else {
		SynchPutString("Achieved sending the first message\n");
	}

	result  = CloseConnection(con);
	if(result == -1) {
		SynchPutString("Failed closing first connection\n");
		return;
	} 
	else SynchPutString("Closed first connection\n");
}

int main() {

	//int result;
	
	int pid = UserThreadCreate(&openCloseThread, (void *)0);
	int pid2 = UserThreadCreate(&openCloseThread, (void *)1);

	UserThreadJoin(pid);
	UserThreadJoin(pid2);

	return 0;

}