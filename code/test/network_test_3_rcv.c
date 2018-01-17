#include "syscall.h"
#include "utility.h"

int main() {
    int connId = NetworkConnectAsClient(0, 0);
    char data[32] = { 0 };
    int bytesReceived = NetworkReceiveFromByConnId(connId, data);

    SynchPutString("Bytes received: "); SynchPutInt(bytesReceived); PutChar('\n');
    SynchPutString("Data received: "); SynchPutString(data); PutChar('\n');


    for (int ii = 0; ii < 5000000; ++ii) { ; }

    return 0;
}
