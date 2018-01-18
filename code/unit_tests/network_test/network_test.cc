#include <stdlib.h>
#include "test_utils.h"
#include <limits.h>


void TestUtils_SetUp() {
}

void TestUtils_TearDown() {
}

TEST_LIST_BEGIN

/*DECLARE_TEST_BEGIN(OpenConnection)

DECLARE_TEST_END(OpenConnection)*/


TEST_LIST_END

int main () {
    std::cout << "STARTED TEST NETWORK\n"<< std::endl;
    INITIALIZE_TESTS;
    RUN_ALL_TESTS;
    std::cout << "\n\n"<< std::endl;
}
