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
//#include "post.h"
#include "securepost.h"
#include "interrupt.h"

// Test out message /, by doing the following:
//	1. send a message to the machine with ID "farAddr", at mail box #0
//	2. wait for the other machine's message to arrive (in our mailbox #0)
//	3. send an acknowledgment for the other machine's message
//	4. wait for an acknowledgement from the other machine to our 
//	    original message

void
MailTest(int farAddr)
{
    PacketHeader outPktHdr, inPktHdr;
    MailHeaderSecure outMailHdr, inMailHdr;
    const char *data = "Hello there!";
    const char *ack = "Got it!";
    char buffer[MaxMailSizeSecure];

    for(int j = 0; j < 1; j++) {

        // construct packet, mail header for original message
        // To: destination machine, mailbox 0
        // From: our machine, reply to: mailbox 1
        outPktHdr.to = farAddr;     
        //outMailHdr.length = strlen(data) + 1;
        outMailHdr.segments = 1;


        for(int i = 0; i < 3; i+=2) {
            outMailHdr.to = i;
            outMailHdr.from = i+1;
            // Send the first message
            postOffice->Send(outPktHdr, outMailHdr, data);
        }

        
        for(int i = 0; i < 4; i+=2) {
           // Wait for the first message from the other machine
           postOffice->Receive(i, &inPktHdr, &inMailHdr, buffer, true);
           if(inMailHdr.segments != -1) {
               printf("Got \"%s\" from %d, box %d\n",buffer,inPktHdr.from,inMailHdr.to);
               fflush(stdout);

               // Send acknowledgement to the other machine (using "reply to" mailbox
               // in the message that just arrived
               outPktHdr.to = inPktHdr.from;
               outMailHdr.to = inMailHdr.from;
               outMailHdr.from = i+1;
               //outMailHdr.length = strlen(ack) + 1;
               outMailHdr.segments = 1;
               postOffice->Send(outPktHdr, outMailHdr, ack);
           } else {
                printf("Timeout from %d\n", i);
           }
        }

        for(int i = 1; i < 4; i+=2) {
            // Wait for the ack from the other machine to the first message we sent.
           postOffice->Receive(i, &inPktHdr, &inMailHdr, buffer, true);
           if(inMailHdr.segments != -1) {
               printf("Got \"%s\" from %d, box %d\n",buffer,inPktHdr.from,inMailHdr.from);
               fflush(stdout);
           } else {
                printf("Timeout from %d\n", i);
           }
        }
    }

    // Then we're done!
    interrupt->Halt();
}

void
RingTopology(int myID, int n) {

    PacketHeader outPktHdr, inPktHdr;
    MailHeaderSecure outMailHdr, inMailHdr;
    char data[SIZEOFSEGMENT] = "Token";
    char buffer[MaxMailSizeSecure];
    int next = (myID + 1) % n;
    
    if(myID == 0) {
        outPktHdr.to = next;     
        //outMailHdr.length = strlen(data) + 1;
        outMailHdr.to = 0;
        outMailHdr.from = 1;
        outMailHdr.segments = 1;

        // Send the token
        postOffice->Send(outPktHdr, outMailHdr, data);
    }

    postOffice->Receive(0, &inPktHdr, &inMailHdr, buffer, true);
    printf("Got \"%s\" from %d, box %d\n",buffer,inPktHdr.from,inMailHdr.to);
    fflush(stdout);

    if(myID == 0) {

        interrupt->Halt();

    } else {

        Delay(3);

        printf("Next is %d\n", next);
        fflush(stdout);

        outPktHdr.to = next;     
        //outMailHdr.length = inMailHdr.length;
        outMailHdr.to = 0;
        outMailHdr.from = 1;
        outMailHdr.segments = 1;

        // Send the token
        postOffice->Send(outPktHdr, outMailHdr, buffer);

        interrupt->Halt();
    }

    
}

void
MailTest0(int farAddr)
{
    PacketHeader outPktHdr, inPktHdr;
    MailHeaderSecure outMailHdr, inMailHdr;
    char data[SIZEOFSEGMENT] = "Hello";
    char ack[SIZEOFSEGMENT] = "Gotit";
    char buffer[MaxMailSizeSecure];

    // construct packet, mail header for original message
    // To: destination machine, mailbox 0
    // From: our machine, reply to: mailbox 1
    outPktHdr.to = farAddr;     
    outMailHdr.to = 0;
    outMailHdr.from = 1;
    outMailHdr.segments = 1;
    //outMailHdr.length = strlen(data) + 1;

    // Send the first message
    postOffice->Send(outPktHdr, outMailHdr, data); 

    // Wait for the first message from the other machine
    postOffice->Receive(0, &inPktHdr, &inMailHdr, buffer, true);
    printf("Got \"%s\" from %d, box %d\n",buffer,inPktHdr.from,inMailHdr.from);
    fflush(stdout);

    // Send acknowledgement to the other machine (using "reply to" mailbox
    // in the message that just arrived
    outPktHdr.to = inPktHdr.from;
    outMailHdr.to = inMailHdr.from;
    outMailHdr.segments = 1;
    //outMailHdr.length = strlen(ack) + 1;
    postOffice->Send(outPktHdr, outMailHdr, ack); 

    // Wait for the ack from the other machine to the first message we sent.
    postOffice->Receive(1, &inPktHdr, &inMailHdr, buffer, true);
    printf("Got \"%s\" from %d, box %d\n",buffer,inPktHdr.from,inMailHdr.from);
    fflush(stdout);

    // Then we're done!
    interrupt->Halt();
}
