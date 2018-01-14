
#include "syscall.h"
#include "nachos_stdio.h"

int main() {
    
    _printf("\n# File system test is working #\n");
    
    ListDirectoryContent(".");
    
    _printf("\nCreating directory\n");

    CreateDirectory("hello_there");
    
    _printf("\nListing directory\n");
    ListDirectoryContent(".");
    
    return 0;
}

