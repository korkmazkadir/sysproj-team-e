#include "SynchPost.h"

#include <stdlib.h>
#include <string>
#include <chrono>

#include "copyright.h"

#include "system.h"
#include "network.h"
#include "post.h"
#include "interrupt.h"
#include "filesys.h"

#define HEADER_WORD "HEADER"
#define CONN_CLOSE_RETVAL (-30)

SynchPost::SynchPost(NetworkAddress addr, double reliability, int nBoxes):
    selfAddress(addr)
{
    postOffice = new PostOffice(addr, reliability, nBoxes);
    interrupt->Schedule(&SynchPost::periodicCloseChecker, (int)this, 40000000, TimerInt);
}

SynchPost::~SynchPost()
{
    delete postOffice;
}

/*!
 * Sends data to the specified address using the specified mailbox (something like port)
 * Ensures that data has been received
 * \param addr      <- address of the recepient (in order to set an address, launch NACHOS with -m %n% param
 * \param mailbox   <- mailbox for communication (some sort of port analogue)
 * \param data      <- data to send
 * \param len       <- number of bytes to send
 * \return 0        <- if data transfer was successful
 *        -2        <- if attempt has been made to transfer more than NETWORK_MAX_TRANSFER_BYTES (2048) bytes
 *        -1        <- if at least one sent packet has not been acknowledged by the recepient
 */
int SynchPost::SendTo(int addr, int mailbox, const char *data, int len, unsigned specialMask)
{
    PacketHeader outPktHdr;
    PacketHeader inPktHdr;
    MailHeader outMailHdr;
    MailHeader inMailHdr;
    char buffer[MaxMailSize + 1] = { 0 };

    //TODO: Use goto error_exit convention
    if (len > NETWORK_MAX_TRANSFER_BYTES) {
        return -2;
    }

    // construct packet, mail header for original message
    // To: destination machine, mailbox 0
    // From: our machine, reply to: mailbox 1
    outPktHdr.to = addr;
    outMailHdr.to = mailbox;
    outMailHdr.from = mailbox + NUM_MAIL_BOXES / 2; // TODO: Double-check how we determine it automatically

    char currentChunk[MaxMailSize + 1];
    int numChunks = divRoundUp(len, MaxMailSize);

    // Start with -1, since packet -1'st packet indicates a header
    // that contains information about total number of packets that will be transferred
    for(int ii = -1; ii < numChunks; ++ii) {
        memset(currentChunk, 0x00, sizeof(currentChunk));

        if (ii >= 0) { /* if this is a regular data packet */
            memset(buffer, 0x00, sizeof(buffer));
            int chunkLength = std::min(MaxMailSize, static_cast<unsigned>(len));
            outMailHdr.length = chunkLength;
            outMailHdr.index = getMasked(ii, specialMask);
            len -= chunkLength;
            memcpy(currentChunk, data + ii * MaxMailSize, chunkLength);
        } else { /* if this is a FIRST data packet */
            outMailHdr.index = getMasked(static_cast<unsigned>(len), HEADER_MASK);

            if (SPECIAL_CLOSE_CONN == specialMask) {
                outMailHdr.index = getMasked(outMailHdr.index, specialMask);
            }

            outMailHdr.length = strlen(HEADER_WORD);
            char headerData[MaxMailSize] = HEADER_WORD;
            memcpy(currentChunk, headerData, outMailHdr.length);
        }

        int status = 0;
        int numAttempts = 0;
        int cmpRes = 0;

        // Try sending data until we receive an ACK or number of attemts is exceeded
        do {
            memset(buffer, 0x00, sizeof(buffer));
            // Send the chunk
            postOffice->Send(outPktHdr, outMailHdr, currentChunk);

            // Receive ACK
            status = postOffice->Receive(outMailHdr.from, &inPktHdr, &inMailHdr, buffer, MAX_ACK_TIMEOUT);

            // Expect usual ACK for usual packets, HEADER_ACK for headers
            if (ii >= 0) {
                // Fail the acknowledgement if indexes do not match
                cmpRes = strncmp(buffer, ACK_WORD, strlen(ACK_WORD));
                if (outMailHdr.index != getIndex(inMailHdr.index, ACK_MASK)) {
                    cmpRes = -1;

                }
            } else {
                // Fail the acknowledgement if indexes do not match
                cmpRes = strncmp(buffer, HEADER_ACK, strlen(HEADER_ACK));
                if (getIndex(outMailHdr.index, HEADER_MASK) != getIndex(inMailHdr.index, HEADER_ACK_MASK)) {
                    cmpRes = -1;
                }
            }

            ++numAttempts;
        } while(((status != 0) || (cmpRes != 0)) && (numAttempts < MAX_ATTEMPTS));

        // Transfer failed if chunk cannot be transmitted
        if (status != 0) {
            goto error_exit;
        }
    }

    return 0;
    error_exit:
    return -1;
}

