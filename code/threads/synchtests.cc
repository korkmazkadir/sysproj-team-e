#include <stdio.h>
#include "synch.h"
#include "system.h"
#include <unistd.h>

Condition condvar;
Lock mutex;

int data_ready = 0;

static void consumer (int notused)
{
    (void)notused;
    printf ("In consumer thread...\n");
    while (1) {
        mutex.Acquire();
        while (0 >= data_ready) {
            condvar.Wait(&mutex);
        }
        // process data
        --data_ready;
        printf ("consumer:  data_ready %d\n\n", data_ready);
        condvar.Signal(&mutex);
        mutex.Release();
    }
}

static void producer (int notused)
{
    (void)notused;
    printf ("In producer thread...\n");
    while (1) {
        for (int ii = 0; ii < 100000000; ++ii) { ; }
        mutex.Acquire();
        while (0 < data_ready) {
            condvar.Wait(&mutex);
        }
        ++data_ready;
        printf ("producer:  data_ready %d\n", data_ready);
        condvar.Signal(&mutex);
        mutex.Release();
    }
}

void RunSyncTests() {
    printf ("Starting consumer/producer example...\n");

    Thread *prod_thread = new Thread("", NULL, NULL, NULL);
    prod_thread->Fork(&producer, 0);

    Thread *cons_thread = new Thread("", NULL, NULL, NULL);
    cons_thread->Fork(&consumer, 0);

    currentThread->Yield();

    // let the threads run for a bit
//    for (int ii = 0; ii < 10000000; ++ii) { ; }
}
