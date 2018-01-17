

#include "nachos_stdio.h"

//char COMMAND_LIST[10][MAX_STRING_SIZE] = {"", "ls", "cd","mkdir","rm"};

#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

char _command[MAX_STRING_SIZE];
char _parameter[MAX_STRING_SIZE];
int _userSelection = 0;
int _backgroundProcess = 0;

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

void removeNewLine(char *string, int size){
    for (int i = 0; i < size; i++) {
        if(string[i] == '\n'){
            string[i] = '\0';
            break;
        }
    }   
}


void resetValues(){
    _userSelection = -1;
    _backgroundProcess = 0;
    clearString(_command,MAX_STRING_SIZE);
    clearString(_parameter,MAX_STRING_SIZE);
}

void printCursor(){ 
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
    if(_backgroundProcess == 0){
        UserThreadJoin(tid);
    }else{
        _printf("Running at background. PID : %d\n",tid);
    }
}

void decodeCommand(char *command){


    int commandCursor = 0;
    int parameterCursor = 0;
    
    int readIndex = 1;
    for (int i = 0; i < MAX_STRING_SIZE && command[i] != '\n'; i++) {
        char c = command[i];
        
        if(c == ' '){
            readIndex++;
            continue;
        }
        
        if(readIndex == 1){
           _command[commandCursor] = c;
           commandCursor++;
        }else if(readIndex == 2){
           _parameter[parameterCursor] = c;
           parameterCursor++;
        }else if(readIndex == 3){
            if(c == '&')
                _backgroundProcess = 1;
        }
        
    }
    
    removeNewLine(_parameter,MAX_STRING_SIZE);
    
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
            if(strlen(_command) > 0)
                _printf("%s : command not found\n",_command);
            break;
        }

    }
}

int main() {
    
    char command[MAX_STRING_SIZE];
    while (1 == 1) {
        resetValues();
        printCursor();
        _scanf("%s",command);
        decodeCommand(command);
        handleCommand();
    }

    Exit(0);
}