/*!
 * data MUST be able to accomodate MaxMailSize bytes
 * if out_pktHeader is not null, it will contain received packet header
 */
unsigned SynchPost::ReceiveSingleChunkFrom(int mailbox, char *data, int timeout, PacketHeader *out_pktHeader)
{
    MailHeader inMailHdr;
    PacketHeader inPktHdr;

    MailHeader outMailHdr;
    PacketHeader outPktHdr;

    int status = postOffice->Receive(mailbox, &inPktHdr, &inMailHdr, data, timeout);
    if (-1 == status) {
        return -5;
    }

    const char *ackMessage;

    // Depending on the data type generate an appropriate acknowledgement message
    // TODO: header flag should be set directly in the packet properties rather than being derived from data
    unsigned mask = 0;
    unsigned pureIndex = 0;
    if (isHeader(inMailHdr.index)) {
        ackMessage = HEADER_ACK;
        mask = HEADER_ACK_MASK;
        pureIndex = getIndex(inMailHdr.index, HEADER_MASK);
    } else {
        ackMessage = ACK_WORD;
        mask = ACK_MASK;
        pureIndex = inMailHdr.index;
    }

    outPktHdr.to = inPktHdr.from;
    outMailHdr.to = inMailHdr.from;
    outMailHdr.length = strlen(ackMessage);
    outMailHdr.index = getMasked(pureIndex, mask);

    postOffice->Send(outPktHdr, outMailHdr, ackMessage);

    if (out_pktHeader) {
        *out_pktHeader = inPktHdr;
    }

    return inMailHdr.index;
}

/*!
 * Returns the number of bytes read
 */
int SynchPost::ReceiveFrom(int mailbox, char *data, PacketHeader *out_pktHeader)
{
    int ix = -1;
    int numChunks = 0;
    char buffer[MaxMailSize + 1] = { 0 };
    int len = -1;
    int retLen = len;
    for (; ix != numChunks; ) {
        memset(buffer, 0x00, sizeof(buffer));
        if (-1 == ix) { /* If we are to receive first packet - header */
            char headerData[MaxMailSize] = { 0 };
            unsigned maskedIndex = ReceiveSingleChunkFrom(mailbox, headerData, -1, out_pktHeader);

            if (isClose(maskedIndex)) {
                return CONN_CLOSE_RETVAL;
            }

            len = (int)getIndex(maskedIndex, HEADER_MASK);
            retLen = len;

            // Check we have a valid header
            int cmpRes = strncmp(headerData, HEADER_WORD, strlen(HEADER_WORD));
            if ((cmpRes != 0) || (len <= 0) || (!isHeader(maskedIndex))) {
                continue;
            }

            numChunks = divRoundUp(len, MaxMailSize);
        } else {
            unsigned packetIndex = ReceiveSingleChunkFrom(mailbox, buffer, -1);

            if (isClose(packetIndex)) {
                return CONN_CLOSE_RETVAL;
            }

            if (getMask(packetIndex) >= SPECIAL_FILE_END) {
                return -15;
            }

            if ((int)packetIndex != ix) {
                continue;
            }
            int curLength = std::min(MaxMailSize, static_cast<unsigned>(len));
            len -= curLength;

            memcpy(data + ix * MaxMailSize, buffer, curLength);
        }
        ++ix;
    }

    return retLen;
}

/*!
 * Waits for a connection request by the client on the specified mailbox
 * \returns connection identifier if connection is successful
 *          -2 if the server connection quota is exceeded
 *          -1 RESERVED for future use
 */
