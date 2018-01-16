#include "syscall.h"
#include "nachos_stdio.h"
#include "semaphore.h"

#define BUF_SIZE 500
sem_t mtx, created;

void t1(void *ptr) {
    SemWait(&mtx);
    Chdir("subDir1");
    _printf("1  Chdir(subDir1) :\n");
    List();
    _printf("1\n\n");
    SemPost(&mtx);
    
    SemWait(&mtx);
    if (Mkdir("t1SubDir") == -1) Exit(2);
    _printf("1  Mkdir(t1SubDir) :\n");
    List();
    _printf("1\n\n");
    SemPost(&mtx);
    
    SemWait(&created);
    SemWait(&mtx);
    Chdir("../subDir2/t2SubDir");
    _printf("1  Chdir(t2SubDir) :\n");
    List();
    _printf("1\n\n");
    SemPost(&mtx);
}

void stayAndWrite(void *ptr) {
    SemWait(&mtx);
    Chdir("subDir2");
    _printf("2 Chdir(subDir2) :\n");
    List();
    _printf("2\n\n");
    SemPost(&mtx);
    
    SemWait(&mtx);
    if (Mkdir("t2SubDir") == -1) Exit(11);
    SemPost(&created);
    _printf("2  Mkdir(t2SubDir) :\n");
    List();
    _printf("2\n\n");
    SemPost(&mtx);
}

void stayAndReadWrite(void *ptr) {
    char buf[BUF_SIZE];
    int pos = 0;
    int numBytes = 0;
    OpenFileId SARData, sd11f;

    if (Create("SARData") == -1) Exit(30);

    if ((SARData = Open("SARData")) == -1) Exit(31);


    //don't change dirs, just read the file
    if ((sd11f = Open("sd11file")) == -1) Exit(32);
    SemPost(&mtx);

    for (int i = 0; i < 10000; i++) {
        numBytes = ReadAt(buf, 15, sd11f, pos);
        if (numBytes == -1) {
            _printf("ReadAt fail pos %d\n", pos);
            Exit(33);
        }
        //_printf("hey,");
        numBytes = WriteAt(buf, 15, SARData, pos);
        if (numBytes == -1) Exit(34);
        //_printf("YOU!\n");
        pos = (pos + 15) % 30;
    }
        
}

void moveAndModifyDirs(void *ptr) {
    Chdir("..");
    if (Rmdir("subDir33") == -1) Exit(40);
    Chdir("../dir1");
    if (Mkdir("newSubDir12") == -1) Exit(41);
    
    if (Rmdir("subDir21") == -1) {
        _printf("mamd Rmdir of subDir21 failed");
    }
    else {
        _printf("mamd Rmdir of subDir21 success");
    }
}

void moveAndModifyFiles(void *ptr) {
    Chdir("subDir11");
    if (Close(5) != -1) {
        _printf("mamf Close should have failed, and didn't\n");
        Exit(51);
    }
    OpenFileId ofid = Open("sd11f");
    if (ofid == -1) Exit(52);
    int numBytes = WriteAt("mvModFiles passing through...", 30, ofid, 30);
    if (numBytes == -1) Exit(53);
    SemWait(&mtx);
    if (Remove("sd11f") == -1) Exit(54);
    if (Close(ofid) == -1) Exit(55);
    
}


int main() {
    SemInit(&mtx, 0);
    
    if (Mkdir("dir1") == -1) Exit(1);
    if (Mkdir("dir2") == -1) Exit(2);
    if (Mkdir("dir3") == -1) Exit(3);
    if (Mkdir("dir1/subDir11") == -1) Exit(4);
    if (Mkdir("dir2/subDir21") == -1) Exit(5);
    if (Mkdir("dir2/subDir22") == -1) Exit(6);
    if (Mkdir("dir3/subDir31") == -1) Exit(7);
    if (Mkdir("dir3/subDir32") == -1) Exit(8);
    if (Mkdir("dir3/subDir33") == -1) Exit(9);
    _printf("Main made directory structure\n");
    List();
    _printf("\n");
    
    //making files
    if (Create("dir1/d1file") == -1) Exit(10);
    OpenFileId d1f;
    if ((d1f = Open("dir1/d1file")) == -1) Exit(11);
    if (Create("dir1/subDir11/sd11file") == -1) Exit(12);
    OpenFileId sd11f;
    if ((sd11f = Open("dir1/subDir11/sd11file")) == -1) Exit(13);
    
    //writing into files and closing
    char *buf = "this should be sixty chars long so i'm writing random stuff!";
    int numBytes = Write(buf, 60, d1f);
    if (numBytes == -1) Exit(14);
    buf = "THIS-SHOULD-BE-SIXTY-CHARS-LONG-SO-I,M-WRITING-RANDOM-STUFF?";
    numBytes = Write(buf, 60, sd11f);
    if (numBytes == -1) Exit(15);
    
    if (Close(d1f) == -1) Exit(16);
    if (Close(sd11f) == -1) Exit(17);
    
    _printf("Main Setup complete\n");
    //Print();
    _printf("\n\n");
    
    
    
    _printf("Main launching threads\n");
    Chdir("dir1/subDir11");
    int tid1 = UserThreadCreate(stayAndReadWrite, 0);
    Chdir("../../dir3/subDir33");
    int tid2 = UserThreadCreate(moveAndModifyDirs, 0);
    Chdir("../../dir2");
    int tid3 = UserThreadCreate(moveAndModifyFiles, 0);
    
    UserThreadJoin(tid1);
    UserThreadJoin(tid2);
    UserThreadJoin(tid3);
    _printf("\nmain\n");
    Print();
    
    return 0;
}
