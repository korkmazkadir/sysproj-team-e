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
        }else{
            SynchPutInt(str1[i]);
            PutChar('H');
            SynchPutInt(str2[i]);
            return -1;
        }
    }
    return -1;
}

int
main() {
    
    char ch1 = 'a';
    char ch2 = 'b';
    char ch3 = 'k';
    
    int val1 = -123;
    int val2 = 0;
    int val3 = 99999;
        
    char *str1 = "Lorem Ipsum is simply dummy text of the printing and typesetting industry.[]{} \n";
    char *str2 = "2384727834rıurfjernfekrjnjnjjh000==94309509304593045^+%&%+^&%&/&%/ \n";
    
    
    PutChar(ch1);
    PutChar(ch2);
    PutChar(ch3);
    
    PutChar('\n');
    
    SynchPutInt(val1);
    PutChar('\n');

    SynchPutInt(val2);
    PutChar('\n');
    
    SynchPutInt(val3);
    PutChar('\n');
    
    
    SynchPutString(str1);
    
    SynchPutString(str2);
    
    
    char ch;
    
    ch = SynchGetChar();
    _ASSERT(ch == ch1);

    ch = SynchGetChar();
    _ASSERT(ch == ch2);
    
    ch = SynchGetChar();
    _ASSERT(ch == ch3);
    
    SynchGetChar();//To remove enter
    
    
    int val;
    
    SynchGetInt(&val);
    _ASSERT(val == val1);
    
    
    SynchGetInt(&val);
    _ASSERT(val == val2);
    
    SynchGetInt(&val);
    _ASSERT(val == val3);
    
    
    char str[MAX_STRING_SIZE];
    SynchGetString(str,MAX_STRING_SIZE); 
    _ASSERT(stringCompare(str,str1) == 0);    
    
    SynchGetString(str,MAX_STRING_SIZE);
    _ASSERT(stringCompare(str,str2) == 0);
    

    Exit(0);
    
}