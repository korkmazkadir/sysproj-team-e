

#include "nachos_stdio.h"


void printCommandList(){
    _printf("#### Commands ######\n");
    _printf("1 - ls\n");
    _printf("2 - mkdir\n");
    _printf("3 - cd\n");
    _printf("4 - rm\n");
    
    _printf("0 - exit\n");
    
    _printf(">> ");
}

void printError(char *message){
    _printf("ERROR : %s\n",message);
}

void cmd_ls(){
    ListDirectoryContent();
}

void cmd_mkdir(){
    _printf("(create directory)Enter name of the directory : ");
    char name[MAX_STRING_SIZE];
    SynchGetString(name,MAX_STRING_SIZE);
    CreateDirectory(name);
}

void cmd_cd(){
    _printf("(chnage directory)Enter name of the directory : ");
    char name[MAX_STRING_SIZE];
    SynchGetString(name,MAX_STRING_SIZE);
    ChangeDirectory(name);
}


void cmd_rm(){
    _printf("(remove directory)Enter name of the directory : ");
    char name[MAX_STRING_SIZE];
    SynchGetString(name,MAX_STRING_SIZE);
    int result = RemoveDirectory(name);
    if(result == -2){
        printError("Can not delete, directory it is not empty.\n");
    }
}

void handleUserSelection(int userSelection){
    switch(userSelection){
        
        case 1: {
            cmd_ls();
            break;
        }
        
        case 2: {
            cmd_mkdir();
            break;
        }
        
        case 3: {
            cmd_cd();
            break;
        }
        
        case 4: {
            cmd_rm();
            break;
        }
        
        
        case 0: {
            Exit(0);
        }
        
        default :{
            _printf("---> Bad request. Try again\n");
        }
        
    }
}

int main() {
    
    int userSelection;
    while (1 == 1) {
        printCommandList();
        SynchGetInt(&userSelection);
        handleUserSelection(userSelection);
    }

    Exit(0);
}

