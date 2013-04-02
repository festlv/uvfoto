#include "gcode/gcode.h"
#include <CppUTest/TestHarness.h>

TEST_GROUP(FirstTestGroup) {
    void setup() {

    }
    void teardown() {

    }
};


TEST(FirstTestGroup, FirstTest)
{
       FAIL("Fail me!");
}


