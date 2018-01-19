#include "syscall.h"
#include "utility.h"

#define MAX_STRING_SIZE 99

int
main() {
    
    char buffer[MAX_STRING_SIZE];
    SynchGetString(buffer,MAX_STRING_SIZE);
    
    
    char *expectedInput = "Test 1\n";
    _ASSERT(stringCompare(buffer,expectedInput) == 0);
   
    
    int val;
    SynchGetInt(&val);
    _ASSERT(val == 32);
    
    SynchGetInt(&val);
    _ASSERT(val == -456);
    
    char ch = SynchGetChar();
    _ASSERT(ch == 'a');
    SynchGetChar();//Remove new line
    
    ch = SynchGetChar();
    _ASSERT(ch == 'b');
    SynchGetChar();//Remove new line
    
    ch = SynchGetChar();
    _ASSERT(ch == 'c');
    SynchGetChar();//Remove new line
    
    ch = SynchGetChar();
    _ASSERT(ch == 'd');
    SynchGetChar();//Remove new line
    

    Exit(0);
    
}
