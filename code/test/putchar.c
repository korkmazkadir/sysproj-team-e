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
    print('p', 2);
    /*
    char c = SynchGetChar();
    PutChar(c);
    (void)SynchGetChar();
    SynchPutString("Enter data!\n");


    static char test[10] = { 0 };
    SynchGetString(&test[0], 10);
    SynchPutString("\n");
    SynchPutString(test);
    SynchPutString("\n");

    SynchPutString("Enter integer: ");
    int n;
    SynchGetInt(&n);
    SynchPutString("Your input was ");
    SynchPutInt(n);
    SynchPutString("\n");*/
    int a = 5;
    int b = 2;
    int c = a + b;
    for (;c>0;c--){};

}
