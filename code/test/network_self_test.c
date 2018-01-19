#include "syscall.h"
#include "utility.h"

static void fun_1(void *arg) {
    (void)arg;
    int connId = NetworkConnectAsServer(0);
    const char *dataToSend = "Test Data";
    NetworkSendToByConnId(connId, dataToSend, strlen(dataToSend));
}

static void fun_2(void *arg) {
    int connId = NetworkConnectAsClient(0, 0);
    char data[32] = { 0 };
    NetworkReceiveFromByConnId(connId, data);
    SynchPutString("RECEIVED DATA ");
    SynchPutString(data);
    PutChar('\n');
}

int main () {
    int tid_1 = UserThreadCreate(fun_1, 0);
    int tid_2 = UserThreadCreate(fun_2, 0);

    UserThreadJoin(tid_1);
    UserThreadJoin(tid_2);

    return 0;
}
