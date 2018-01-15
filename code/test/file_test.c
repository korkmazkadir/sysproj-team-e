
#include "nachos_stdio.h"

int main() {
        
    OpenFileId file1 = Open("test.txt");
    Write("kadirKorkmaz\0",10,file1);
    
    _printf("file id is %d\n",(int)file1);
    
    char test[10];
    Read(test,10,file1);
    
    _printf("Read is ok\n");
    
    test[9] = '\0';
    
    for(int i = 0; i < 10; i++){
        _printf("i = %d",i);
        PutChar(test[i]);
    }
    
    
    Close(file1);
    
    _printf("End of file\n");
    
}

