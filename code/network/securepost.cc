#include "copyright.h"
#include "securepost.h"
#include "system.h"


#include <strings.h> /* for bzero */
#include <iostream>
#include <sstream>
#include <cstring>

const char* HANDSHAKEMESSAGE = "handshaking";
const char* ACK = "ack";
const char* SYN = "sync";

void TimeoutHandler (int b)
{
    SecurePost *post = (SecurePost *)b;
    post->CheckTimeout();
    /*if(box->GetInitialTicks() == -1) return;
    
    if((stats->totalTicks - box->GetInitialTicks()) >= box->GetTimeout()) {
        printf("Timeout handled with stats ticks %lld and initial ticks %lld\n", stats->totalTicks, box->GetInitialTicks());
        fflush(stdout);
        box->ReachTimeout();
    }*/

}

Connection::Connection() {
    remoteAddr = -1;
    remoteBox = -1;
    localBox = -1;
}

void
Connection::Reset()
{
    remoteAddr = -1;
    remoteBox = -1;
    localBox = -1;
}
//----------------------------------------------------------------------
// Mail::Mail
//      Initialize a single mail message, by concatenating the headers to
//  the data.
//
//  "pktH" -- source, destination machine ID's
//  "mailH" -- source, destination MailBoxSecure ID's
//  "data" -- payload data
//----------------------------------------------------------------------

MailSecure::MailSecure(PacketHeader pktH, MailHeaderSecure mailH, char *msgData)
{
    ASSERT(mailH.length <= MaxMailSizeSecure);

    pktHdr = pktH;
    mailHdr = mailH;
    bcopy(msgData, data, mailHdr.length);
}

//----------------------------------------------------------------------
// MailBoxSecure::MailBoxSecure
//      Initialize a single mail box within the post office, so that it
//  can receive incoming messages.
//
//  Just initialize a list of messages, representing the MailBoxSecure.
//----------------------------------------------------------------------


MailBoxSecure::MailBoxSecure()
{ 
    messages = new SynchList();
    timeout = 500000000;
    initialTicks = -1;
    connected = -1;
}

void MailBoxSecure::StartCountingTimeout() {
    initialTicks = stats->totalTicks;
    //timerTimeout = new Timer(TimeoutHandler, (int)this, false);
}

void MailBoxSecure::StopCountingTimeout()
{
    initialTicks = -1;
    /*if(timerTimeout != NULL) {
        delete timerTimeout;
        timerTimeout = NULL;
    }*/
}

void MailBoxSecure::ReachTimeout() {
    StopCountingTimeout();
    messages->ReachTimeout();
}

long long int MailBoxSecure::GetTimeout() {
    return timeout;
}

long long int MailBoxSecure::GetInitialTicks() {
    return initialTicks;
}

void MailBoxSecure::Check() 
{
    if(GetInitialTicks() == -1) {
        return;
    }
    long long int ticks = stats->totalTicks;
    //printf("Checking with substract %lld\n", ticks - GetInitialTicks());
    if((ticks - GetInitialTicks()) >= GetTimeout()) {
        //printf("Timeout handled with stats ticks %lld and initial ticks %lld and substract %lld\n", ticks, GetInitialTicks(), ticks - GetInitialTicks());
        //fflush(stdout);

        StopCountingTimeout();
        messages->ReachTimeout();
    }
}
//----------------------------------------------------------------------
// MailBoxSecure::~MailBoxSecure
//      De-allocate a single mail box within the post office.
//
//  Just delete the MailBoxSecure, and throw away all the queued messages 
//  in the MailBoxSecure.
//----------------------------------------------------------------------

MailBoxSecure::~MailBoxSecure()
{ 
    delete messages;
}

//----------------------------------------------------------------------
// PrintHeader
//  Print the message header -- the destination machine ID and MailBoxSecure
//  #, source machine ID and MailBoxSecure #, and message length.
//
//  "pktHdr" -- source, destination machine ID's
//  "mailHdr" -- source, destination MailBoxSecure ID's
//----------------------------------------------------------------------

static void 
PrintHeader(PacketHeader pktHdr, MailHeaderSecure mailHdr)
{
    printf("From (%d, %d) to (%d, %d) bytes %d\n",
            pktHdr.from, mailHdr.from, pktHdr.to, mailHdr.to, mailHdr.length);
}

