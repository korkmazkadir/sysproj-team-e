/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   nachos_stdio.h
 * Author: kadir
 *
 * Created on December 14, 2017, 10:42 PM
 */

#ifndef NACHOS_STDIO_H
#define NACHOS_STDIO_H

#include "syscall.h"

#define MAX_STRING_SIZE 99

#define ASSERT_NEW(expr) if (!(expr)) aFailed(__FILE__, __LINE__)



//void aFailed(char *fileName, int line);

//void _printf(char *format, ...);

void aFailed(char *fileName, int line) {
    AssertionFailed(fileName, line);
}

int copyString(char *src, char *dest){

    int index = 0;
    dest[index] = src[index];
    while(dest[index] != '\0'){
        index++;
        dest[index] = src[index];
        PutChar(dest[index]);
    }
    
    return index;
}

void _printf(char *format, ...) {

    char buffer[MAX_STRING_SIZE];
    void *argListPointer =   (void*)format + 1 ;

    //%c, %d
    int index = 0;
    int writeIndex = 0;
    char ch = format[index];
    while (1) {
        
        if(ch == '\0'){
            break;
        }
        
        if (ch == '%' && format[index + 1] == 'c') {
            
            buffer[writeIndex] = *((char*) argListPointer);
            index++;
            argListPointer = argListPointer + 1;
            
        }else if (ch == '%' && format[index + 1] == 's') {
            
            int copiedStrSize =  copyString( (char*)argListPointer , &buffer[writeIndex] );
            writeIndex += copiedStrSize;
            
            index++;
            argListPointer = (void*)argListPointer + 1;
            
        }
        else {
            
            buffer[writeIndex] = ch;
            if (ch == '\0') {
                break;
            }
            
        }
        index++;
        writeIndex++;
        ch = format[index];
        
    }

    SynchPutString(buffer);
}

#endif /* NACHOS_STDIO_H */

