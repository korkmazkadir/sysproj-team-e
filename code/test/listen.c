#include "syscall.h"

int main() {

	//int result = Receive(0);
	//SynchPutInt(result);
	int con = Accept();

	if(con != -1) {
		//connection good
		int notFinish = 1;
		while(notFinish) {
			int result = Receive(0);
			if(result) {
				SynchPutString("Result of receive: ");
				SynchPutInt(result);
			}
		}
	} else {

		SynchPutString("Failed opening connection\n");
	}

	return 0;
}