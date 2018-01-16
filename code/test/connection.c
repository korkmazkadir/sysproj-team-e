#include "syscall.h"

int main() {

	int result = OpenConnection(1);
	if(result == -1) {
		SynchPutString("Failed opening connection"); 
		return -1;
	}
	else SynchPutString("Opened connection");

	int i = 0;
	while(i < 150) {
		i++;
	}

	result = Send("hey", 1);

	result  = CloseConnection(1);
	if(result == -1) {
		SynchPutString("Failed closing connection");
		return -1;
	} 
	else SynchPutString("Closed connection");
}