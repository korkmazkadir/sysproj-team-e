#include "syscall.h"

void printStuff(void *string) {
    char *s = (char*)string;
    SynchPutString(s);
    Yield();
    SynchPutString("another one\n");
    /*int i = 0;
    while(1) {
        i++;
    }*/
    UserThreadExit();
}

int main() {
    SynchPutString("Hello, yes this is main");
    
    UserThreadCreate(printStuff, "weeeeeee!\n");
    //Yield();
    
    PutChar('x');
    PutChar('y');
    PutChar('z');
    int i;
    int ret = 0;
    for (i = 0; ret != -1 && i < 500; i++) {
        ret = UserThreadCreate(printStuff, "Yeeeeeehaw!\n");
        SynchPutInt(ret);PutChar('\n');
    }
    if (ret == -1) {
        SynchPutString("prog: Could not allocate thread:");
        SynchPutInt(i);
        PutChar('\n');
    }
    SynchPutInt(i);
    //Yield();
    Halt(); 
    PutChar('p');
    PutChar('m');
    PutChar('l');  
    
    //Yield();
    
    PutChar('a');
    PutChar('b');
    PutChar('c'); 
    //Yield();


}

