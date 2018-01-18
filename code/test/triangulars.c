
#include "nachos_stdio.h"


void calculateTrinangulars(int count){
    int i = 0;
    for(i = 0; i < count ; i++){
        int nthNumber = 0;
        int j = 0;
        for(j = 0; j <= i; j++){
            nthNumber += j;
        }
        
        _printf("%d -> %d\n",i,nthNumber);
    }
    
}


int main() {
    
    
    _printf("\n# Canculating Triangular Numbers #\n");
    
    OpenFileId out = Open("out.txt");
    OUTPUT_FILE = out;
    
    calculateTrinangulars(100);
    
    return 0;
}

