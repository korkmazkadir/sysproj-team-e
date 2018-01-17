#include "syscall.h"
#include "utility.h"

int main() {
    int connId = NetworkConnectAsServer(0);
    SynchPutString("Conn ID: "); SynchPutInt(connId); PutChar('\n');

    const char *data = "Hello world";
    NetworkSendToByConnId(connId, data, strlen(data));
    int status = NetworkCloseConnection(connId);
    SynchPutString("Conn closed with status: "); SynchPutInt(status); PutChar('\n');

    connId = NetworkConnectAsServer(0);
    SynchPutString("Conn ID: "); SynchPutInt(connId); PutChar('\n');

    NetworkSendToByConnId(connId, data, strlen(data));
    status = NetworkCloseConnection(connId);
    SynchPutString("Conn closed with status: "); SynchPutInt(status); PutChar('\n');
}
