#include "syscall.h"

int
main() {

    //char *h = "Hello there";
    
   // _printf("%d -- %s ",123,h);
    
    SynchPutString("\n\nTest of the putChar, getChar, putString, getString, putInt, getInt\n\n");

    int n;

    SynchPutString(">>Enter the number of characters : ");
    SynchGetInt(&n);

    char array[n];

    for (int i = 0; i < n; i++) {
        SynchPutString(">>Enter character : ");
        char ch = SynchGetChar();
        //To remove enter
        SynchGetChar();
        array[i] = ch;
    }

    for (int i = 0; i < n; i++) {
        SynchPutString("--->>Character from user ");
        PutChar(array[i]);
        SynchPutString("\n");
    }

    SynchPutString(">>Enter the number of strings : ");
    SynchGetInt(&n);

    char *stringArray[n];

    for (int i = 0; i < n; i++) {
        SynchPutString(">>Enter string : ");
        SynchGetString(stringArray[i],25);
        SynchPutString(stringArray[i]);
    }

    for (int i = 0; i < n; i++) {
        SynchPutString("--->>integer from user ");
        SynchPutString(stringArray[i]);
        SynchPutString("\n");
    }
    
    //ASSERT_NEW (2 > 5);


}