//----------------------------------------------------------------------
// MailBoxSecure::Put
//  Add a message to the MailBoxSecure.  If anyone is waiting for message
//  arrival, wake them up!
//
//  We need to reconstruct the Mail message (by concatenating the headers
//  to the data), to simplify queueing the message on the SynchList.
//
//  "pktHdr" -- source, destination machine ID's
//  "mailHdr" -- source, destination MailBoxSecure ID's
//  "data" -- payload message data
//----------------------------------------------------------------------

void 
MailBoxSecure::Put(PacketHeader pktHdr, MailHeaderSecure mailHdr, char *data)
{ 
    MailSecure *mail = new MailSecure(pktHdr, mailHdr, data); 

    messages->Append((void *)mail); // put on the end of the list of 
                    // arrived messages, and wake up 
                    // any waiters
    tickOfLastMessage = stats->totalTicks;
}

//----------------------------------------------------------------------
// MailBoxSecure::Get
//  Get a message from a MailBoxSecure, parsing it into the packet header,
//  MailBoxSecure header, and data. 
//
//  The calling thread waits if there are no messages in the MailBoxSecure.
//
//  "pktHdr" -- address to put: source, destination machine ID's
//  "mailHdr" -- address to put: source, destination MailBoxSecure ID's
//  "data" -- address to put: payload message data
//----------------------------------------------------------------------

void 
MailBoxSecure::Get(PacketHeader *pktHdr, MailHeaderSecure *mailHdr, char *data, bool timeO) 
{ 

    DEBUG('n', "Waiting for mail in MailBoxSecure\n");
    MailSecure *mail;

    if(timeO) {
        StartCountingTimeout();
        mail = (MailSecure *) messages->RemoveTimeout();   // remove message from list;
                            // will wait if list is empty
        if(mail == NULL) {
            mailHdr->segments = -1;

            return;
        }

        StopCountingTimeout();
    } else {
        mail = (MailSecure *) messages->Remove();   // remove message from list;
                            // will wait if list is empty
    }

    *pktHdr = mail->pktHdr;
    *mailHdr = mail->mailHdr;

    mailHdr->segments = 1;

    if (DebugIsEnabled('n')) {
    printf("Got mail from MailBoxSecure: ");
    PrintHeader(*pktHdr, *mailHdr);
    }
    bcopy(mail->data, data, mail->mailHdr.length);
                    // copy the message data into
                    // the caller's buffer
    delete mail;            // we've copied out the stuff we
                    // need, we can now discard the message
    delete timerTimeout;
}

bool
MailBoxSecure::IsAvailable() {
    return (initialTicks == -1) ? true : false;
}

int
MailBoxSecure::GetRemoteConnected() {
    return connected;
}

bool
MailBoxSecure::IsReusable() {
    if((stats->totalTicks - tickOfLastMessage) > timeout) {
        return true;
    }

    return false;
}

//----------------------------------------------------------------------
// PostalHelper, ReadAvail, WriteDone
//  Dummy functions because C++ can't indirectly invoke member functions
//  The first is forked as part of the "postal worker thread; the
//  later two are called by the network interrupt handler.
//
//  "arg" -- pointer to the Post Office managing the Network
//----------------------------------------------------------------------

static void PostalHelper(int arg)
{ SecurePost* po = (SecurePost *) arg; po->PostalDelivery(); }
static void ReadAvail(int arg)
{ SecurePost* po = (SecurePost *) arg; po->IncomingPacket(); }
static void WriteDone(int arg)
{ SecurePost* po = (SecurePost *) arg; po->PacketSent(); }

//----------------------------------------------------------------------
// SecurePost::SecurePost
//  Initialize a post office as a collection of MailBoxSecurees.
//  Also initialize the network device, to allow post offices
//  on different machines to deliver messages to one another.
//
//      We use a separate thread "the postal worker" to wait for messages 
//  to arrive, and deliver them to the correct MailBoxSecure.  Note that
//  delivering messages to the MailBoxSecurees can't be done directly
//  by the interrupt handlers, because it requires a Lock.
//
//  "addr" is this machine's network ID 
//  "reliability" is the probability that a network packet will
//    be delivered (e.g., reliability = 1 means the network never
//    drops any packets; reliability = 0 means the network never
//    delivers any packets)
//  "nBoxes" is the number of mail boxes in this Post Office
//----------------------------------------------------------------------

