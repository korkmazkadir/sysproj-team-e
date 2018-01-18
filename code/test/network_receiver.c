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

    int ii = 0;
    for (ii = 0; ii < bytesReceived; ++ii) {
        PutChar(data[ii]);
    }

    PutChar('\n');
#if 0
    const char *smallData = "SmallDatab";
    NetworkSendToByConnId(connId, smallData, strlen(smallData));
#endif

    int fileStatus = ReceiveFile(connId, "OMG.O");
    SynchPutInt(fileStatus);
    PutChar('\n');
    //NOTE: This assert CAN happen and is NORMAL
    //      If checker sees it, it should just skip file comparison part
    _ASSERT(fileStatus == 0);
    return 0;
}
