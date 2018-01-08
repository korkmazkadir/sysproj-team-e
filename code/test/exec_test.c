

#include "nachos_stdio.h"

int main(int argc, char** argv) {

    _printf("\n >>> Prog 2 \n");

    ForkExec("/home/kadir/Git/sysproj-team-e/code/build/prog_2");

    _printf("\n >>> Prog 1 \n");
    
    ForkExec("/home/kadir/Git/sysproj-team-e/code/build/prog_1");
    
    
    _printf("\n >>> Prog 1 \n");

    
    ForkExec("/home/kadir/Git/sysproj-team-e/code/build/prog_1");
    
    while(1);
    
}