int SynchPost::ConnectAsServer(int mailbox)
{
    int retVal = -1;
    int cmpRes = 0;
    int status = 0;
    char data[MaxMailSize] = { 0 };
    PacketHeader inPacketHeader;

    int connectionIx = availConnections.Find();
    if (connectionIx == -1) {
        retVal = -2;
        goto early_exit;
    }

    do {
        status = ReceiveFrom(mailbox, data, &inPacketHeader);
        cmpRes = strncmp(data, INIT_CONN, strlen(INIT_CONN));
    } while ((cmpRes != 0) || (status <= 0));

    connections[connectionIx].connectionID = connectionIx;
    connections[connectionIx].serverAddress = selfAddress;
    connections[connectionIx].clientAddress = inPacketHeader.from;
    connections[connectionIx].mailbox = mailbox;
    connections[connectionIx].tid = currentThread->Tid();
    connections[connectionIx].isServer = true;
    retVal = connectionIx;

    early_exit:
    return retVal;
}

/*!
 * Initiates connection to the server specified by address and mailbox
 * \returns connection identifier in case of success
 *          -2 if quota of connection is exceeded
 *          -1 if connection canno be established
 */
int SynchPost::ConnectAsClient(int address, int mailbox)
{
    int retVal = -1;
    int status = 0;
    int peekStatus = 0;

    int connectionIx = availConnections.Find();
    if (connectionIx == -1) {
        retVal = -2;
        goto early_exit;
    }

    do {
        peekStatus = postOffice->Peek(mailbox, nullptr, nullptr);
        if (peekStatus == 0) {
            postOffice->Remove(mailbox);
        }
    } while(peekStatus == 0);

    status = SendTo(address, mailbox, INIT_CONN, strlen(INIT_CONN));
    (void)status;
#if 0
    if (0 != status) {
        availConnections.Clear(connectionIx);
        retVal = -1;
        goto early_exit;
    }
#endif

    connections[connectionIx].connectionID = connectionIx;
    connections[connectionIx].clientAddress = selfAddress;
    connections[connectionIx].serverAddress = address;
    connections[connectionIx].mailbox = mailbox;
    connections[connectionIx].tid = currentThread->Tid();
    connections[connectionIx].isServer = false;
    retVal = connectionIx;

    early_exit:
    return retVal;
}

#define INVALID_CONN_ID (-10)
#define INVALID_THREAD (-20)
#define INVALID_ACCESS (-30)

int SynchPost::SendToByConnId(int connId, const char *data, int len, unsigned specialMask)
{
    int retVal = 0;

    int check = checkConnIdValidity(connId);
    if (0 != check) {
        return check;
    }

    auto curConnection = connections[connId];

    if (curConnection.isServer) {

        retVal = SendTo(curConnection.clientAddress, curConnection.mailbox, data, len, specialMask);
    } else {
        retVal = SendTo(curConnection.serverAddress, curConnection.mailbox, data, len, specialMask);
    }
    return retVal;
}

int SynchPost::ReceiveFromByConnId(int connId, char *data)
{
    int retVal = 0;

    int check = checkConnIdValidity(connId);
    if (0 != check) {
        return check;
    }
    auto curConnection = connections[connId];
    retVal = ReceiveFrom(curConnection.mailbox, data);

    if (retVal == CONN_CLOSE_RETVAL) {
        performConnClose(connId);
    }

    return retVal;
}

/*!
 *
 * \return -1       < File cannot be opened
 */
int SynchPost::SendFile(int connId, const char *fileName, int *transferSpeed) {
    int retVal = 0;

    OpenFile *file = fileSystem->Open(fileName);
    if (!file) {
        retVal = -1;
        return retVal;
    }

    char buffer[MAX_FILE_CHUNK + 1] = { 0 };
    int lengthOfFile = file->Length();
    int chunks = divRoundUp(lengthOfFile, MAX_FILE_CHUNK);
    std::string cvtNumOfFileChunks = std::to_string(chunks);
    SendToByConnId(connId, cvtNumOfFileChunks.c_str(), cvtNumOfFileChunks.length());
    std::chrono::high_resolution_clock::time_point beforeFile = std::chrono::high_resolution_clock::now();
    int totalBytes = 0;
    for (int ii = 0; ii < chunks; ++ii) {
        int bytesRead = file->Read(buffer, MAX_FILE_CHUNK);
        totalBytes += bytesRead;
        std::chrono::high_resolution_clock::time_point beforeSending = std::chrono::high_resolution_clock::now();
        SendToByConnId(connId, buffer, bytesRead);
        std::chrono::high_resolution_clock::time_point afterSending = std::chrono::high_resolution_clock::now();
        (void)beforeSending;
        (void)afterSending;
#if 0
        printf("^^^^^^^^^^^^^^^ >>>>>>>>>>>>>> SPEED (KBytes / second): %lf \n",
               (double)bytesRead /
               (double)std::chrono::duration_cast<std::chrono::milliseconds>(afterSending - beforeSending).count());
#endif
    }
    std::chrono::high_resolution_clock::time_point afterFile = std::chrono::high_resolution_clock::now();
    if (transferSpeed) {
        *transferSpeed = static_cast<int> (
                (double)totalBytes /
                (double)std::chrono::duration_cast<std::chrono::milliseconds>(afterFile - beforeFile).count()
                );
    }

    SendToByConnId(connId, TERMINATION_INDICATOR, strlen(TERMINATION_INDICATOR), SPECIAL_FILE_END);

    //TODO: Integrate with Phil
    delete file;
    return retVal;
}

