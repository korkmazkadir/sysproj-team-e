

#include "nachos_stdio.h"

//char COMMAND_LIST[10][MAX_STRING_SIZE] = {"", "ls", "cd","mkdir","rm"};

#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

char _command[MAX_STRING_SIZE];
char _parameter[MAX_STRING_SIZE];
int _userSelection = 0;


int strlen(char *str){
    int count = 0;
    for (int i = 0; i < MAX_STRING_SIZE; i++) {
        if(str[i] != '\0'){
            count++;
        }else{
            break;
        }
    }
    
    return count;
}


void printCursor(){
    _userSelection = -1;
    _printf(ANSI_COLOR_CYAN);
    _printf("NashOS >> ");
    _printf(ANSI_COLOR_RESET);
    
}


void printError(char *message){
    _printf("ERROR : %s\n",message);
}

void cmd_ls(){
    int size = strlen(_parameter);
    if(size != 0){
        ListDirectoryContent(_parameter);
    }else{
        ListDirectoryContent(".");
    }
}

void cmd_mkdir(){
    CreateDirectory(_parameter);
}


void cmd_cd(){
    ChangeDirectory(_parameter);
}


void cmd_rm(){
    int result = RemoveDirectory(_parameter);
    if(result == -2){
        printError("Can not delete, directory it is not empty.\n");
    }
}

void cmd_run(){
    int tid = ForkExec(_parameter);
    UserThreadJoin(tid);
}


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

void clearString(char *string, int size){
    for (int i = 0; i < size; i++) {
        string[i] = '\0';
    }   
}


void decodeCommand(char *command){
    clearString(_command,MAX_STRING_SIZE);
    clearString(_parameter,MAX_STRING_SIZE);

    int commandCursor = 0;
    int parameterCursor = 0;
    
    int isCommand = 1;
    for (int i = 0; i < MAX_STRING_SIZE && command[i] != '\n'; i++) {
        char c = command[i];
        
        if(c == ' '){
            isCommand = 0;
            continue;
        }
        
        if(isCommand){
           _command[commandCursor] = c;
           commandCursor++;
        }else{
           _parameter[parameterCursor] = c;
           parameterCursor++;
        }
    }
    

    if(compareString("ls",_command) == 0){
        _userSelection = 1;
    }else if(compareString("cd",_command) == 0){
        _userSelection = 2;
    }else if(compareString("mkdir",_command) == 0){
        _userSelection = 3;
    }else if(compareString("rm",_command) == 0){
        _userSelection = 4;
    }else if(compareString("run",_command) == 0){
        _userSelection = 5;
    }else if(compareString("clear",_command) == 0){
        _userSelection = 6;
    }else if(compareString("exit",_command) == 0){
        Exit(0);
    }

}



void handleCommand(){
    
    switch(_userSelection){
        
        case 1: {
            cmd_ls();
            break;
        }
        
        case 2: {
            cmd_cd();
            break;
        }
        
        case 3: {
            cmd_mkdir();
            break;
        }
        
        case 4: {
            cmd_rm();
            break;
        }
        
        case 5: {
            cmd_run();
            break;
        }
        
        case 6: {
            _printf("\033[H\033[J");
            break;
        }
        
        case 0: {
            Exit(0);
        }
        
        default : {
            _printf("%s : command not found\n",_command);
            break;
        }

    }
}

int main() {
    
    char command[MAX_STRING_SIZE];
    while (1 == 1) {
        printCursor();
        SynchGetString(command,MAX_STRING_SIZE);
        decodeCommand(command);
        handleCommand();
    }

    Exit(0);
}

