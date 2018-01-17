
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


char name[MAX_STRING_SIZE];
int openFile(){
    _printf("Enter the name of the file : ");

    _scanf("%s",name);
    
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
void readFromFile(int file){
    int size = Read(line,MAX_STRING_SIZE, file);
    _printf("%s\n",line);
    while(size == MAX_STRING_SIZE){
        size = Read(line,MAX_STRING_SIZE, file);
        _printf("%s",line);
       lineCount++;
    }
}

int main() {
    int file = openFile();
    _printf("\n_______________%s_______________\n",name);
    readFromFile(file);
    _printf("\n______________________________________\n",name);
}

