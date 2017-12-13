#include <iostream>

#define EXPECT_EQ(expected, actual) \
    do \
    { \
        if (expected == actual) { /* do nothing */ } \
        else { std::cout << "Test Failed: " \
                         << "Function " << __FUNCTION__ << " Line: " \
                         << __LINE__ \
                         << " Expected " << expected << " Got " << actual << std::endl; } \
    } while(0);