SecurePost::SecurePost(NetworkAddress addr, double reliability, int nBoxes)
{
// First, initialize the synchronization with the interrupt handlers
    messageAvailable = new Semaphore("message available", 0);
    messageSent = new Semaphore("message sent", 0);
    sendLock = new Lock("message send lock");

// Second, initialize the MailBoxSecurees
    netAddr = addr;
    numBoxes = nBoxes;
    boxes = new MailBoxSecure[nBoxes];

    connections = new Connection[nBoxes];
    conLock = new Lock("Connection lock");
    //connections = new SynchList();
    

// Third, initialize the network; tell it which interrupt handlers to call
    network = new Network(addr, reliability, ReadAvail, WriteDone, (int) this);


// Finally, create a thread whose sole job is to wait for incoming messages,
//   and put them in the right MailBoxSecure. 
    Thread *t = new Thread("postal worker");

    t->Fork(PostalHelper, (int) this);
    timerTimeout = new Timer(TimeoutHandler, (int)this, false);
}

//----------------------------------------------------------------------
// SecurePost::~SecurePost
//  De-allocate the post office data structures.
//----------------------------------------------------------------------

SecurePost::~SecurePost()
{
    delete network;
    delete [] boxes;
    delete messageAvailable;
    delete messageSent;
    delete sendLock;
    if(timerTimeout != NULL) {
        delete timerTimeout;
        timerTimeout = NULL;
    }
    delete [] connections;
    delete conLock;
}

//----------------------------------------------------------------------
// SecurePost::PostalDelivery
//  Wait for incoming messages, and put them in the right MailBoxSecure.
//
//      Incoming messages have had the PacketHeader stripped off,
//  but the MailHeaderSecure is still tacked on the front of the data.
//----------------------------------------------------------------------

void
SecurePost::PostalDelivery()
{
    PacketHeader pktHdr;
    MailHeaderSecure mailHdr;
    char *buffer = new char[MaxPacketSize];

    for (;;) {
        // first, wait for a message
        messageAvailable->P();  
        pktHdr = network->Receive(buffer);

        mailHdr = *(MailHeaderSecure *)buffer;
        //printf("Received a message from: %d with FIN %d\n", pktHdr.from, mailHdr.FIN);
        if (DebugIsEnabled('n')) {
        printf("Putting mail into MailBoxSecure: ");
        PrintHeader(pktHdr, mailHdr);
        }

        //printf("Putting mail into MailBoxSecure: ");
        //PrintHeader(pktHdr, mailHdr);

    // check that arriving message is legal!
    ASSERT(0 <= mailHdr.to && mailHdr.to < numBoxes);
    ASSERT(mailHdr.length <= MaxMailSizeSecure);

    // put into MailBoxSecure
    //printf("Got mail in box %d\n", mailHdr.to);
        boxes[mailHdr.to].Put(pktHdr, mailHdr, buffer + sizeof(MailHeaderSecure));
        /*else {
            int result;

            Connection* con = GetConnection(pktHdr.from);
            result = RespondCloseHandshake(con);
            printf("Result of answer CLOSE handshake: %d\n",result);
        }*/
    }
}

int
SecurePost::SendMessageWithAck(PacketHeader pH, MailHeaderSecure mH, PacketHeader *inPH, 
    MailHeaderSecure *inMH, const char* outMessage, char* inMessage)
{
    for(int i = 0; i < MAXREEMISSIONS; i++) {
        printf("Attempt: %d\n", i);
        Send(pH, mH, outMessage);

        //Receive hanshake ack
        Receive(mH.from, inPH, inMH, inMessage, true);

        if(inMH->segments != -1) return true;
    }

    return false;
}

/*
 * Three step handshake
 *
 */

