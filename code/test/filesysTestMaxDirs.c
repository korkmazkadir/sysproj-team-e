#include "syscall.h"
#include "nachos_stdio.h"
#define BUF_SIZE 15

int main() {
    
    _printf("\n####  ---- make 8 directories in newDir2\n");

    Mkdir("newDir2");
    _printf("mkdir newDir2 done!\n");
    List();
    _printf("\n");
    
    Chdir("newDir2");
    _printf("chdir to newDir2 done!\n");
    List();
    _printf("\n");

    Mkdir("subDir1");
    _printf("mkdir newSubDir done!\n");
    List();
    _printf("\n");
    
    Mkdir("subDir2");
    _printf("mkdir newSubDir done!\n");
    List();
    _printf("\n");

    Mkdir("subDir3");
    _printf("mkdi newSubDir done!\n");
    List();
    _printf("\n");
    
    Mkdir("subDir4");
    _printf("mkdir newSubDir done!\n");
    List();
    _printf("\n");
    
    Mkdir("subDir5");
    _printf("mkdir newSubDir done!\n");
    List();
    _printf("\n");
    
    Mkdir("subDir6");
    _printf("mkdir newSubDir done!\n");
    List();
    _printf("\n");
    
    Mkdir("subDir7");
    _printf("mkdir newSubDir done!\n");
    List();
    _printf("\n");
    
    Mkdir("subDir8");
    _printf("mkdir newSubDir done!\n");
    List();
    _printf("\n");
    
    Mkdir("subDir9");
    _printf("mkdir newSubDir done!\n");
    List();
    _printf("\n");
    
    
    List();
    _printf("\n");
    Chdir("subDir1");
    List();
    _printf("\n");
    return 0;
}
