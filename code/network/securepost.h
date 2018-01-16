#include "copyright.h"

#ifndef POST_H
#define POST_H

#include "network.h"
#include "synchlist.h"
#include "timer.h"

// Mailbox address -- uniquely identifies a mailbox on a given machine.
// A mailbox is just a place for temporary storage for messages.
typedef int MailBoxAddressSecure;

class MailHeaderSecure {
  public:
    MailBoxAddressSecure to;      // Destination mail box
    MailBoxAddressSecure from;    // Mail box to reply to
    unsigned length;        // Bytes of message data (excluding the 
                // mail header)
    int segments;
    unsigned checksum;
    int ACK;
    int SYN;
    int FIN;
};

class Connection {
    public:
        Connection();
        void Reset();
        int remoteAddr;
        int remoteBox;
        int localBox;
};

#define MaxMailSizeSecure     (MaxPacketSize - sizeof(MailHeaderSecure))
#define MAXREEMISSIONS 5
#define HANDSHAKEREQUESTBOX 0
#define HANDSHAKEANSWERBOX 1


// The following class defines the format of an incoming/outgoing 
// "Mail" message.  The message format is layered: 
//  network header (PacketHeader) 
//  post office header (MailHeader) 
//  data

class MailSecure {
  public:
     MailSecure(PacketHeader pktH, MailHeaderSecure mailH, char *msgData);
                // Initialize a mail message by
                // concatenating the headers to the data

     PacketHeader pktHdr;   // Header appended by Network
     MailHeaderSecure mailHdr;    // Header appended by PostOffice
     char data[MaxMailSizeSecure];    // Payload -- message data
};

// The following class defines a single mailbox, or temporary storage
// for messages.   Incoming messages are put by the PostOffice into the 
// appropriate mailbox, and these messages can then be retrieved by
// threads on this machine.

class MailBoxSecure {
  public: 
    MailBoxSecure();          // Allocate and initialize mail box
    ~MailBoxSecure();         // De-allocate mail box

    void StartCountingTimeout();
    void StopCountingTimeout();
    void Put(PacketHeader pktHdr, MailHeaderSecure mailHdr, char *data);
                // Atomically put a message into the mailbox
    void Get(PacketHeader *pktHdr, MailHeaderSecure *mailHdr, char *data, bool timeO); 
                // Atomically get a message out of the 
                // mailbox (and wait if there is no message 
                // to get!)

    long long int GetTimeout();
    long long int GetInitialTicks();
    void ReachTimeout();

    void Check();
    int GetRemoteConnected();
    bool IsAvailable();
    bool IsReusable();



  private:
    SynchList *messages;    // A mailbox is just a list of arrived messages
    Timer *timerTimeout;
    long long int initialTicks;
    long long int timeout;
    int connected;
    long long int tickOfLastMessage;
};

class SecurePost {
public:
    SecurePost(NetworkAddress addr, double reliability, int nBoxes);
                // Allocate and initialize Post Office
                //   "reliability" is how many packets
                //   get dropped by the underlying network
    ~SecurePost();      // De-allocate Post Office data
    
    void Send(PacketHeader pktHdr, MailHeaderSecure mailHdr, const char *data);
                    // Send a message to a mailbox on a remote 
                // machine.  The fromBox in the MailHeader is 
                // the return box for ack's.
    
    void Receive(int box, PacketHeader *pktHdr, 
        MailHeaderSecure *mailHdr, char *data, bool timeout);
                    // Retrieve a message from "box".  Wait if
                // there is no message in the box.

    void PostalDelivery();  // Wait for incoming messages, 
                // and then put them in the correct mailbox

    void PacketSent();      // Interrupt handler, called when outgoing 
                // packet has been put on network; next 
                // packet can now be sent
    void IncomingPacket();  // Interrupt handler, called when incoming
                // packet has arrived and can be pulled
                // off of network (i.e., time to call 
                // PostalDelivery)
    void CheckTimeout();

    int SendMessageWithAck(PacketHeader pH, MailHeaderSecure mH, PacketHeader *inPH, 
        MailHeaderSecure *inMH, const char* outMessage, char* inMessage);

    int PerformHandshake(int dest);

    int PerformCloseHandshake(Connection * con);

    //int ListenHandshake(int origin);

    int SendSecure(char *message, int dest);

    int ReceiveSecure(int origin, int timeout, char *buff);

    int GetAvailableBox();

    int OpenConnection(int dest);

    int CloseConnection(int dest);

    Connection* GetConnection(int dest);

    int AcceptHandshake();
    int RespondCloseHandshake(Connection* con);


private:
	Network *network;      // Physical network connection
    NetworkAddress netAddr; // Network address of this machine
    MailBoxSecure *boxes;     // Table of mail boxes to hold incoming mail
    int numBoxes;       // Number of mail boxes
    Semaphore *messageAvailable;// V'ed when message has arrived from network
    Semaphore *messageSent; // V'ed when next message can be sent to network
    Lock *sendLock;     // Only one outgoing message at a time
    Timer *timerTimeout;
    //int *connections;
    Connection *connections;
    Lock *conLock;
    //SynchList connections;
};

#endif