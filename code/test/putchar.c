#include "syscall.h"

void print(char c, int n)
{
    int i;
    for (i = 0; i < n; i++) {
        PutChar(c+i);
    }
    PutChar('\n');
}

int main()
{
//    char c = SynchGetChar();
//    PutChar(c);

    SynchPutString("Enter data!\n");

    static char test[10] = { 0 };
    SynchGetString(&test[0], 5);
    SynchPutString("\n");
    SynchPutString(test);
    SynchPutString("\n");


    Halt();
}