int
SecurePost::PerformHandshake(int dest) {
    char buffer[MaxMailSizeSecure];
    PacketHeader outPktHdr, inPktHdr;
    MailHeaderSecure outMailHdr, inMailHdr;

    int avBox = GetAvailableBox(); //synchronize this
    if(avBox == -1) return -1;

    std::stringstream str;
    str << avBox;
    const char* message = str.str().c_str();

    //Send a synch message
    outPktHdr.to = dest;
    outPktHdr.from = netAddr;
    outMailHdr.to = HANDSHAKEREQUESTBOX;
    outMailHdr.from = HANDSHAKEANSWERBOX;
    outMailHdr.length = strlen(message) + 1;
    outMailHdr.ACK = 0;
    outMailHdr.SYN = 1;

    bool result = SendMessageWithAck(outPktHdr, outMailHdr, &inPktHdr, &inMailHdr, message, buffer);

    if(!result || inMailHdr.ACK != 1 || inMailHdr.SYN != 1) { //We did't get what we expected
        return -1;
    }

    //Send the last ack
    outMailHdr.ACK = 1;
    outMailHdr.SYN = 0;
    outMailHdr.to = HANDSHAKEANSWERBOX;

    Send(outPktHdr, outMailHdr, message);
    

    //Get the box in the answer message
    printf("Finished handshake: Remote box to send to: %s\n", buffer);

    int remoteBox = atoi(buffer);
    conLock->Acquire();
    connections[avBox].remoteAddr = dest;
    connections[avBox].remoteBox = remoteBox;
    connections[avBox].localBox = avBox; //Keep track of connections
    conLock->Release();
    //Set the box 
    return avBox;

}

/*
 * Three step handshake
 *
 */

int
SecurePost::AcceptHandshake() {
    //printf("Started accepting handshake\n");
    char buffer[MaxMailSizeSecure];
    PacketHeader outPktHdr, inPktHdr;
    MailHeaderSecure outMailHdr, inMailHdr;

    //Receive handshake initial
    Receive(HANDSHAKEREQUESTBOX, &inPktHdr, &inMailHdr, buffer, true);

    if(inMailHdr.segments == -1) return -1;

    int remoteBox = atoi(buffer);

    int avBox = GetAvailableBox(); //synchronize this
    if(avBox == -1 || inMailHdr.SYN != 1) return -1;

    //printf("Received handshake message\n");

    std::stringstream str;
    str << avBox;
    const char* message = str.str().c_str();

    //Send a synch message
    outPktHdr.to = inPktHdr.from;
    outPktHdr.from = netAddr;
    outMailHdr.to = inMailHdr.from;
    outMailHdr.from = HANDSHAKEANSWERBOX;
    outMailHdr.length = strlen(message) + 1;
    outMailHdr.ACK = 1;
    outMailHdr.SYN = 1;

    bool result = SendMessageWithAck(outPktHdr, outMailHdr, &inPktHdr, &inMailHdr, message, buffer);

    if(!result || inMailHdr.ACK != 1) { //We did't get what we expected
        return -1;
    }

    conLock->Acquire();
    connections[avBox].remoteAddr = inPktHdr.from;
    connections[avBox].remoteBox = remoteBox;
    connections[avBox].localBox = avBox; //Keep track of connections
    conLock->Release();
    printf("Finished accepting handshake for remote addr %d: \n", connections[avBox].remoteAddr);

    //Set the box 
    return avBox;

}

int
SecurePost::RespondCloseHandshake(Connection *con) {
    char buffer[MaxMailSizeSecure];
    PacketHeader outPktHdr, inPktHdr;
    MailHeaderSecure outMailHdr, inMailHdr;
    const char* message = "fin";

    //Send a FIN message
    outPktHdr.to = con->remoteAddr;
    outPktHdr.from = netAddr;
    outMailHdr.to = con->remoteBox;
    outMailHdr.from = con->localBox;
    outMailHdr.length = strlen(message) + 1;
    outMailHdr.ACK = 1;
    outMailHdr.SYN = 0;
    outMailHdr.FIN = 0;

    Send(outPktHdr, outMailHdr, message);

    //Do something in between maybe

    outMailHdr.ACK = 0;
    outMailHdr.SYN = 0;
    outMailHdr.FIN = 1;
    bool result = SendMessageWithAck(outPktHdr, outMailHdr, &inPktHdr, &inMailHdr, message, buffer);

    if(!result || inMailHdr.ACK != 1) { //Not what we expected
        return -1;
    }   

    //Get the box in the answer message
    //printf("Finished close handshake: Remote box to send to: %s\n", buffer);

    //Set box assigned to this dest
    
    
    connections[con->localBox].Reset();
    //Set the box 
    return true;

}

