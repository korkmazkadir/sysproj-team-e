#include "syscall.h"
#include "utility.h"

int main() {

    int connId = NetworkConnectAsClient(0, 0);
    SynchPutInt(connId);
    PutChar('\n');

    char data[200];
    int bytesReceived = NetworkReceiveFromByConnId(connId, data);
    SynchPutString("Bytes received: ");
    SynchPutInt(bytesReceived);
    PutChar('\n');

    for (int ii = 0; ii < bytesReceived; ++ii) {
        PutChar(data[ii]);
    }

    PutChar('\n');
#if 0
    const char *smallData = "SmallDatab";
    NetworkSendToByConnId(connId, smallData, strlen(smallData));
#endif

    ReceiveFile(connId, "OMG_THIS_IS_HALT.O");

    return 0;
}
