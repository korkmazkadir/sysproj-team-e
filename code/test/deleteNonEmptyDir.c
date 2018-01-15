#include "syscall.h"
#include "nachos_stdio.h"
#define BUF_SIZE 15
/*
 * Try to delete a non-empty directory, then make it empty and try again
 * Excpect: doesn't delete first time (is still shown by List(). Is deleted second time.
 */
sem_t mtx;
OpenFileId ofid;
void func(void *ptr) {
    //SemWait(&mtx);
    _printf("opener opening file.\n");
    ofid = Open("bla");
    if (ofid == 0) Exit(4);
    _printf("opener closing file.\n");
    if (Close(ofid) == -1) Exit(5);
    _printf("opener removing file.\n");
    if (Remove("bla") == -1) Exit(6);
    //SemPost(&mtx);
}
int main() {
    char buf[BUF_SIZE];
    (void) buf;
    //trying mkdir and rmdir
   /* if (Mkdir("newDir") == -1) {
        _printf("user: Mkdir failed\n");
    }
    List();
    _printf("\n");
    Chdir("newDir");
    Mkdir("subDir");
    List();
    _printf("\n");
    Chdir("..");
    if (Rmdir("newDir") == -1) {
        _printf("user: rmdir failed\n");
    }
    List(); //here the Rmdir should have failed
    _printf("Rmdir of newDir has run shouldve failed\n");
    Chdir("newDir");
    Rmdir("subDir");
    List();
    _printf("\n");
    Chdir("..");
    _printf("\nremoving newDir should work:\n");
    if (Rmdir("newDir") == -1) {
        _printf("user: rmdir failed\n");
    }
    List(); //here the Rmdir should have succeeded*/
    _printf("\n\n do open on marked for removal");
    
    if (Create("bla") == -1) Exit(1);
    ofid = Open("bla");
    if (ofid == 0) Exit(2);
    Write("hello!", 6, ofid);
    _printf("should fail:");
    int tid = UserThreadCreate(func, 0);
    UserThreadJoin(tid);
    if (Close(ofid) == -1) Exit(3);
    _printf("closed the file. should now be removed. next open shouldn't find file:");
    tid = UserThreadCreate(func, 0);
    UserThreadJoin(tid);
    return 0;
}
