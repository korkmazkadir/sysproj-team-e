#include "nachos_stdio.h"



void test(void *args){
    
    char *path = (char*)args;

    int result = -1;
    
    result = ChangeDirectory(path);
    _ASSERT(result == 0);
    
    result = CreateDirectory("a");
    _ASSERT(result == 0);
    
    result = CreateDirectory("b");
    _ASSERT(result == 0);
    
    result = CreateDirectory("c");
    _ASSERT(result == 0);
    
    
    char fileName[] = "hello.txt";
    
    OpenFileId file = Open(fileName);
    _ASSERT(file > 0);
    
    
    int size = -1;
    
    char buffer[] = "Hello World :)\nI am here";
    
    size = Write(buffer, sizeof(buffer),file);
    _ASSERT(size == sizeof(buffer));
    
    Close(file);
    
}



int main() {

    
    int result = -1;
    
    char path1[] = "./dir1";
    char path2[] = "./dir2";
    char path3[] = "./dir3";
    
    _printf("%d\n",result);
    result = CreateDirectory(path1);
    _printf("%d\n",result);
    _ASSERT((result == 0));
    
    result = CreateDirectory(path2);
    _printf("%d\n",result);
    _ASSERT((result == 0));
    
    
    result = CreateDirectory(path3);
    _printf("%d\n",result);
    _ASSERT((result == 0));
    
    int tid1 = UserThreadCreate(&test, (void*)path1);
    _ASSERT(tid1 > 0);
    
    int tid2 = UserThreadCreate(&test, (void*)path2);
    _ASSERT(tid2 > 0);

    int tid3 = UserThreadCreate(&test, (void*)path3);
    _ASSERT(tid3 > 0);
    
    UserThreadJoin(tid1);
    UserThreadJoin(tid2);
    UserThreadJoin(tid3);
    
    return 0;

}

