// #ifndef NATIVE
// #define NATIVE
// #endif

#ifdef NATIVE

#include <unity.h>
#include <my.h>
#include "MockRTCLib.h"
#include <ArduinoFake.h>
#include <arduino-irrigation-controller.cpp>
#include <irrigation.h>

using namespace fakeit;

void test_setup(void)
{
    When(Method(ArduinoFake(), pinMode)).Return();

    setup();

    Verify(Method(ArduinoFake(), pinMode).Using(LED_BUILTIN, OUTPUT)).Once();
}

void setUp(void) {
    ArduinoFakeReset();

}

void tearDown(void) {
// clean stuff up here
}

void test_example(void) {
    TEST_ASSERT_EQUAL(5, SENSOR_VOLTAGE_REF);
}

void test_relay_pins_are_set_to_high_at_boot(void) {
    When(Method(ArduinoFake(), pinMode)).Return();
    When(Method(ArduinoFake(), analogRead, 0)).Return(0.3);

    sleepyMethod();


    Verify(Method(ArduinoFake(), pinMode).Using(23, OUTPUT)).Once();
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
    RUN_TEST(test_example);
    RUN_TEST(test_relay_pins_are_set_to_high_at_boot);
    UNITY_END();      // stop unit testing
}

#else

#include <Arduino.h>
#include <unity.h>
void setup() {
    UNITY_BEGIN();    // IMPORTANT LINE!
}

void loop() {
  UNITY_END(); // stop unit testing
}
#endif
