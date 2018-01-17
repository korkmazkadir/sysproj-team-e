#include "syscall.h"

sem_t listSem;

void acceptThread(void *ptr) {
    int * l = (int *)ptr;
    while(1) {
        //SynchPutString("ACCEPTING HANDSHAKE\n");
        int con = Accept();
        if(con != -1) {
            for(int i = 0; i < 10; i++) {
                //SemWait(&listSem);
                if(l[i] == -1) {
                    l[i] = con;
                    break;
                }
                //SemPost(&listSem);
            }
        }
    }
}

void acceptMessages(void *ptr) {
    int * l = (int *)ptr;
    char buff[100];
    while(1) {
        for(int i = 0; i < 10; i++) {
            //SemWait(&listSem);
            if(l[i] != -1) {
                int result = Receive(l[i], 1, buff);
                if(result == -2) {
                    SynchPutString("Connection in position closed\n");

                    //cons[]
                } else if(result > 0) {
                    SynchPutString("Message Received for connection in position\n");
                }
            }
            //SemPost(&listSem);
        }
    }
}

void printstatus(int *cons) {
    SynchPutString("ConnectionStatus: ");
    for(int i = 0; i < 10; i++) {
        SynchPutInt(cons[i]);
        SynchPutString(",");

    }
    SynchPutString("\n");
}

int main() {
    char buff[100];
    int cons[10];
    for(int i = 0; i < 10; i++) {
        cons[i] = -1;
    }
    SemInit(&listSem, 1);
    int pid = UserThreadCreate(&acceptThread, (void *)cons);
    //int pid2 = UserThreadCreate(&acceptMessages, (void *)cons);
    printstatus(cons);
    
    while(1) {
        for(int i = 0; i < 10; i++) {
            //SemWait(&listSem);
            if(cons[i] != -1) {
                /*SynchPutString("Receiving in ");
                SynchPutInt(cons[i]);
                SynchPutString("\n");*/
                //printstatus(cons);
                int result = Receive(cons[i], 1, buff);
                if(result == -2) {
                    cons[i] = -1;
                    //printstatus(cons);
                    SynchPutString("Connection in position closed for ");
                    SynchPutInt(i);
                    SynchPutString("\n");
                } else if(result > 0 && buff != (void *)0) {
                    SynchPutString("Received message: ");
                    SynchPutString(buff);
                    SynchPutString("\n");
                }
            }
            //SemPost(&listSem);
        }
    }
    UserThreadJoin(pid);
    //UserThreadJoin(pid2);
    //_ASSERT(ret == -2);
    return 0;
}