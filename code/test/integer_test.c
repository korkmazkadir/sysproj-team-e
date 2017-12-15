#include "syscall.h"

int
main() {

    int number = -3;
    SynchPutInt(number);
    SynchGetInt(&number);
    SynchPutInt(number);
    
}