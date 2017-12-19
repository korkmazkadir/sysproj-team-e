#include "semaphore.h"
#include "syscall.h"
#include "utility.h"
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

/*!
 * \fn producer
 * Implements a producer in a producer-consumer problem
 * Puts values from 0 to 19 to the buffer and exits
 */
void producer(void *_) {
    (void)_;
    int data;
    int ii = 0;
    for (ii = 0; ii < 20; ++ii) {
        data = ii;
        int jj;
        for (jj = 0; jj < 10000; ++jj) { ; }

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

/*!
 * \fn consumer
 * Implements a consumer in a producer-consumer problem
 * Reads the value from the buffer that was put by producer
 */
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
            static int expectedData = 0;
            _ASSERT(expectedData == data);
            ++expectedData;
        }
        SemPost(&outputMutex);


    }
}

/*!
 * Tries to create \c SEM_VALUE_MAX + 1 semaphores and
 * checks that first SEM_VALUE_MAX calls succeeds and the
 * next call fails.
 *
 * Does the same for SemDestroy
 */
static void testManySemaphoresInits() {
    int ii = 0;
    sem_t arr[SEM_VALUE_MAX + 1];
    for (ii = 0; ii < SEM_VALUE_MAX + 1; ++ii) {
        int retVal = SemInit(&arr[ii], 1);

        if (ii < SEM_VALUE_MAX) {
            _ASSERT(0 == retVal);
        } else {
            _ASSERT(-1 == retVal);
        }
    }

    for (ii = 0; ii < SEM_VALUE_MAX + 1; ++ii) {
        int retVal = SemDestroy(&arr[ii]);
        if (ii < SEM_VALUE_MAX) {
            _ASSERT(0 == retVal);
        } else {
            _ASSERT(-1 == retVal);
        }
    }

}

int main() {

    int semInitStatus = 0;

    testManySemaphoresInits();

    // Expect successful initialization - output 0
    semInitStatus = SemInit(&fillSem, 0);
    _ASSERT(0 == semInitStatus);

    // Expect successful initialization - output 0
    semInitStatus = SemInit(&emptSem, N);
    _ASSERT(0 == semInitStatus);

    // Expect successful initialization - output 0
    semInitStatus = SemInit(&mutex, 1);
    _ASSERT(0 == semInitStatus);

    // Expect successful initialization - output 0
    semInitStatus = SemInit(&outputMutex, 1);
    _ASSERT(0 == semInitStatus);

    // Expect -3 error code
    semInitStatus = SemInit(&outputMutex, 1);
    _ASSERT(-3 == semInitStatus);

    // Check that passing a negative value to SemInit results in a correct return code
    sem_t oneMoreSemaphore;
    semInitStatus = SemInit(&oneMoreSemaphore, -1);
    _ASSERT(-2 == semInitStatus);

    int prodId = UserThreadCreate(&producer, 0);

    UserThreadCreate(&consumer, 0);
    UserThreadCreate(&consumer, 0);
    UserThreadCreate(&consumer, 0);

    int joinSuccess = UserThreadJoin(prodId);
    _ASSERT(0 == joinSuccess);

    int destroySuccess;
    destroySuccess = SemDestroy(&emptSem);
    _ASSERT(0 == destroySuccess);

    destroySuccess = SemDestroy(&fillSem);
    _ASSERT(0 == destroySuccess);

    destroySuccess = SemDestroy(&emptSem);
    _ASSERT(-1 == destroySuccess);

    destroySuccess = SemDestroy(&fillSem);
    _ASSERT(-1 == destroySuccess);

    return 0;
}
