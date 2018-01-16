#include "syscall.h"

int main() {
	int result;
	int con = OpenConnection(1);
	int con2 = OpenConnection(1);
	if(con == -1) {
		SynchPutString("Failed opening first connection"); 
		return -1;
	}
	else SynchPutString("Opened connection first ");

	if(con2 == -1) {
		SynchPutString("Failed opening second connection"); 
		return -1;
	}
	else SynchPutString("Opened connection second");

	int i = 0;
	while(i < 150) {
		i++;
	}

	result = Send("hey", con);

	if(!result) {
		SynchPutString("Failed sending first package\n");
	} else {
		SynchPutString("Achieved sending the first message\n");
	}

	int con3 = OpenConnection(1);

	if(con3 == -1) {
		SynchPutString("Failed opening second connection"); 
		return -1;
	}
	else SynchPutString("Opened connection second");

	result = Send("hey2", con2);

	if(!result) {
		SynchPutString("Failed sending second package\n");
	}

	result  = CloseConnection(con);
	if(result == -1) {
		SynchPutString("Failed closing first connection");
		return -1;
	} 
	else SynchPutString("Closed first connection");

	result  = CloseConnection(con2);
	if(result == -1) {
		SynchPutString("Failed closing second connection");
		return -1;
	} 
	else SynchPutString("Closed second connection");

	result  = CloseConnection(con3);
	if(result == -1) {
		SynchPutString("Failed closing third connection");
		return -1;
	} 
	else SynchPutString("Closed third connection");
}
