#include "syscall.h"

#define _ASSERT(expr) if (!(expr)) aFailed(__FILE__, __LINE__)

#define MAX_STRING_SIZE 99

void aFailed(char *fileName, int line) {
    AssertionFailed(fileName, line);
}


int stringCompare(const char *s1, const char *s2)
{
    for ( ; *s1 == *s2; s1++, s2++)
    if (*s1 == '\0')
        return 0;
    return ((*(unsigned char *)s1 < *(unsigned char *)s2) ? -1 : +1);
}

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
