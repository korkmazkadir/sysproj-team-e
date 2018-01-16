#include "syscall.h"
#include "utility.h"

#define BUF_SIZE 32
#define CONNECTIONS 16

static int first_thread_exited_times = 0;
static int second_thread_exited_times = 0;

static sem_t first_thread_sem;
static sem_t second_thread_sem;

static void first_thread(void  *arg) {
    int port = (int)arg;
    const char *expectedMesssage = "message from snd to rcv";
    int connId = NetworkConnectAsClient(1, port);
    _ASSERT(connId >= 0);

    char buffer[BUF_SIZE];
    for (int ii = 0; ii < BUF_SIZE; ++ii) {
        buffer[ii] = 0;
    }
    int bytesRead = NetworkReceiveFromByConnId(connId, buffer);
    _ASSERT(bytesRead == strlen(expectedMesssage));
    _ASSERT(stringCompare(buffer, expectedMesssage) == 0);

    _ASSERT(0 == SemWait(&first_thread_sem));
    ++first_thread_exited_times;
    _ASSERT(0 == SemPost(&first_thread_sem));
}

static void second_thread(void *arg) {
    int port = (int)arg;
    int connId = NetworkConnectAsServer(port);
    _ASSERT(connId >= 0);
    const char *stringToSend = "message from rcv to snd";
    int retVal = NetworkSendToByConnId(connId, stringToSend, strlen(stringToSend));
    _ASSERT(0 == retVal);
    _ASSERT(0 == SemWait(&second_thread_sem));
    ++second_thread_exited_times;
    _ASSERT(0 == SemPost(&second_thread_sem));
}

int main() {

    int status_1 = SemInit(&first_thread_sem, 1);
    _ASSERT(0 == status_1);

    int status_2 = SemInit(&second_thread_sem, 1);
    _ASSERT(0 == status_2);

    int tids[CONNECTIONS * 2];

    int ii = 0;
    for (ii = 0; ii < CONNECTIONS; ++ii) {
        tids[ii * 2 + 0] = UserThreadCreate(&first_thread, (void *)(2 * ii));
        tids[ii * 2 + 1] = UserThreadCreate(&second_thread, (void *)(2 * ii + 1));
    }

    for (ii = 0; ii < CONNECTIONS * 2; ++ii) {
        UserThreadJoin(tids[ii]);
    }

    _ASSERT(first_thread_exited_times == CONNECTIONS);
    _ASSERT(second_thread_exited_times == CONNECTIONS);

    _ASSERT(NetworkConnectAsServer(2 * CONNECTIONS) < 0);
    _ASSERT(0 == SemDestroy(&first_thread_sem));
    _ASSERT(0 == SemDestroy(&second_thread_sem));

    return 0;
}
