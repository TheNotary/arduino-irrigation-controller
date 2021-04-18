#ifndef NATIVE
#define NATIVE
#endif
#include <unity.h>
#include <my.h>
#include "MockRTCLib.h"
#include <arduino-irrigation-controller.cpp>
#include <irrigation.h>

void setUp(void) {
// set stuff up here
}

void tearDown(void) {
// clean stuff up here
}

void test_led_builtin_pin_number(void) {
    TEST_ASSERT_EQUAL(5, SENSOR_VOLTAGE_REF);
}

void mega_test(void) {
  // what do I want to test?
  // When I call
  // irrigation.work()
  // A shit ton of global variables change
  //   - Sensor values are updated to expectations
  //   - Irrigation count on plot one goes up
  //   - Serial is printed to (requires refactor of println)
  //   - The relay pin for plot 1 is set to "low" and then back to "high"
  //   - The method delays exactly x milliseconds
  //   -
}

int main(int argc, char **argv) {
    UNITY_BEGIN();    // IMPORTANT LINE!
    RUN_TEST(test_led_builtin_pin_number);
    UNITY_END();      // stop unit testing
}
