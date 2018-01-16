#include "syscall.h"

int main() {
	char buffer[100];
	//int result = Receive(0);
	//SynchPutInt(result);

	int con = Accept();

	if(con != -1) {
		//connection good
		int notFinish = 1;
		while(notFinish) {
			Receive(con, 0, buffer);
			/*if(result) {
				SynchPutString("Result of receive: ");
				SynchPutInt(result);
			}*/
		}
	} else {

		SynchPutString("Failed opening connection\n");
	}

	return 0;
}