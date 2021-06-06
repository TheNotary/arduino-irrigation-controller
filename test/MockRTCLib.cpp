#ifdef NATIVE

#include <ArduinoFake.h>
#include "MockRTCLib.h"

const uint8_t daysInMonth[] PROGMEM = {31, 28, 31, 30, 31, 30,
                                       31, 31, 30, 31, 30};


DateTime::DateTime(uint32_t t) {
  t -= SECONDS_FROM_1970_TO_2000; // bring to 2000 timestamp from 1970

  ss = t % 60;
  t /= 60;
  mm = t % 60;
  t /= 60;
  hh = t % 24;
  uint16_t days = t / 24;
  uint8_t leap;
  for (yOff = 0;; ++yOff) {
    leap = yOff % 4 == 0;
    if (days < 365U + leap)
      break;
    days -= 365 + leap;
  }
  for (m = 1; m < 12; ++m) {
    uint8_t daysPerMonth = pgm_read_byte(daysInMonth + m - 1);
    if (leap && m == 2)
      ++daysPerMonth;
    if (days < daysPerMonth)
      break;
    days -= daysPerMonth;
  }
  d = days + 1;
}

/**************************************************************************/
/*!
    @brief  Constructor from (year, month, day, hour, minute, second).
    @warning If the provided parameters are not valid (e.g. 31 February),
           the constructed DateTime will be invalid.
    @see   The `isValid()` method can be used to test whether the
           constructed DateTime is valid.
    @param year Either the full year (range: 2000--2099) or the offset from
        year 2000 (range: 0--99).
    @param month Month number (1--12).
    @param day Day of the month (1--31).
    @param hour,min,sec Hour (0--23), minute (0--59) and second (0--59).
*/
/**************************************************************************/
DateTime::DateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour,
                   uint8_t min, uint8_t sec) {
  if (year >= 2000U)
    year -= 2000U;
  yOff = year;
  m = month;
  d = day;
  hh = hour;
  mm = min;
  ss = sec;
}

/**************************************************************************/
/*!
    @brief  Copy constructor.
    @param copy DateTime to copy.
*/
/**************************************************************************/
DateTime::DateTime(const DateTime &copy)
    : yOff(copy.yOff), m(copy.m), d(copy.d), hh(copy.hh), mm(copy.mm),
      ss(copy.ss) {}



boolean RTC_DS1307::begin(void) {
  return true;
  // Wire.begin();
  // Wire.beginTransmission(DS1307_ADDRESS);
  // if (Wire.endTransmission() == 0)
  //   return true;
  // return false;
}

uint8_t RTC_DS1307::isrunning(void) {
  return 1;
  // Wire.beginTransmission(DS1307_ADDRESS);
  // Wire._I2C_WRITE((byte)0);
  // Wire.endTransmission();
  //
  // Wire.requestFrom(DS1307_ADDRESS, 1);
  // uint8_t ss = Wire._I2C_READ();
  // return !(ss >> 7);
}



DateTime RTC_DS1307::now() {
  // Wire.beginTransmission(DS1307_ADDRESS);
  // Wire._I2C_WRITE((byte)0);
  // Wire.endTransmission();
  //
  // Wire.requestFrom(DS1307_ADDRESS, 7);
  // uint8_t ss = bcd2bin(Wire._I2C_READ() & 0x7F);
  // uint8_t mm = bcd2bin(Wire._I2C_READ());
  // uint8_t hh = bcd2bin(Wire._I2C_READ());
  // Wire._I2C_READ();
  // uint8_t d = bcd2bin(Wire._I2C_READ());
  // uint8_t m = bcd2bin(Wire._I2C_READ());
  // uint16_t y = bcd2bin(Wire._I2C_READ()) + 2000U;

  return DateTime(0, 0, 0, 0, 0, 0);
}

#endif
