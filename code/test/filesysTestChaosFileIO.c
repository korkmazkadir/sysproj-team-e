#include "syscall.h"
#include "nachos_stdio.h"
#include "semaphore.h"
#define BUF_SIZE 1000


/*
 * Write to file using implicit position, expect chaos
 */
 
char *fileName = "myfile";

void writer1(void *ptr) {
    char *string = (char*)ptr;
    
    OpenFileId ofid = Open(fileName);
    if (ofid == -1) {
        _printf("userprogram Open failed\n");
    }
    int pos = 0;
    for (int i = 0; i < 8; i++) {
        int numBytes = WriteAt(string, 3, ofid, pos);
        if (numBytes == -1) {
            _printf("userprogram: WriteAt failed\n");
            Exit(21);
        }
        pos+=3;
        //_printf("1 writing 1\n");
    }
    if (Close(ofid) == -1) {
        _printf("userprogram Close failed\n");
    }
    //_printf("%s",string);
}

void writer2(void *ptr) {
    char *string = (char*)ptr;
    
    OpenFileId ofid = Open(fileName);
    if (ofid == -1) {
        _printf("userprogram Open failed\n");
    }
    int pos = 100;
    for (int i = 0; i < 8; i++) {
        int numBytes = WriteAt(string, 3, ofid, pos);
        if (numBytes == -1) {
            _printf("userprogram: WriteAt failed\n");
            Exit(21);
        }
        pos+=3;
        //_printf("2-WRITING-2\n");

    }
    if (Close(ofid) == -1) {
        _printf("userprogram Close failed\n");
    }
    //_printf("%s",string);
}


int main() {
    int numBytes;
    char buf[BUF_SIZE];
    
    //create file, write to it
    if (Create(fileName) == -1) {
        _printf("userprogram: Create failed\n");
        Exit(1);
    }
    List();

    char *string1 = "1-1"; 
    char *string2 = "2-2"; 
    
    _printf("usrpg: Main Thread launching writers\n");
    int tid1 = UserThreadCreate(writer1, string1);
    int tid2 = UserThreadCreate(writer2, string2);
    
    int i;
    for (i = 0; i < 4; i++) {        
        OpenFileId ofid = Open(fileName);
        if (ofid == -1) {
            _printf("userprogram: Open failed\n");
            Exit(2);
        } 
        //read from the file    
        numBytes = ReadAt(buf, 100, ofid, 0);
        if (numBytes == -1) {
            _printf("userprogram: Read failed\n");
            Exit(3);
        }
        //_printf("userprogram: buf contains %s\n", buf);
        SynchPutString("\nbuf contains:");
        SynchPutString(buf);
        
        numBytes = ReadAt(buf, 100, ofid, 100);
        if (numBytes == -1) {
            _printf("userprogram: Read failed\n");
            Exit(3);
        }
        //_printf("userprogram: buf contains %s\n", buf);  
        SynchPutString("\nbuf contains:");
        SynchPutString(buf);
        Close(ofid);
    
    }
    UserThreadJoin(tid1);
    UserThreadJoin(tid2);


    //Print();

    return 0;
}
