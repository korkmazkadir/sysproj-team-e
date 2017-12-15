
#include "nachos_stdio.h"


int
main() {

    //char *h = "Hello there";
    
    const char c1 = 'c';
    const char c2 = 'b';
    
    
    
    _printf("Kadir % korkmaz : %c %c \n",c1,c2);
    
    //_printf("%c -- %c \n", c1, c2);

    char *str = "Hello World\n\0";
    
    _printf("String : %s \0", str);
    
    
    PutChar(c1);
    PutChar(c2);
    
    ASSERT_NEW (2 > 5);

}
