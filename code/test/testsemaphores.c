#include "semaphore.h"
#include "syscall.h"
#include "nachos_stdio.h"

#define N 40

sem_t fillSem;
sem_t emptSem;
sem_t mutex;
sem_t outputMutex;

static int buffer[N];
static int writeIx = 0;
static int readIx = 0;

char producerExited = 0;

void producer(void *_) {
    (void)_;
    int data;
    int ii = 0;
    for (ii = 0; ii < 20; ++ii) {
        data = ii;
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
    producerExited = 1;
}

void consumer(void *_) {
    (void)_;
    int data;

    while (1) {

        int ii = 0;
        for (ii = 0; ii < 100000; ++ii) { ; }

        SemWait(&fillSem);

        SemWait(&mutex);
        {
            if (producerExited && readIx == writeIx) {
                SemPost(&mutex);
                UserThreadExit();
            }
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

    int semInitStatus = 0;
    _printf("%d Kadir Korkmaz %s", 1234, "Hello There :) \n");

    // Expect successful initialization - output 0
    semInitStatus = SemInit(&fillSem, 0);
    _printf("K %d \n", semInitStatus);

    // Expect successful initialization - output 0
    semInitStatus = SemInit(&emptSem, N);
    _printf("K %d\n", semInitStatus);

    // Expect successful initialization - output 0
    semInitStatus = SemInit(&mutex, 1);
    _printf("K %d\n", semInitStatus);

    // Expect successful initialization - output 0
    semInitStatus = SemInit(&outputMutex, 1);
    _printf("K %d\n", semInitStatus);

    // Expect -3 error code
    semInitStatus = SemInit(&outputMutex, 1);
    _printf("%d\n", semInitStatus);

    int prodId = UserThreadCreate(&producer, 0);

    UserThreadCreate(&consumer, 0);
    UserThreadCreate(&consumer, 0);
    UserThreadCreate(&consumer, 0);

    UserThreadJoin(prodId);
    SemDestroy(&emptSem);
    SemDestroy(&fillSem);

    return 0;
}
