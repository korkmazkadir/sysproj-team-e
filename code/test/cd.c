
#include "syscall.h"
#include "nachos_stdio.h"

int main() {
    
    _printf("Name of the directory to go : ");

    char name[MAX_STRING_SIZE];
    SynchGetString(name,MAX_STRING_SIZE);
    
    ChangeDirectory(name);
    ListDirectoryContent();

    SynchGetString(name,MAX_STRING_SIZE);
    int result = CreateDirectory(name);
    _printf("mkdir result : %d\n", result);
    
    
    Exit(0);
}

