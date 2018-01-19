// nettest.cc 
//	Test out message delivery between two "Nachos" machines,
//	using the Post Office to coordinate delivery.
//
//	Two caveats:
//	  1. Two copies of Nachos must be running, with machine ID's 0 and 1:
//		./nachos -m 0 -o 1 &
//		./nachos -m 1 -o 0 &
//
//	  2. You need an implementation of condition variables,
//	     which is *not* provided as part of the baseline threads 
//	     implementation.  The Post Office won't work without
//	     a correct implementation of condition variables.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#include "system.h"
#include "network.h"
#include "post.h"
#include "interrupt.h"
#include "SynchPost.h"


#if 0
// Test out message delivery, by doing the following:
//	1. send a message to the machine with ID "farAddr", at mail box #0
//	2. wait for the other machine's message to arrive (in our mailbox #0)
//	3. send an acknowledgment for the other machine's message
//	4. wait for an acknowledgement from the other machine to our
//	    original message

void
MailTest(int farAddr)
{
    PacketHeader outPktHdr, inPktHdr;
    MailHeader outMailHdr, inMailHdr;
    const char *data = "Hello there!";
    const char *ack = "Got it!";
    char buffer[MaxMailSize];

    // construct packet, mail header for original message
    // To: destination machine, mailbox 0
    // From: our machine, reply to: mailbox 1
    outPktHdr.to = farAddr;
    outMailHdr.to = 2;
    outMailHdr.from = 3;
    outMailHdr.length = strlen(data) + 1;

    // Send the first message
    postOffice->Send(outPktHdr, outMailHdr, data);

    // Wait for the first message from the other machine
    postOffice->Receive(2, &inPktHdr, &inMailHdr, buffer);
    printf("Got \"%s\" from %d, box %d\n",buffer, inPktHdr.from, inMailHdr.from);
    fflush(stdout);

    // Send acknowledgement to the other machine (using "reply to" mailbox
    // in the message that just arrived
    outPktHdr.to = inPktHdr.from;
    outMailHdr.to = inMailHdr.from;
    outMailHdr.length = strlen(ack) + 1;
    postOffice->Send(outPktHdr, outMailHdr, ack);

    // Wait for the ack from the other machine to the first message we sent.
    postOffice->Receive(3, &inPktHdr, &inMailHdr, buffer);
    printf("Got \"%s\" from %d, box %d\n",buffer,inPktHdr.from,inMailHdr.from);
    fflush(stdout);

    // Then we're done!
    interrupt->Halt();
}

void Mail10Test(int farAddr) {
    PacketHeader inPktHeader;
    PacketHeader outPktHeader;
    MailHeader outMailHeader;
    MailHeader inMailHeader;

    const char *formatSend = "Sending message # %d";
    const char *formatAck = "ACK message # %d";

    char buf[MaxMailSize];

    for (int ii = 0; ii < 10; ++ii) {


        char dataBuf[80] = { 0 };
        sprintf(dataBuf, formatSend, ii);


        outPktHeader.to = farAddr;

        outMailHeader.to = 2;
        outMailHeader.from = 3;
        outMailHeader.length = strlen(dataBuf);

        postOffice->Send(outPktHeader, outMailHeader, dataBuf);

        postOffice->Receive(2, &inPktHeader, &inMailHeader, buf);
        printf("Got \"%s\" from %d, box %d\n", buf, inPktHeader.from, inMailHeader.from);

        fflush(stdout);

        // Send acknowledgement to the other machine (using "reply to" mailbox
        // in the message that just arrived

        char dataBuf2[80] = { 0 };
        sprintf(dataBuf2, formatAck, ii);

        outPktHeader.to = inPktHeader.from;
        outMailHeader.to = inMailHeader.from;
        outMailHeader.length = strlen(dataBuf2) + 1;
        postOffice->Send(outPktHeader, outMailHeader, dataBuf2);

        // Wait for the ack from the other machine to the first message we sent.
        postOffice->Receive(3, &inPktHeader, &inMailHeader, buf);
        printf("Got \"%s\" from %d, box %d\n",buf,inPktHeader.from,inMailHeader.from);
        fflush(stdout);

    }

    interrupt->Halt();


}

void MailTimeoutTest(int farAddr) {
    (void)farAddr;
    PacketHeader inPktHeader;
    MailHeader inMailHeader;

    char buf[MaxMailSize];
    postOffice->Receive(2, &inPktHeader, &inMailHeader, buf);

}

#endif

void MailSynchTest1(int farAddr) {
    const char *longData = "This is the example of the data chunk that is larger than the max size of the packet...........................";
    synchPost->SendTo(farAddr, 0, longData, strlen(longData));

}

void MailSynchTest2(int farAddr) {
    while(1) {
        char data[512] = { 0 };
        synchPost->ReceiveFrom(0, data);
        printf("DATA :: %s \n", data);
    }
}
