#ifndef INTERRUPTMOCK_H
#define INTERRUPTMOCK_H

//#include "interrupt.h"

class InterruptMock {
public:
    InterruptMock() { }
    ~InterruptMock() { }

    bool halted = false;

    void Halt() {
        halted = true;
    }
};


#endif // INTERRUPTMOCK_H