Connection*
SecurePost::GetConnection(int dest) {
    for(int i = 2; i < numBoxes; i++) {

        //printf("connection %d with remoteAddr %d\n", i, connections[i].remoteAddr);
        if(connections[i].localBox == dest) {
            //conLock->Release();
            return &(connections[i]);
        }
    }
    return NULL;
}

int
SecurePost::PerformCloseHandshake(Connection* con) {
    char buffer[MaxMailSizeSecure];
    PacketHeader outPktHdr, inPktHdr;
    MailHeaderSecure outMailHdr, inMailHdr;
    const char* message = "fin";

    //Send a FIN message
    outPktHdr.to = con->remoteAddr;
    outPktHdr.from = netAddr;
    outMailHdr.to = con->remoteBox;
    outMailHdr.from = con->localBox;
    outMailHdr.length = strlen(message) + 1;
    outMailHdr.ACK = 0;
    outMailHdr.SYN = 0;
    outMailHdr.FIN = 1;

    printf("Sending message to close to ned addr %d: ",netAddr);
    PrintHeader(outPktHdr, outMailHdr);
    
    bool result = SendMessageWithAck(outPktHdr, outMailHdr, &inPktHdr, &inMailHdr, message, buffer);

    if(!result || inMailHdr.ACK != 1) { //Not what we expected
        return -1;
    }

    //Expect the FIN message of the other side
    Receive(con->localBox, &inPktHdr, &inMailHdr, buffer, true);

    if(inMailHdr.segments == -1 || inMailHdr.FIN != 1) return -1; //we didn't get what we expected

    //Send the last ack
    outMailHdr.ACK = 1;
    outMailHdr.SYN = 0;
    outMailHdr.FIN = 0;
    outMailHdr.to = con->remoteBox;

    Send(outPktHdr, outMailHdr, message);
    

    //Get the box in the answer message
    //printf("Finished close handshake: Remote box to send to: %s\n", buffer);

    //Set box assigned to this dest
    connections[con->localBox].Reset();
    //Set the box 
    return 1;

}

/*int
SecurePost::ListenHandshake(int origin)
{
    //Listen for handshake first if not stablished connection already
    char* buffer = new char[MaxPacketSize];
    PacketHeader outPktHdr, inPktHdr;
    MailHeaderSecure outMailHdr, inMailHdr;
    
    Receive(HANDSHAKEREQUESTBOX, &inPktHdr, &inMailHdr, buffer); //First listen handshake

    if(inMailHdr.segments == -1) return -1;

    int avBox = GetAvailableBox(); //synchronize this
    if(avBox == -1) return -1; //Thee's no av box

    printf("Available box!!!: %d\n", avBox);

    //Keep track of connection;
    connections[avBox].;

    std::stringstream str;
    str << avBox;
    const char* message = str.str().c_str();

    outPktHdr.to = origin;
    outPktHdr.from = netAddr;
    outMailHdr.to = HANDSHAKEANSWERBOX;
    outMailHdr.from = HANDSHAKEANSWERBOX;
    outMailHdr.length = strlen(message) + 1;

    Send(outPktHdr, outMailHdr, message);

    printf("Finished handshake listening\n");
    //Wait for ack confirmation?
    return avBox;
}*/


