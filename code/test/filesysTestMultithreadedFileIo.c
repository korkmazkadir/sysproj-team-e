#include "syscall.h"
#include "nachos_stdio.h"
#include "semaphore.h"
#define BUF_SIZE 1000


/*
 * here IO is ordered: first thread runs, opens file, writes, closes.
 * If doing actual concurrent accesses, ordering of Reads/writes is undefined
 */
 
char *fileName = "myfile";
int pos = 0;
sem_t mtx;

void writer1(void *ptr) {
    char *string = (char*)ptr;
    
    OpenFileId ofid = Open(fileName);
    if (ofid == -1) {
        _printf("userprogram Open failed\n");
    }
    int numBytes = WriteAt(string, 25, ofid, 0);
    if (numBytes == -1) {
        _printf("userprogram: WriteAt failed\n");
        Exit(21);
    }
    if (Close(ofid) == -1) {
        _printf("userprogram Close failed\n");
    }
    _printf("%s",string);
}

void writer2(void *ptr) {
    char *string = (char*)ptr;
    
    OpenFileId ofid = Open(fileName);
    if (ofid == -1) {
        _printf("userprogram Open failed\n");
    }
    int numBytes = WriteAt(string, 25, ofid, 26);
    if (numBytes == -1) {
        _printf("userprogram: WriteAt failed\n");
        Exit(21);
    }
    if (Close(ofid) == -1) {
        _printf("userprogram Close failed\n");
    }
    _printf("%s",string);
    SemPost(&mtx);
}


int main() {
    int numBytes;
    char buf[BUF_SIZE];
    SemInit(&mtx, 1);

    
    //create file, write to it
    if (Create(fileName) == -1) {
        _printf("userprogram: Create failed\n");
        Exit(1);
    }
    List();
    char *string1 = "1 first writer writing 1"; 
    char *string2 = "2-SECOND-WRITER-WRITING-2"; 
    
    _printf("usrpg: Main Thread launching writers\n");
    int tid1 = UserThreadCreate(writer1, string1);
    UserThreadJoin(tid1);
    int tid2 = UserThreadCreate(writer2, string2);
    UserThreadJoin(tid2);
    //SemDestroy(&mtx);
    SemWait(&mtx);
    
    
    OpenFileId ofid = Open(fileName);
    if (ofid == -1) {
        _printf("userprogram: Open failed\n");
        Exit(2);
    }
    //read from the file    
    numBytes = ReadAt(buf, 54, ofid, 0);
    if (numBytes == -1) {
        _printf("userprogram: Read failed\n");
        Exit(3);
    }
    _printf("userprogram: buf contains %s\n", buf);
    numBytes = ReadAt(buf, 54, ofid, 26);
    if (numBytes == -1) {
        _printf("userprogram: Read failed\n");
        Exit(3);
    }
    _printf("userprogram: buf contains %s\n", buf);
    Print();
    Close(ofid);
    

    return 0;
}