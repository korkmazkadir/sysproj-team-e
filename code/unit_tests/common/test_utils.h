#include <iostream>
#include <vector>
#include <random>
#include <algorithm>

#include <string.h>

/*!
 * The following is the brief description of our own 'test framework'
 * Every test case is a function whose body MUST be specified between
 * DECLARE_TEST_BEGIN(name)
 * and
 * DECLARE_TEST_END(name)
 * macros, where name is the name of the test case.
 *
 * All the test cases must be within
 * TEST_LIST_BEGIN
 * and
 * TEST_LIST_END
 * macros
 *
 * All the properly specified tests are run in a random order
 * EXPECT_* macros should be used for performing checks inside test cases
 *
 * These macros will properly update overall test result and print a nice
 * error message if any
 *
 * For convenience, one can re-define functions
 * void TestUtils_SetUp() and void TestUtils_TearDown()
 *
 * TestUtils_SetUp is run BEFORE executing every test case
 * TestUtils_TearDown is run AFTER executing every test case
 */

void TestUtils_SetUp() __attribute__((weak));
void TestUtils_TearDown() __attribute__((weak));

class TUTestInterface
{
private:
    virtual void userFunction() { }

    void printTestResults() {
        if (m_testResult) {
            std::cout << "Test " << m_testName << " PASSED" << std::endl;
        } else {
            std::cout << "Test " << m_testName << " FAILED" << std::endl;
        }
    }

protected:
    bool m_testResult;
    std::string m_testName;

public:
    static std::vector<TUTestInterface *> testRunners;

    static void runAllTests() {
        std::random_shuffle(testRunners.begin(), testRunners.end());
        for (TUTestInterface *runner : testRunners) {
            runner->run();
        }
    }

    TUTestInterface(const char *name):
        m_testResult(true)
      , m_testName(name)
        { }

    void run() {
        std::cout << "Executing test " << m_testName << " ..." << std::endl;
        if (TestUtils_SetUp) {
            TestUtils_SetUp();
        }
        userFunction();
        if (TestUtils_TearDown) {
            TestUtils_TearDown();
        }
        printTestResults();

    }
};

std::vector<TUTestInterface *> TUTestInterface::testRunners = std::vector<TUTestInterface *>();

#define TEST_LIST_BEGIN void test_utils_wrapper() {

#define DECLARE_TEST_BEGIN(name) \
        class name##_stub_class: public TUTestInterface { \
            public: \
            name##_stub_class(const char *c): TUTestInterface(c) { } \
            private: \
            void userFunction() {

#define DECLARE_TEST_END(name) }\
    }; \
    TUTestInterface::testRunners.push_back(new name##_stub_class(#name));

#define RUN_ALL_TESTS TUTestInterface::runAllTests()

#define TEST_LIST_END }

#define INITIALIZE_TESTS do { srand(time(NULL)); test_utils_wrapper(); } while (0)

#define GENERIC_FAILURE_OUTPUT(expected, actual) do { \
               m_testResult = false; \
               std::cout << "Test Failed: " \
                         << "Function " << __FUNCTION__ << " Line: " \
                         << __LINE__ \
                         << " Expected " << expected << " Got " << actual << std::endl; } while(0)

#define EXPECT_EQ(expected, actual) \
    do \
    { \
        if (expected == actual) { /* do nothing */ } \
        else { GENERIC_FAILURE_OUTPUT(expected, actual); } \
    } while(0)

#define EXPECT_STREQ(expected, actual) \
    do \
    { \
        if (strcmp(expected, actual) == 0) { /* do nothing */ } \
        else { GENERIC_FAILURE_OUTPUT(expected, actual); } \
    } while(0)

#define EXPECT_LESS(less, greater) \
    do \
    { \
        if (less < greater) { /* do nothing */ } \
        else { \
            m_testResult = false; \
            std::cout << "Test Failed: " \
              << "Function " << __FUNCTION__ << " Line: " \
              << __LINE__ \
              << " Expected " << less << " less than " << greater << std::endl; } \
    } while(0)
