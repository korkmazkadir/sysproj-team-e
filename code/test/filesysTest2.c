#include "syscall.h"
#include "nachos_stdio.h"
#define BUF_SIZE 15

int main() {    
    /*Mkdir("newDir2");
    Chdir("newDir2");
    Mkdir("newSubDir");
    Chdir("..");

    //multi-level hops path names
    _printf("\n####   ---- multi-level hops, paths\n");
    List();
    Chdir("./newDir2/newSubDir");
    _printf("chdir to ./newDir2/newSubDir done!\n");
    List();
    _printf("\n");
    Chdir("../..");
    _printf("chdir to ../.. (should lead to /) done!\n");
    List();
    _printf("\n");*/
    
    Create("loltxt");
    OpenFileId wid = Open("loltxt");
    OpenFileId rid = Open("loltxt");
    Write("hello world!", 15, wid);
    char buf[15];
    Read(buf, 15, rid);
    Close(wid);
    Close(rid);
    SynchPutString(buf);
    Remove("loltxt");
    List();
    
    /*
    
    //sniper mkdir and rmdir (long distance)
    _printf("sniper mkdir and rmdir (long distance(very far(multiple directories awaaaay...)))\n");
    Mkdir("./newDir2/newSubDir/target");
    _printf("Mkdir of ./newDir2/newSubDir/target done!\n");
    List();
    _printf("\n");
    
    _printf("Performing recon to find target\n");
    Chdir("./newDir2/newSubDir");
    List();
    _printf("\n");
    Chdir("../..");
    _printf("\n");
    
    Rmdir("./newDir2/newSubDir/target");
    _printf("Target neutralized(killed(dir removed!))\n\n");
    _printf("Moving to ./newDir2/newSubDir to confirm kill\n");
    
    Chdir("./newDir2/newSubDir");
    List();
    _printf("\n");
    _printf("KILL CONFIRMED\n");*/
    return 0;
}
