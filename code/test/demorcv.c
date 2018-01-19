#include "utility.h"
#include "syscall.h"

#define BUF_SIZE 32

int main() {
    int connId = NetworkConnectAsClient(0, 0);
    int fileRcp = ReceiveFile(connId, "rcvdtxt");
    SynchPutString("File RCP: "); SynchPutInt(fileRcp); PutChar('\n');
    NetworkCloseConnection(connId);

    OpenFileId fd = Open("rcvdtxt");
    char buffer[BUF_SIZE] = { 0 };
    int numBytes = 0;
    do {
        memset(buffer, 0, sizeof(buffer));
        numBytes = Read(buffer, BUF_SIZE - 1, fd);
        if (numBytes > 0) {
            SynchPutString(buffer);
        }
    } while (numBytes > 0);
    Close(fd);

    return 0;
}
