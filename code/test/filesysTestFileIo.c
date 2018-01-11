#include "syscall.h"
#include "nachos_stdio.h"
#define BUF_SIZE 1500

int main() {
     Exit(99);
  
  /*
    
    char buf[BUF_SIZE];
    OpenFileId ofid;
    int numBytes;
    

//create file, write 10 bytes to it
    if (Create("myfile") == -1) {
        _printf("userprogram: Create failed\n");
        Exit(1);
    }
    //List();
    ofid = Open("myfile");
    if (ofid == 0) {
        _printf("userprogram: Open failed\n");
        Exit(2);
    }
    _printf("user program: ofid is %d\n", ofid);
    numBytes = Write("0123456789", 10, ofid);
    if (numBytes == -1) {
        _printf("userprogram: Write failed\n");
        Exit(3);
    }
    if (Close(ofid) != -1) {
        _printf("userprogram: Close failed\n");
        Exit(4);
    }
    
//read 10 bytes from the file
    ofid = Open("myfile");
    numBytes = Read(buf, 10, ofid);
    if (numBytes == -1) {
        _printf("userprogram: Read failed\n");
        Exit(5);
    }
    _printf("userprogram: expect 0123456789 buf contains %s\n", buf);
    Close(ofid);
    
    
//write 2 bytes at a specific location
    ofid = Open("myfile");
    if (ofid == 0) {
        _printf("userprogram: Open failed\n");
        Exit(6);
    }
    numBytes = WriteAt("hi", 2, ofid, 2);
    if (numBytes == -1) {
        _printf("userprogram: WriteAt failed\n");
        Exit(7);
    }
    numBytes = ReadAt(buf, 10, ofid, 0);
    if (numBytes == -1) {
        _printf("userprogram: ReadAt failed\n");
        Exit(8);
    }
    _printf("userprogram: expect 01hi456789 buf contains %s\n", buf);
    Close(ofid);

//read 2 bytes from a specific location

    return 0;*/
}
