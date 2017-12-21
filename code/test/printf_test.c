
#include "nachos_stdio.h"


int
main() {

    const char c1 = 'Q';
    const char c2 = 'W';
    
    char * str = "Kadir  korkmaz : %c ------- %c --- #%d# ----  #%d#  #%d# \n";
    
    _printf(str,c2,c1,12,-133,-2345);
    _printf("%d Kadir Korkmaz %s", 1234, "Hello There :) \n");
    _printf("Address of C1 : %d \n", &c1);
    _printf("Address of C2 : %d \n", &c2);
    _printf("%c -- %d -- %s -- %b\n", 'a', 0, "K.Korkmaz", 7);

    _ASSERT(3 > 5);
    
}
