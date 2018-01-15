#include "syscall.h"
#include "nachos_stdio.h"
#include "semaphore.h"
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

void t2(void *ptr) {
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

int main() {
    SemInit(&mtx, 1);
    SemInit(&created, 0);
    
    if (Mkdir("subDir1") == -1) Exit(20);
    if (Mkdir("subDir2") == -1) Exit(21);
    List();
    
    _printf("usrpg: Main launching writers\n");
    int tid1 = UserThreadCreate(t1, 0);
    int tid2 = UserThreadCreate(t2, 0);
    
    UserThreadJoin(tid1);
    UserThreadJoin(tid2);
    _printf("\nmain\n");
    Chdir("subDir1");
    
    SemDestroy(&mtx);
    return 0;
}
