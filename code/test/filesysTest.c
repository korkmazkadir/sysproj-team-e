#include "syscall.h"
#include "nachos_stdio.h"
#define BUF_SIZE 15

int main() {
    char buf[BUF_SIZE];

    /*Create("myfile");
    OpenFileId ofid = Open("myfile");
    Read(buf, 10, ofid);
    Write(buf, 10, ofid);
    Close(ofid);*/
    (void) buf;
    
    //trying mkdir and rmdir
    if (Mkdir("newDir") == -1) {
        _printf("user: rmdir failed\n");
    }
    List();
    if (Rmdir("newDir") == -1) {
        _printf("user: rmdir failed\n");
    }
    _printf("mkdir and rmdir done!\n\n");
    List();
    
    /*
    // Single level hops, directory names
    _printf("\n####  ---- Single level hops, directory names\n");

    Mkdir("newDir2");
    _printf("mkdir newDir2 done!\n");
    List();
    _printf("\n");
    
    Chdir("newDir2");
    _printf("chdir to newDir2 done!\n");
    List();
    _printf("\n");

    Mkdir("newSubDir");
    _printf("mkdir newSubDir done!\n");
    List();
    _printf("\n");
    
    Chdir("newSubDir");
    _printf("chdir to newSubDir done!\n");
    List();
    _printf("\n");
    
    _printf("now go back up the tree\n\n");
    
    Chdir("..");
    List();
    _printf("\n");
    
    Chdir("..");
    List();
    _printf("\n");
*/
    return 0;
}
