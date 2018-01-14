#ifndef MACHINE_MOCK_H
#define MACHINE_MOCK_H

#include <array>
#include <map>
#include "machine.h"

class MachineMock {
  public:

    MachineMock(bool) { }
    ~MachineMock() { }
// Routines callable by the Nachos kernel
    void Run() { }

    int ReadRegister(int num) {
        return registers[num];
    }

    void WriteRegister(int num, int value) {
        registers[num] = value;
    }

    bool ReadMem(int addr, int size, int* value) {
        (void)size;
        if (memory.find(addr) == memory.end()) {
            return false;
        }

        *value = memory[addr];

        return true;
    }

    bool WriteMem(int addr, int size, int value) {
        (void)size;
        memory[addr] = value;
        return true;
    }
                    // Read or write 1, 2, or 4 bytes of virtual
                // memory (at addr).  Return FALSE if a
                // correct translation couldn't be found.



    void Debugger() { }
    void DumpState() { }

    std::array<int, NumTotalRegs> registers;
    std::map<int, int> memory;
    char mainMemory[MemorySize];
};

#endif // MACHINE_MOCK_H

