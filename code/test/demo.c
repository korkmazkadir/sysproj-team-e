#include "semaphore.h"
#include "syscall.h"
#include "utility.h"
#include "nachos_stdio.h"

#define N 40

sem_t fillSem;
sem_t emptSem;
sem_t mutex;
sem_t outputMutex;
sem_t consumerDone;

static int buffer[N];
static int writeIx = 0;
static int readIx = 0;

char producerExited = 0;

OpenFileId msgFd;

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
            int d = data;
            int numBytes = 5;
            //numBytes = Write((char*)&d, 1, msgFd);
            if (numBytes <= 0) {
                _printf("demo: Write failed\n");
            }
            _printf("cons wrote %d to msg file\n", d);
        }
        SemPost(&outputMutex);


    }
}


void func(void *_) {

    _printf("weee\n");
    
}

int main() {
    int e = 1; //for Exit() error code
    int semInitStatus;
    if (CreateDirectory("newDir") == -1) {
        _printf("demo: create dir of newDir failed\n");
        Exit(e++);
    }
    //ListDirectoryContent(".");

    if ((msgFd = Open("newDir/message")) == -1) {
        _printf("demo: Open newDir/message failed\n");
        Exit(e++);         
    }
    
    if (ChangeDirectory("newDir") == -1) {
        _printf("demo: chnageDirectory newDir failed\n");
        Exit(e++);         
    }
    /*
      int tid = UserThreadCreate(func, 0);
    int val = UserThreadJoin(tid);
    if (tid < 0 || val < 0) {
        _printf("nope");
        Exit(50);
    }
    _printf("donee");
    
    goto skip;    */
    
    
    
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

    int c1 = UserThreadCreate(&consumer, 0);
    int c2 = UserThreadCreate(&consumer, 0);
    int c3 = UserThreadCreate(&consumer, 0);

    int joinSuccess = UserThreadJoin(prodId);
    _ASSERT(0 == joinSuccess);
    
    joinSuccess = UserThreadJoin(c1);
    _ASSERT(0 == joinSuccess);
    
    joinSuccess = UserThreadJoin(c2);
    _ASSERT(0 == joinSuccess);
    
    joinSuccess = UserThreadJoin(c3);
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
    
    
    
    

    SemDestroy(&emptSem);
    SemDestroy(&fillSem);
    SemDestroy(&emptSem);
    SemDestroy(&fillSem);
    _printf("---    End of Demo    ---\n");
//skip:
    return 0;
}
