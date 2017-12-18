#include "syscall.h"

#define _ASSERT(expr) if (!(expr)) aFailed(__FILE__, __LINE__)

#define MAX_STRING_SIZE 99

void aFailed(char *fileName, int line) {
    AssertionFailed(fileName, line);
}


int stringCompare(char *str1, char *str2){
    for(int i = 0; i < MAX_STRING_SIZE ; i++){
        if(str1[i] == str2[i] ){
            if(str1[i] == '\0'){
                return 0;
            }
        }
    }
    return -1;
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