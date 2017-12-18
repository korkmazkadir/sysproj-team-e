#include "semaphore.h"
#include "syscall.h"

#define N 10

sem_t fillSem;
sem_t emptSem;
sem_t mutex;
sem_t outputMutex;

static int buffer[N];
static int writeIx = 0;
static int readIx = 0;

void producer(void *_) {
    (void)_;
    int data;
    while (1) {
        SynchGetInt(&data);
        SemWait(&emptSem);
        {
            SemWait(&mutex);
            {
                buffer[writeIx] = data;
                ++writeIx;
                if (writeIx >= N) {
                    writeIx = 0;
                }
            }
            SemPost(&mutex);
        }
        SemPost(&fillSem);
    }
}

void consumer(void *_) {
    (void)_;
    int data;

    while (1) {

        int ii = 0;
        for (ii = 0; ii < 500000; ++ii) { ; }
        SemWait(&fillSem);

        SemWait(&mutex);
        {
            data = buffer[readIx];
            ++readIx;
            if (readIx >= N) {
                readIx = 0;
            }
        }
        SemPost(&mutex);

        SemPost(&emptSem);

        SemWait(&outputMutex);
        {
            SynchPutString("Thread "); SynchPutInt(UserThreadSelfId()); SynchPutString(" outputs: ");
            SynchPutInt(data);
            PutChar('\n');
        }
        SemPost(&outputMutex);

    }
}

int main() {

    SemInit(&fillSem, 0);
    SemInit(&emptSem, N);
    SemInit(&mutex, 1);
    SemInit(&outputMutex, 1);

    UserThreadCreate(&producer, 0);

    UserThreadCreate(&consumer, 0);
    UserThreadCreate(&consumer, 0);
    UserThreadCreate(&consumer, 0);

    return 0;
}
