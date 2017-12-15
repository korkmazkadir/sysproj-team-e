#include "syscall.h"

void print(char c, int n) {
    int i;
    for (i = 0; i < n; i++) {
        PutChar(c + i);
    }
    PutChar('\n');
    
    SynchPutString("Putting String :)\n");
}

int
main() {
    print('a', 4);
    return 33;
    //Halt();
}