int SynchPost::ReceiveFile(int connId, const char *fileName) {
    int retVal = 0;

    OpenFile *file = fileSystem->Open(fileName);
    bool creationSucc = true;

    if (file) {
        delete file;
        fileSystem->Remove(fileName);
    }

    creationSucc = fileSystem->Create(fileName, 0);
    file = fileSystem->Open(fileName);

    if (!creationSucc || !file) {
        retVal = -1;
        return retVal;
    }

    char buffer[MAX_FILE_CHUNK + 1] = { 0 };
    int bytesRead = 0;
    bytesRead = ReceiveFromByConnId(connId, buffer);

    if (bytesRead <= 0) {
        retVal = -2;
        return retVal;
    }

    int fileChunksToRead = atoi(buffer);
    for (int ii = 0; ii < fileChunksToRead; ++ii) {
        bytesRead = ReceiveFromByConnId(connId, buffer);
        if (bytesRead <= 0) {
            retVal = -2;
            return retVal;
        }

        file->Write(buffer, bytesRead);
    }

    //Receive terminator
    (void)ReceiveFromByConnId(connId, buffer);

    //TODO: Integrate with Phil
    delete file;

    return retVal;
}

int SynchPost::CloseConnection(int connId)
{
    int retVal = 0;
    const char *data = CONNECTION_CLOSE;

    // Invalid ID specified
    if ((connId < 0) || (connId >= NETWORK_MAX_CONNECTIONS) || (!availConnections.Test(connId))) {
        retVal = -1;
        goto early_exit;
    }

    SendToByConnId(connId, data, strlen(data), SPECIAL_CLOSE_CONN);
    retVal = performConnClose(connId);

    early_exit:
    return retVal;
}

int SynchPost::checkConnIdValidity(int connId) const {
    if ((connId < 0) || (connId >= NETWORK_MAX_CONNECTIONS) || !availConnections.Test(connId)) {
        return INVALID_CONN_ID;
    }

    auto curConnection = connections[connId];
    if (curConnection.tid != currentThread->Tid()) {
        return INVALID_THREAD;
    }

    return 0;
}

bool SynchPost::checkConnClosed(int connId) {
    bool retVal = false;
    int status;
    ConnectionID curConnection;

    if ((connId < 0) || (connId >= NETWORK_MAX_CONNECTIONS) || (!availConnections.Test(connId))) {
        retVal = false;
        goto early_exit;
    }

    MailHeader mailHdr;
    PacketHeader pktHdr;

    curConnection = connections[connId];

    status = postOffice->Peek(curConnection.mailbox, &pktHdr, &mailHdr);
    if (status != 0) {
        retVal = false;
        goto early_exit;
    }

    if (isClose(mailHdr.index)) {
        retVal = true;
    } else {
        retVal = false;
    }

    early_exit:
    return retVal;
}

void SynchPost::periodicCloseChecker(int arg) {
    SynchPost *thisPtr = (SynchPost *)arg;

    for (int ii = 0; ii < NETWORK_MAX_CONNECTIONS; ++ii) {
        if (thisPtr->checkConnClosed(ii)) {
            thisPtr->performConnClose(ii);
        }
    }

    interrupt->Schedule(&SynchPost::periodicCloseChecker, arg, 40000000, TimerInt);

}

int SynchPost::performConnClose(int connId) {
    int retVal = 0;

    if ((connId < 0) || (connId >= NETWORK_MAX_CONNECTIONS) || (!availConnections.Test(connId))) {
        retVal = -1;
        goto early_exit;
    }

    printf("CONNECTION ID %d CLOSED \n", connId);
    availConnections.Clear(connId);

    early_exit:
    return retVal;
}
