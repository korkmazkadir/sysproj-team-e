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

    int SendTo(int addr, int mailbox, const char *data, int len, unsigned specialMask = 0x00000000);
    unsigned ReceiveSingleChunkFrom(int mailbox, char *data, int timeout, PacketHeader *out_pktHeader = nullptr);
    int ReceiveFrom(int mailbox, char *data, PacketHeader *out_pktHeader = nullptr);

    int ConnectAsServer(int mailbox);
    int ConnectAsClient(int address, int mailbox);
    int SendToByConnId(int connId, const char *data, int len, unsigned specialMask = 0x00000000);
    int ReceiveFromByConnId(int connId, char *data);

    int SendFile(int connId, const char *fileName, int *transferSpeed = nullptr);
    int ReceiveFile(int connId, const char *fileName);
    int CloseConnection(int connId);

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

    bool checkConnClosed(int connId);
    static void periodicCloseChecker(int arg);
    int performConnClose(int connId);
//    void clearMailBox

    static const int MAX_ACK_TIMEOUT = 400000;
    static const int MAX_ATTEMPTS = 500;
    static const int MAX_FILE_CHUNK = 512;
    static constexpr const char *const TERMINATION_INDICATOR = "FILE TRANSFER FINISHED";
    static constexpr const char *const CONNECTION_CLOSE = "CONNECTION_CLOSED";

    static const unsigned HEADER_MASK        = 0x08000000u;
    static const unsigned ACK_MASK           = 0x04000000u;
    static const unsigned HEADER_ACK_MASK    = 0x02000000u;
    static const unsigned SPECIAL_FILE_END   = 0x10000000u;
    static const unsigned SPECIAL_CLOSE_CONN = 0x20000000u;

    static inline unsigned getMasked    (unsigned index,  unsigned mask ) { return (index | mask);                }
    static inline unsigned getIndex     (unsigned masked, unsigned mask ) { return (masked & (~mask));            }
    static inline unsigned getMask      (unsigned masked                ) { return (masked & 0xff000000);         }
    static inline bool     isHeader     (unsigned value                 ) { return (value & HEADER_MASK);         }
    static inline bool     isAck        (unsigned value                 ) { return (value & ACK_MASK);            }
    static inline bool     isHeaderAck  (unsigned value                 ) { return (value & HEADER_ACK_MASK);     }
    static inline bool     isClose      (unsigned value                 ) { return (value & SPECIAL_CLOSE_CONN);  }



    PostOffice *postOffice;
    NetworkAddress selfAddress;

    ConnectionID connections[NETWORK_MAX_CONNECTIONS];
    BitMap availConnections { NETWORK_MAX_CONNECTIONS };
};

#endif // SYNCHPOST_H
