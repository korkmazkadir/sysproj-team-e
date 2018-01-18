#include "test_utils.h"
#include "machine_mock.h"
#include "synchconsole_mock.h"
#include "InterruptMock.h"

#define Machine MachineMock
#define SynchConsole SynchConsoleMock
#define Interrupt InterruptMock
#define MACHINE_H
#define TLB_H
#define SYNCHCONSOLE_H
#define INTERRUPT_H

#include "exception.cc"

static int init_prev_instr = 4;
static int init_cur_instr = 12;
static int init_next_instr = 20;

/*!
 * To be called only from test cases
 */
#define VerifyPCCounter \
    EXPECT_EQ(init_cur_instr, machine->registers[PrevPCReg]); \
    EXPECT_EQ(init_next_instr, machine->registers[PCReg]); \
    EXPECT_EQ(init_next_instr + 4, machine->registers[NextPCReg])

void TestUtils_SetUp() {
    machine = new Machine(false);
    syncConsole = new SynchConsole(NULL, NULL);
    interrupt = new InterruptMock();

    machine->registers[PrevPCReg] = init_prev_instr;
    machine->registers[PCReg] = init_cur_instr;
    machine->registers[NextPCReg] = init_next_instr;
}

void TestUtils_TearDown() {
    delete interrupt;
    delete syncConsole;
    delete machine;
}

TEST_LIST_BEGIN

DECLARE_TEST_BEGIN(ExceptionPutChar)
    //Arrange
    machine->WriteRegister(2, SC_PutChar);
    machine->WriteRegister(4, (int)'c');

    //Act
    ExceptionHandler(SyscallException);

    //Assert
    EXPECT_EQ('c', syncConsole->lastPutChar);
    VerifyPCCounter;
DECLARE_TEST_END(ExceptionPutChar)

DECLARE_TEST_BEGIN(ExceptionHalt)
    //Arrange
    machine->WriteRegister(2, SC_Halt);

    //Act
    ExceptionHandler(SyscallException);

    //Assert
    EXPECT_EQ(true, interrupt->halted);
DECLARE_TEST_END(ExceptionHalt)

DECLARE_TEST_BEGIN(ExceptionGetChar)
    //Arrange
    machine->WriteRegister(2, SC_SynchGetChar);
    syncConsole->charToGet = 'c';

    //Act
    ExceptionHandler(SyscallException);

    //Assert
    char retValue = (char)machine->ReadRegister(2);
    EXPECT_EQ('c', retValue);
    VerifyPCCounter;
DECLARE_TEST_END(ExceptionGetChar)

DECLARE_TEST_BEGIN(ExceptionPutString)
    //Arrange
    machine->WriteRegister(2, SC_SynchPutString);
    const char *local_str = "Test String";
    const char *ptr = local_str;
    static const int test_offset = 4;
    while (*ptr) {
        machine->WriteMem((int)(ptr + test_offset), 1, (int)*ptr);
        ++ptr;
    }
    machine->WriteRegister(4, (int)(local_str + test_offset));

    //Act
    ExceptionHandler(SyscallException);

    //Assert
    EXPECT_EQ(std::string("Test String"), syncConsole->putString);
    VerifyPCCounter;
DECLARE_TEST_END(ExceptionPutString)

DECLARE_TEST_BEGIN(ExceptionGetString)
    //Arrange
    machine->WriteRegister(2, SC_SynchGetString);
    char local_buf[20];
    syncConsole->stringToGet = std::string("Hello world");
    int numBytes = 15;
    int initAddress = 1115;
    machine->WriteRegister(4, initAddress);
    machine->WriteRegister(5, numBytes);

    //Act
    ExceptionHandler(SyscallException);

    //Assert
    int ii = 0;
    for (ii = 0; ii < numBytes; ++ii) {
        *(local_buf + ii) = machine->memory[initAddress + ii];
    }
    EXPECT_STREQ("Hello world", local_buf);
    VerifyPCCounter;
DECLARE_TEST_END(ExceptionGetString)

DECLARE_TEST_BEGIN(ExceptionPutInt)
    //Arrange
    machine->WriteRegister(2, SC_SynchPutInt);
    machine->WriteRegister(4, 996);

    //Act
    ExceptionHandler(SyscallException);

    //Assert
    EXPECT_EQ(std::string("996"), syncConsole->putString);
    VerifyPCCounter;
DECLARE_TEST_END(ExceptionPutInt)

DECLARE_TEST_BEGIN(ExceptionGetInt)
    //Arrange
    machine->WriteRegister(2, SC_SynchGetInt);
    int virtualAddress = 4456;
    machine->WriteRegister(4, virtualAddress);
    syncConsole->stringToGet = std::string("5997");

    //Act
    ExceptionHandler(SyscallException);

    //Assert
    EXPECT_EQ(5997, machine->memory[virtualAddress]);
    VerifyPCCounter;
DECLARE_TEST_END(ExceptionGetInt)

TEST_LIST_END

int main() {
    std::cout << "STARTING EXCEP VM:\n\n"<< std::endl;
    INITIALIZE_TESTS;
    RUN_ALL_TESTS;
    std::cout << "\n\n"<< std::endl;
}
