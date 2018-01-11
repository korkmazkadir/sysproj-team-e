#include "syscall.h"
#include "nachos_stdio.h"
#define BUF_SIZE 15

/*
 * Try to delete a non-empty directory, then make it empty and try again
 * Excpect: doesn't delete first time (is still shown by List(). Is deleted second time.
 */
 
int main() {
    char buf[BUF_SIZE];


    (void) buf;
    
    //trying mkdir and rmdir
    if (Mkdir("newDir") == -1) {
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
    _printf("Rmdir of newDir has run\n");
    
    Chdir("newDir");
    Rmdir("subDir");
    List();
    _printf("\n");
    
    Chdir("..");
    if (Rmdir("newDir") == -1) {
        _printf("user: rmdir failed\n");
    }
    List(); //here the Rmdir should have succeeded
    _printf("\n");

    
    
    
    


    return 0;
}
