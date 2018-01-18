
#include "nachos_stdio.h"

int compareString(char *str1, char *str2){
    int i = 0;
    for (i = 0; i < MAX_STRING_SIZE; i++) {
        char ch1 = str1[i];
        char ch2 = str2[i];
        
        if(ch1 != ch2){
            return -1;
        }else if(ch1 == '\0'){
            return 0;
        }
    }
    
    return 0;
}


OpenFileId outFile;
OpenFileId inFile;

void writerThread(void *_){
    _printf("Writer is working :)\n");
    char ch = '1';
    int i = 0;
    for(i = 0; i < 1000; i++ ){
        Write(&ch,1,outFile);
    }
}


void readerThread(void *_){
    _printf("Writer is working :)\n");
    char ch = 1;
    int i = 0;
    for(i = 0; i < 1000; i++ ){
        Read(&ch,1,inFile);
        _printf("%c",ch);
    }
}



int main() {
    outFile = Open("output");
    inFile = Open("output");
    
    if(outFile < 0){
        _printf("Error at out file opening. Error %d\n",outFile);
        Exit(-1);
    }
    


    int tidWriter = UserThreadCreate(&writerThread, 0);
    int tidReader = UserThreadCreate(&readerThread, 0);
    _printf("Reader thread tid %d\n", tidReader);
    _printf("writer thread tid %d\n", tidWriter);
    
   
    UserThreadJoin(tidWriter);
    UserThreadJoin(tidReader);


    Close(outFile);
    Close(inFile);
    
    _printf("End of File System Test 1\n");
    return 0;
}

