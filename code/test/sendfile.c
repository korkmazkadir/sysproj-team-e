#include "syscall.h"

int main() {

	OpenFileId file = Open ("transfer.txt");

	if(file) {
		SynchPutString("Opened file\n");
	} else {
		SynchPutString("Failed to open file\n");
	}
}