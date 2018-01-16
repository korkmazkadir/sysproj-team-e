#ifndef SYNCHPOST_H
#define SYNCHPOST_H

#include <stdint.h>
#include "network.h"
#include "bitmap.h"

#define NETWORK_MAX_TRANSFER_BYTES 2048
#define NETWORK_MAX_FILE_LENGTH 128
#define NETWORK_MAX_CONNECTIONS 32
#define INIT_CONN "InitCONN"
#define SERVER_ACK "SERVER_ACK"
#define ACK_WORD "ACK"
#define HEADER_ACK "HEADER_ACK"

class PostOffice;
class SynchPost
{
public:
    SynchPost(NetworkAddress addr, double reliability, int nBoxes);
    ~SynchPost();

    int SendTo(int addr, int mailbox, const char *data, int len);
    int ReceiveSingleChunkFrom(int mailbox, char *data, int timeout, PacketHeader *out_pktHeader = nullptr);
    int ReceiveFrom(int mailbox, char *data, PacketHeader *out_pktHeader = nullptr);

    int ConnectAsServer(int mailbox);
    int ConnectAsClient(int address, int mailbox);
    int SendToByConnId(int connId, const char *data, int len);
    int ReceiveFromByConnId(int connId, char *data);

    int SendFile(int connId, const char *fileName);
    int ReceiveFile(int connId, const char *fileName);

    typedef struct ConnectionID {
        int connectionID;
        int clientAddress;
        int serverAddress;
        int mailbox;
        int tid;
        bool isServer;
    } ConnectionID;

private:

    int checkConnIdValidity(int connId) const;

    static const int MAX_ACK_TIMEOUT = 4000000;
    static const int MAX_ATTEMPTS = 30;
    static const int MAX_FILE_CHUNK = 512;


    PostOffice *postOffice;
    NetworkAddress selfAddress;

    ConnectionID connections[NETWORK_MAX_CONNECTIONS];
    BitMap availConnections { NETWORK_MAX_CONNECTIONS };
};

#endif // SYNCHPOST_H
