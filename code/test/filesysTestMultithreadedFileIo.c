#include "syscall.h"
#include "nachos_stdio.h"
#include "semaphore.h"
#define BUF_SIZE 1000

char *fileName = "myfile";
OpenFileId ofid;
int pos = 0;
//sem_t mtx;

void writer1(void *ptr) {
    char *string = (char*)ptr;
    
    //SemWait(&mtx);
    int numBytes = WriteAt(string, 26, ofid, 0);
    //SemPost(&mtx);
    
    if (numBytes == -1) {
        _printf("userprogram: WriteAt failed\n");
        Exit(21);
    }
    _printf("%s",string);
}

void writer2(void *ptr) {
    char *string = (char*)ptr;
    
    int numBytes = WriteAt(string, 26, ofid, 26);

    if (numBytes == -1) {
        _printf("userprogram: WriteAt failed\n");
        Exit(21);
    }
    _printf("%s",string);
}


int main() {
    int numBytes;
    char buf[BUF_SIZE];
    //SemInit(&mtx, 1);

    
    //create file, write to it
    if (Create(fileName) == -1) {
        _printf("userprogram: Create failed\n");
        Exit(1);
    }
    List();
    
    ofid = Open(fileName);
    if (ofid == 0) {
        _printf("userprogram: Open failed\n");
        Exit(2);
    }

    char *string1 = "1 first writer writing 1"; 
    char *string2 = "2-SECOND-WRITER-WRITING-2"; 
    _printf("usrpg: Main Thread launching writers\n");
    int tid1 = UserThreadCreate(writer1, string1);
    int tid2 = UserThreadCreate(writer2, string2);
    UserThreadJoin(tid1);
    UserThreadJoin(tid2);
    //SemDestroy(&mtx);
    
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
