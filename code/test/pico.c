
#include "nachos_stdio.h"

int compareString(char *str1, char *str2){
    for (int i = 0; i < MAX_STRING_SIZE; i++) {
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

int strlen(char *str){
    int length = 0;
    char c = str[length];
    while(c != '\0'){
        length++;
         c = str[length];
    }
    return length;
}

void removeNewLine(char *string, int size){
    for (int i = 0; i < size; i++) {
        if(string[i] == '\n'){
            string[i] = '\0';
            break;
        }
    }   
}


int openFile(){
    _printf("Enter the name of the file : ");
    
    char name[MAX_STRING_SIZE];
    SynchGetString(name,MAX_STRING_SIZE);
    
    removeNewLine(name,MAX_STRING_SIZE);
    
    OpenFileId fileDescriptor = Open(name);

    if (fileDescriptor < 0) {
        _printf("Can not open file. Error code %d\n", fileDescriptor);
        Exit(-1);
    }
    
    return fileDescriptor;
}


int lineCount = 1;
char line[MAX_STRING_SIZE];
void writeToFile(int file){
    while(1 == 1){
        _printf("%d ",lineCount);
        SynchGetString(line,MAX_STRING_SIZE);
        if(compareString(":q\n",line) == 0){
            Close(file);
            return;
        }
        Write(line,strlen(line), file);
        lineCount++;
    }
}


int main() {
    
    _printf("\nNano Text Editor\n");
    int file = openFile();
    writeToFile(file);
  
    _printf("End of nano\n");
}

