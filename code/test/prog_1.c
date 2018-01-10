
#include "syscall.h"

int main() {
    
    SynchPutString("\nprog 1 is working\n");
    int ii = 0;
    for (ii = 0; ii < 100000; ++ii) { ; }
    return 84;
}

