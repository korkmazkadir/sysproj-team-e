#include "copyright.h"
#include "securepost.h"
#include "system.h"


#include <strings.h> /* for bzero */

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
    timeout = 1000000000;
    initialTicks = -1;
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
MailBoxSecure::Get(PacketHeader *pktHdr, MailHeaderSecure *mailHdr, char *data) 
{ 

    DEBUG('n', "Waiting for mail in MailBoxSecure\n");
    //StartCountingTimeout();
    MailSecure *mail = (MailSecure *) messages->Remove();   // remove message from list;
                        // will wait if list is empty
    if(mail == NULL) {
        mailHdr->segments = -1;
        return;
    }

    //StopCountingTimeout();

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
        if (DebugIsEnabled('n')) {
        printf("Putting mail into MailBoxSecure: ");
        PrintHeader(pktHdr, mailHdr);
        }

    // check that arriving message is legal!
    ASSERT(0 <= mailHdr.to && mailHdr.to < numBoxes);
    ASSERT(mailHdr.length <= MaxMailSizeSecure);

    // put into MailBoxSecure
        boxes[mailHdr.to].Put(pktHdr, mailHdr, buffer + sizeof(MailHeaderSecure));
    }
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
                MailHeaderSecure *mailHdr, char* data)
{
    ASSERT((box >= 0) && (box < numBoxes));

    boxes[box].Get(pktHdr, mailHdr, data);
    
    if(mailHdr->segments != -1) {ASSERT(mailHdr->length <= MaxMailSizeSecure);}
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