/*int
SecurePost::SendSecure(int dest, char *message)
{
    char* buffer = new char[MaxPacketSize];
    PacketHeader outPktHdr, inPktHdr;
    MailHeaderSecure outMailHdr, inMailHdr;
    int remoteBox;
    
    for(int i = 0; i < MAXREEMISSIONS; i++) {
        printf("Attempt handshake: %d\n", i);
        remoteBox = PerformHandshake(dest);
        if(remoteBox != -1) break;
    }

    
    if(remoteBox == -1) {
        printf("Remote box: %d\n", remoteBox);
        return -1; // if handshake fails don't do anything
    }

    int avBox = GetAvailableBox(); //synchronize this
    if(avBox == -1) return -1;

    //Set unavailable(box)

    outPktHdr.to = dest;
    outPktHdr.from = netAddr;
    outMailHdr.to = remoteBox;
    outMailHdr.from = avBox;
    outMailHdr.length = strlen(message) + 1;

    int i = 0;
    for(i = 0; i <= MAXREEMISSIONS; i++)
    {
        printf("First transmission: %d\n", i);
        Send(outPktHdr, outMailHdr, message);

        Receive(avBox, &inPktHdr, &inMailHdr, buffer);

        if(inMailHdr.segments != -1) break;
    }

    if(i == MAXREEMISSIONS + 1) return -1;
    else return true;

    return true;
}*/

/*int
SecurePost::ReceiveSecure(int origin) {

    int segments = 0;
    int totalSegments = 1;

    //Listen for handshake first if not stablished connection already
    char* buffer = new char[MaxPacketSize];
    PacketHeader outPktHdr, inPktHdr;
    MailHeaderSecure outMailHdr, inMailHdr;

    //int box = ListenHandshake(origin);
    int box = -1;
    if(box == -1) return -1;

   

    do { //keep track of which segment we got
        Receive(box, &inPktHdr, &inMailHdr, buffer); //Listen

        if(inMailHdr.segments == -1) continue;

        segments += 1;
        totalSegments = inMailHdr.segments;

        printf("Sending confirmation to: %d\n",inMailHdr.from);
        //Send ack
        outPktHdr.to = origin;
        outPktHdr.from = netAddr;
        outMailHdr.to = inMailHdr.from;
        outMailHdr.from = box;
        outMailHdr.length = strlen(ACK) + 1;

        Send(outPktHdr, outMailHdr, ACK);
    } while (segments < totalSegments);

    return 1;

}*/

int
SecurePost::OpenConnection(int dest)
{
    int remoteBox;

    remoteBox = PerformHandshake(dest);

    return remoteBox;
}

int
SecurePost::CloseConnection(int con)
{
    int result;

    conLock->Acquire();
    Connection* connect = GetConnection(con);
    result = PerformCloseHandshake(connect);
    conLock->Release();

    return result;
}

int
SecurePost::ReceiveSecure(int org, int timeout, char *buff) {
    //printf("Starting receive secure for origin box: %d\n", org);
    //buff = NULL;
    char buffer[MaxMailSizeSecure];
    PacketHeader outPktHdr, inPktHdr;
    MailHeaderSecure outMailHdr, inMailHdr;
    const char* ack = "ack";
    //printf("Origin: %d\n", org);

    conLock->Acquire();
    Connection* con = GetConnection(org);
    if(con == NULL) {
        //printf("No connection for: %d\n", org);
        conLock->Release();
        return -1;
    }
    //Send  message
    outPktHdr.to = con->remoteAddr;
    outPktHdr.from = netAddr;
    outMailHdr.to = con->remoteBox;
    outMailHdr.from = con->localBox;
    outMailHdr.length = strlen(ack) + 1;
    outMailHdr.ACK = 1;

    //printf("Connection got: %d\n", con->remoteAddr);
    

    Receive(outMailHdr.from, &inPktHdr, &inMailHdr, buffer, timeout);

    if(inMailHdr.segments == -1) {
        conLock->Release();
        //printf("Received bad message in secure post secure receive\n");
        return -1;
    }
    //printf("Received message secure with message %s in box %d\n", buffer, org);

    if(inMailHdr.FIN == 1) {
        RespondCloseHandshake(con);
        conLock->Release();
        return -2;
    }

    conLock->Release();


    Send(outPktHdr, outMailHdr, ack);

    bcopy(buffer, buff, inMailHdr.length);

    return true;
}

int
SecurePost::SendSecure(char *message, int conn) {
    char buffer[MaxMailSizeSecure];
    PacketHeader outPktHdr, inPktHdr;
    MailHeaderSecure outMailHdr, inMailHdr;

    conLock->Acquire();
    Connection* con = GetConnection(conn);

    //Send  message
    outPktHdr.to = con->remoteAddr;
    outPktHdr.from = netAddr;
    outMailHdr.to = con->remoteBox;
    outMailHdr.from = con->localBox;
    outMailHdr.length = strlen(message) + 1;
    outMailHdr.ACK = 0;
    outMailHdr.SYN = 0;
    outMailHdr.FIN = 0;

    conLock->Release();

    int result = SendMessageWithAck(outPktHdr, outMailHdr, &inPktHdr, &inMailHdr, message, buffer);
    return result;
}

