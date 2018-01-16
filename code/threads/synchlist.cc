// synchlist.cc
//      Routines for synchronized access to a list.
//
//      Implemented by surrounding the List abstraction
//      with synchronization routines.
//
//      Implemented in "monitor"-style -- surround each procedure with a
//      lock acquire and release pair, using condition signal and wait for
//      synchronization.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "synchlist.h"

#include "system.h"

//----------------------------------------------------------------------
// SynchList::SynchList
//      Allocate and initialize the data structures needed for a 
//      synchronized list, empty to start with.
//      Elements can now be added to the list.
//----------------------------------------------------------------------

SynchList::SynchList ()
{
    list = new List ();
    lock = new Lock ("list lock");
    listEmpty = new Condition ("list empty cond");
}

//----------------------------------------------------------------------
// SynchList::~SynchList
//      De-allocate the data structures created for synchronizing a list. 
//----------------------------------------------------------------------

SynchList::~SynchList ()
{
    delete list;
    delete lock;
    delete listEmpty;
}

//----------------------------------------------------------------------
// SynchList::Append
//      Append an "item" to the end of the list.  Wake up anyone
//      waiting for an element to be appended.
//
//      "item" is the thing to put on the list, it can be a pointer to 
//              anything.
//----------------------------------------------------------------------

void
SynchList::Append (void *item)
{
    lock->Acquire ();		// enforce mutual exclusive access to the list
    list->Append (item);
    listEmpty->Signal(lock);	// wake up a waiter, if any
    lock->Release ();
}

//----------------------------------------------------------------------
// SynchList::Remove
//      Remove an "item" from the beginning of the list.  Wait if
//      the list is empty.
// Returns:
//      The removed item. 
//----------------------------------------------------------------------

void *
SynchList::Remove (int timeout)
{
    void *item;
    m_timeout = timeout;
    if (timeout != -1) {
        interrupt->Schedule(&SynchList::HandleTimeout, (int) this, timeout, TimerInt);
    }
    lock->Acquire ();		// enforce mutual exclusion
    if (list->IsEmpty ()) {
        listEmpty->Wait(lock);
    }

    item = list->Remove ();
    lock->Release ();
    return item;
}

//----------------------------------------------------------------------
// SynchList::Mapcar
//      Apply function to every item on the list.  Obey mutual exclusion
//      constraints.
//
//      "func" is the procedure to be applied.
//----------------------------------------------------------------------

void
SynchList::Mapcar (VoidFunctionPtr func)
{
    lock->Acquire ();
    list->Mapcar (func);
    lock->Release ();
}

void SynchList::HandleTimeout(int arg)
{
    auto oldLevel = interrupt->SetLevel(IntOff);
    SynchList *th = (SynchList *)arg;
    if (0 == th->lock->Acquire(true)) {

        th->listEmpty->Signal(th->lock);
        th->lock->Release();

    } else {
        //TODO: Fix this bullshit -- pass timeout to this function somehow
        interrupt->Schedule(&SynchList::HandleTimeout, arg, th->m_timeout, TimerInt);
    }
    interrupt->SetLevel(oldLevel);
}
