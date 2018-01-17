

#include "nachos_stdio.h"


int main(int argc, char** argv) {
    
    _printf("Args count : %d\n",argc);
    
    
    for(int i = 0; i < argc ; i++){
        _printf("arg %d = %s\n",i+1,argv[i]);
    }
    
    _printf("End of args\n");
    
}


