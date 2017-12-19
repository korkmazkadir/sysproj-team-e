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

#define BASE_10 10
#define BASE_2 2

#define END_OF_LINE '\0'

#define _ASSERT(expr) if (!(expr)) aFailed(__FILE__, __LINE__)


typedef unsigned char *va_list;
#define va_start(list, param) (list = (((va_list)&param) + sizeof(param)))
#define va_arg(list, type)    (*(type *)((list += 4) - 4))


void aFailed(char *fileName, int line) {
    AssertionFailed(fileName, line);
}

int copyString(char *src, char *dest) {

    int index = 0;
    dest[index] = src[index];
    while (dest[index] != END_OF_LINE) {
        index++;
        dest[index] = src[index];
    }

    return index;
}

int getStartIndex(int number, int base){
    int div = number / base;
    
    int index = 0;
    while(div > 0){
        div  = div / base;
        index++;
    }
    
    return index;
}

void numberToString( int number, int base, char *stringBuffer){
    
    int index;
    
    if(number < 0){
        number = number * -1;
        // +1 for the minus sign
        index = getStartIndex(number, base) + 1;
        stringBuffer[index + 1] = END_OF_LINE;
        stringBuffer[0] = '-';
    }else{
        index = getStartIndex(number, base);
        stringBuffer[index + 1] = END_OF_LINE;
    }
    
    while(number >= 0){
       int remainder = number % base;
       number = number / base;
       stringBuffer[index] = '0' + remainder;

       if (0 == number) {
           break;
       }

       index--;
    }
    
}


//Implemented for %c, %s, %d %b
void _printf(char *format, ...) {

    //DummyFunction("kadirkorkmaz",'c','w',1,2);
    
    char buffer[MAX_STRING_SIZE];

    va_list args;
    va_start(args, format);

    
    int index = 0;
    int writeIndex = 0;
    while (1) {

        char ch = format[index];
        
        if (ch == '%' && format[index + 1] == 'c') {

            char c = va_arg(args, char);
            buffer[writeIndex] = c;
            index++;

        }else if (ch == '%' && format[index + 1] == 's') {
            
            char *c = va_arg(args, char*);
            int characterCount = copyString(c,&buffer[writeIndex]);
            writeIndex += characterCount - 1;
            index++;
            
        }else if (ch == '%' && format[index + 1] == 'd') {
            char numberStringBuffer[MAX_STRING_SIZE];
            
            int value = va_arg(args, int);
            numberToString(value,BASE_10,numberStringBuffer);
            int characterCount = copyString(numberStringBuffer,&buffer[writeIndex]);
            writeIndex += characterCount - 1;
            index++;

        }else if (ch == '%' && format[index + 1] == 'b') {
            char numberStringBuffer[MAX_STRING_SIZE];
            
            int value = va_arg(args, int);
            numberToString(value,BASE_2,numberStringBuffer);
            int characterCount = copyString(numberStringBuffer,&buffer[writeIndex]);
            writeIndex += characterCount - 1;
            index++;

        }else {

            buffer[writeIndex] = ch;
            if (ch == END_OF_LINE) {
                break;
            }
        }

        index++;
        writeIndex++;

    }
     
    SynchPutString(buffer);
}

#endif /* NACHOS_STDIO_H */
