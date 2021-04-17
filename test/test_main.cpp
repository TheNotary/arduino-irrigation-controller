#include <unity.h>
#include <my.h>
// #include <arduino-irrigation-controller.ino>
#include <irrigation.h>

Irrigation irrigation;


void setUp(void) {
// set stuff up here
}

void tearDown(void) {
// clean stuff up here
}

void test_led_builtin_pin_number(void) {
    TEST_ASSERT_EQUAL(5, SENSOR_VOLTAGE_REF);
}


int main(int argc, char **argv) {
    UNITY_BEGIN();    // IMPORTANT LINE!
    RUN_TEST(test_led_builtin_pin_number);
    UNITY_END(); // stop unit testing
}
