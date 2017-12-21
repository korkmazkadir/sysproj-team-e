#include "syscall.h"
#include "nachos_stdio.h"

/*
 * nullFuncP: pass NULL function pointer to UserThreadCreate
 * expect: UserThreadCreate returns -1
 */

int nullFuncP(void (*fun) (void*)) {
    int tid = 0;
    tid = UserThreadCreate(fun, 0);
    
    if (tid < 0) {
        //_printf("nullFuncP failed code %d\n", tid);
        return tid;
    }
    
    return 0;
}


int main() {
    int ret = nullFuncP(0);
    _ASSERT(ret == -1);
    return 0;
}
