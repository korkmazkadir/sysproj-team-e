#include "syscall.h"
#include "utility.h"

int main() {
    char *longMessageToTransfer = "This is a long message that I am going to transfer in this test. It should be"
                                  "longer than the typical size of the package ... ";

    int connId = NetworkConnectAsServer(0);
    SynchPutInt(connId);
    PutChar('\n');
    NetworkSendToByConnId(connId, longMessageToTransfer, strlen(longMessageToTransfer));
    SynchPutString("TRANSFER FINISHED \n");

#if 0
    SynchPutString("RCVING : ... \n\n");
    char recvData[200];
    int bytesReceived = NetworkReceiveFromByConnId(connId, recvData);
    SynchPutString("BYTES RECEIVED : ");
    SynchPutInt(bytesReceived);
    PutChar('\n');
    for (int ii = 0; ii < bytesReceived; ++ii) {
        PutChar(recvData[ii]);
    }
    PutChar('\n');
#endif

    int speed = -1;
    int sendFileSystes = SendFile(connId, "step6-use", &speed);

#if 1
    SynchPutString("TRANSFER SPEED WAS : ");
    SynchPutInt(speed);
    PutChar('\n');

    SynchPutString("File send status : ");
    SynchPutInt(sendFileSystes);
    PutChar('\n');
#endif
    return 0;
}