//----------------------------------------------------------------------
// SecurePost::Send
//  Concatenate the MailHeader to the front of the data, and pass 
//  the result to the Network for delivery to the destination machine.
//
//  Note that the MailHeaderSecure + data looks just like normal payload
//  data to the Network.
//
//  "pktHdr" -- source, destination machine ID's
//  "mailHdr" -- source, destination MailBoxSecure ID's
//  "data" -- payload message data
//----------------------------------------------------------------------

void
SecurePost::Send(PacketHeader pktHdr, MailHeaderSecure mailHdr, const char* data)
{
    char* buffer = new char[MaxPacketSize]; // space to hold concatenated
                        // mailHdr + data

    if (DebugIsEnabled('n')) {
    printf("Post send: ");
    PrintHeader(pktHdr, mailHdr);
    }
    ASSERT(mailHdr.length <= MaxMailSizeSecure);
    ASSERT(0 <= mailHdr.to && mailHdr.to < numBoxes);
    
    // fill in pktHdr, for the Network layer
    pktHdr.from = netAddr;
    pktHdr.length = mailHdr.length + sizeof(MailHeaderSecure);

    // concatenate MailHeaderSecure and data
    bcopy(&mailHdr, buffer, sizeof(MailHeaderSecure));
    bcopy(data, buffer + sizeof(MailHeaderSecure), mailHdr.length);

    sendLock->Acquire();        // only one message can be sent
                    // to the network at any one time
    network->Send(pktHdr, buffer);
    messageSent->P();           // wait for interrupt to tell us
                    // ok to send the next message
    sendLock->Release();

    delete [] buffer;           // we've sent the message, so
                    // we can delete our buffer
}

//----------------------------------------------------------------------
// SecurePost::Send
//  Retrieve a message from a specific box if one is available, 
//  otherwise wait for a message to arrive in the box.
//
//  Note that the MailHeaderSecure + data looks just like normal payload
//  data to the Network.
//
//
//  "box" -- MailBoxSecure ID in which to look for message
//  "pktHdr" -- address to put: source, destination machine ID's
//  "mailHdr" -- address to put: source, destination MailBoxSecure ID's
//  "data" -- address to put: payload message data
//----------------------------------------------------------------------

void
SecurePost::Receive(int box, PacketHeader *pktHdr, 
                MailHeaderSecure *mailHdr, char* data, bool timeout)
{
        ASSERT((box >= 0) && (box < numBoxes));

        boxes[box].Get(pktHdr, mailHdr, data, timeout);
        
        if(mailHdr->segments != -1) {
            ASSERT(mailHdr->length <= MaxMailSizeSecure);
        }
}

//----------------------------------------------------------------------
// SecurePost::IncomingPacket
//  Interrupt handler, called when a packet arrives from the network.
//
//  Signal the PostalDelivery routine that it is time to get to work!
//----------------------------------------------------------------------

void
SecurePost::IncomingPacket()
{ 
    messageAvailable->V(); 
}

//----------------------------------------------------------------------
// SecurePost::PacketSent
//  Interrupt handler, called when the next packet can be put onto the 
//  network.
//
//  The name of this routine is a misnomer; if "reliability < 1",
//  the packet could have been dropped by the network, so it won't get
//  through.
//----------------------------------------------------------------------

void 
SecurePost::PacketSent()
{ 
    messageSent->V();
}


void
SecurePost::CheckTimeout()
{
    for(int box = 0; box < numBoxes; box++) {
        boxes[box].Check();
    }
}

int
SecurePost::GetAvailableBox() {
    conLock->Acquire();
    for(int box = 2; box < numBoxes; box++) {
        if (connections[box].remoteAddr == -1) {
            printf("Available box given: %d\n", box);
            conLock->Release();
            return box;
        }
    }
    conLock->Release();
    return -1;
}