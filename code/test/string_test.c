#include "syscall.h"

int
main() {

    
    
    
    char buffer[100];
    
    SynchPutString("Kadir Korkmaz\n");
    
    SynchGetString(buffer, 99);
    
    SynchPutString(buffer);
    

}