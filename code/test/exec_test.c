

#include "syscall.h"

int main(int argc, char** argv) {

    SynchPutString("\n >>> Loading Prog 2 \n");
    ForkExec("./prog_2");

    SynchPutString("\n >>> Loading Prog 1 \n");    
    ForkExec("./prog_1");
    

    SynchPutString("\n >>> Loading Prog 1 \n");
    ForkExec("./prog_1");
   
    while(1);
    
}

