// synch.cc 
//      Routines for synchronizing threads.  Three kinds of
//      synchronization routines are defined here: semaphores, locks 
//      and condition variables (the implementation of the last two
//      are left to the reader).
//
// Any implementation of a synchronization routine needs some
// primitive atomic operation.  We assume Nachos is running on
// a uniprocessor, and thus atomicity can be provided by
// turning off interrupts.  While interrupts are disabled, no
// context switch can occur, and thus the current thread is guaranteed
// to hold the CPU throughout, until interrupts are reenabled.
//
// Because some of these routines might be called with interrupts
// already disabled (Semaphore::V for one), instead of turning
// on interrupts at the end of the atomic operation, we always simply
// re-set the interrupt state back to its original value (whether
// that be disabled or enabled).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "synch.h"
#include "system.h"
#include <limits.h>

//----------------------------------------------------------------------
// Semaphore::Semaphore
//      Initialize a semaphore, so that it can be used for synchronization.
//
//      "debugName" is an arbitrary name, useful for debugging.
//      "initialValue" is the initial value of the semaphore.
//----------------------------------------------------------------------

Semaphore::Semaphore (const char *debugName, int initialValue)
{
    name = debugName;
    value = initialValue;
    queue = new List;
}

//----------------------------------------------------------------------
// Semaphore::Semaphore
//      De-allocate semaphore, when no longer needed.  Assume no one
//      is still waiting on the semaphore!
//----------------------------------------------------------------------

Semaphore::~Semaphore ()
{
    delete queue;
}

//----------------------------------------------------------------------
// Semaphore::P
//      Wait until semaphore value > 0, then decrement.  Checking the
//      value and decrementing must be done atomically, so we
//      need to disable interrupts before checking the value.
//
//      Note that Thread::Sleep assumes that interrupts are disabled
//      when it is called.
//----------------------------------------------------------------------

void Semaphore::P()
{
    IntStatus oldLevel = interrupt->SetLevel (IntOff);	// disable interrupts

    while (value == 0)
    {				// semaphore not available
        queue->Append ((void *) currentThread);	// so go to sleep
        currentThread->Sleep ();
    }
    value--;			// semaphore available,
    // consume its value

    (void) interrupt->SetLevel (oldLevel);	// re-enable interrupts
}

//----------------------------------------------------------------------
// Semaphore::V
//      Increment semaphore value, waking up a waiter if necessary.
//      As with P(), this operation must be atomic, so we need to disable
//      interrupts.  Scheduler::ReadyToRun() assumes that threads
//      are disabled when it is called.
//----------------------------------------------------------------------

int Semaphore::V ()
{
    int retVal = 0;
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel (IntOff);

    thread = (Thread *) queue->Remove ();
    if (likely(thread != NULL))	{	// make thread ready, consuming the V immediately
        scheduler->ReadyToRun (thread);
    }

    if (unlikely(value == INT_MAX)) {
        retVal = -1;
        goto early_exit;
    }

    value++;
early_exit:
    (void) interrupt->SetLevel (oldLevel);
    return retVal;
}


Lock::Lock (const char *debugName):
    m_name(debugName)
  , m_currentThread(nullptr)
{
}

Lock::~Lock ()
{
}

void Lock::Acquire ()
{
    IntStatus oldLevel = interrupt->SetLevel (IntOff);	// disable interrupts

    while (m_currentThread)
    {
        m_queue.Append ((void *) currentThread);	// so go to sleep
        currentThread->Sleep ();
    }
    m_currentThread = currentThread;

    (void) interrupt->SetLevel (oldLevel);	// re-enable interrupts
}

void Lock::Release ()
{
    IntStatus oldLevel = interrupt->SetLevel (IntOff);
    Thread *thread;
    /*! Only allow releasing the mutex to the thread that acquired it */
    if (unlikely(!isHeldByCurrentThread())) {
        goto early_exit;
    }

    thread = static_cast<Thread *>(m_queue.Remove ());
    if (likely(thread != nullptr))	{	// make thread ready, consuming the V immediately
        scheduler->ReadyToRun (thread);
    }

    m_currentThread = nullptr;

early_exit:
    (void) interrupt->SetLevel (oldLevel);
}

bool Lock::isHeldByCurrentThread() const
{
    bool retVal = (currentThread && (currentThread == m_currentThread));
    return retVal;
}

Condition::Condition (const char *debugName):
    m_name(debugName)
  , m_threadToUnlock(nullptr)
    { /* do nothing */ }

Condition::~Condition () {
}

void Condition::Wait (Lock * conditionLock) {
    IntStatus oldLevel = interrupt->SetLevel (IntOff);

    /*! nullptr is specified as parameter */
    if (unlikely(!conditionLock)) {
        goto error_exit;
    }

    /*!
     * current thread *MUST* hold a lock in order to Wait
     * this also handles a case when current thread is NULL
     */
    if (unlikely(!conditionLock->isHeldByCurrentThread())) {
        goto error_exit;
    }

    m_queueBlocked.Append((void *)currentThread);

    conditionLock->Release();
    currentThread->Sleep();
    conditionLock->Acquire();

error_exit:
    (void)interrupt->SetLevel(oldLevel);
}

void Condition::Signal (Lock * conditionLock) {

    IntStatus oldLevel = interrupt->SetLevel(IntOff);
    Thread *thread;
    if (unlikely(!conditionLock)) {
        goto error_exit;
    }

    if (unlikely(!conditionLock->isHeldByCurrentThread())) {
        goto error_exit;
    }

    thread = static_cast<Thread *>(m_queueBlocked.Remove());

    if (!thread) {
        //printf("No thread to unblock \n");
        goto error_exit;
    }
    scheduler->ReadyToRun(thread);

error_exit:
    (void)interrupt->SetLevel(oldLevel);
}

void Condition::Broadcast (Lock * conditionLock) {
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    int blockedQueueSize = m_queueBlocked.Size();
    for (int ii = 0; ii < blockedQueueSize; ++ii) {
        Signal(conditionLock);
    }

    (void)interrupt->SetLevel(oldLevel);

}
