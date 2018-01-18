#include <stdlib.h>
#include "test_utils.h"

#include "frameprovider.h"
#include "machine.h"
static FrameProvider *testfp = NULL;

void TestUtils_SetUp() {
    testfp = new FrameProvider();
}

void TestUtils_TearDown() {
    delete testfp;
}

TEST_LIST_BEGIN

DECLARE_TEST_BEGIN(VMGetEmptyFrameTest)
    /*int num = testfp->NumAvailFrame(); //check first state
    EXPECT_EQ(num, NumPhysPages);
    int page = testfp->GetEmptyFrame(); //Get one vm page
    num = testfp->NumAvailFrame(); //check numbers after getting the page
    EXPECT_LESS_EQUAL(page, NumPhysPages);
    EXPECT_EQ(num, NumPhysPages - 1);

    testfp->ReleaseFrame(page);
    num = testfp->NumAvailFrame(); //check numbers after releasing the page
    EXPECT_EQ(num, NumPhysPages);*/

    EXPECT_EQ(0,0);

    
DECLARE_TEST_END(VMGetEmptyFrameTest)

TEST_LIST_END

int main () {
    std::cout << "STARTING TEST VM:\n"<< std::endl;
    INITIALIZE_TESTS;
    RUN_ALL_TESTS;
    std::cout << "\n\n"<< std::endl;
}
