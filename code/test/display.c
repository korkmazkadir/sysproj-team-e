
#include "nachos_stdio.h"

int compareString(char *str1, char *str2) {
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

void removeNewLine(char *string, int size) {
    int i = 0;
    for (i = 0; i < size; i++) {
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

    if (fileDescriptor < 0 || fileDescriptor > 10) {
        _printf("Can not open file. Error code %d\n", fileDescriptor);
        Exit(-1);
    }
  
    return fileDescriptor;
}


int lineCount = 1;
char line[MAX_STRING_SIZE];
void readFromFile(int file){
    memset(line, 0, sizeof(line));
    int size = Read(line,MAX_STRING_SIZE, file);
    _printf("%s\n",line);
    while(size == MAX_STRING_SIZE){
        memset(line, 0, sizeof(line));
        size = Read(line,MAX_STRING_SIZE, file);
        _printf("%s",line);
       lineCount++;
    }
}

int main() {
    _printf("\n\nDisplay Text Viewer\n\n");
    int file = openFile();
    _printf("\n_______________%s_______________\n",name);
    readFromFile(file);
    _printf("\n______________________________________\n",name);
    return 0;
}

