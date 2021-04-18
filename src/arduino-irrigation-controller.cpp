/*

Created by Marc van Iersel & Rhuanito Soranz Ferrarezi, University of Georgia
Contact mvanier@uga.edu / rhuanito@terra.com.br for further assistance

Release date: October 1st, 2014
Updates: June 2nd, 2015 (by Rhuanito Soranz Ferrarezi)

PROGRAM OBJECTIVE:
Read 14 10HS sensors (Decagon Devices, Pullman, WA), convert the voltage measurements to volumetric water content (VWC), and control 14 irrigation plots based on the comparison of readings with Thresholds by opening and closing fourteen regular solenoid valves. The system can log the data on a SD card with time stamp and print the result to the Serial Monitor. Setup also includes one AM2302 temperature and relativity humidity sensor.

THE HARDWARE:
We used one Arduino open source prototype board (Mega 2560; Arduino, Ivrea, Italy), one stackable secure digital (SD) shield with built-in real-time clock (RTC) (SD 2.0; Adafruit, China) and one SD card (1 Gb; SanDisk Corporation, Milpitas, CA) for the logging system. The SD card needs to be formatted to FAT to work properly in the SD shield. The stacked boards were connected to two LEDs (green and red), a 5 VDC relay driver board with 16 relays (SainSmart, Leawood, KA), 14 capacitance-type soil moisture sensors (10HS; Decagon Devices, Pullman, WA), and 14 24 VAC 1-in regular solenoid valves (Rain Bird, Tucson, AZ). We added one temperature and relativity humidity sensor (AM2302; Adafruit). The SD shield was used to collect data, because the Arduino itself is not able to log the readings. The SD shield is assembled, with all the components pre-soldered. The user will only need to solder on stacking headers (not included, ordered separately) to attach to the Arduino.

WIRING:
1) All indicated colors are suggestions.
2) 10HS sensors: The 10HS data output (red wires) connected to analog pins A0 - A3 (sensors 1 - 4) and A6 - A15 (sensors 5 - 14). The 10HS excitation/power (white wires) of two sensors were tied together and connected to digital pins D43 - D49 (connections made using 'euro-style' terminal strip). All 14 10HS ground wires (bare wires) to ground (GND).
3) The RTC on the AdaFruit datalogging shield uses analog pins A4 and A5, so those cannot be used for sensor measurements.
4) AM2302 temperature and relativity humidity sensor: Left pin connected to 5V, second pin from left connected to digital pin D2, and right pin connected to GND.
5) The SD/RTC shield does not get plugged in on top of the Arduino Mega. It is easier to sue by using jumper wires to make the needed connections. AdaFruit datalogging shield attached to Arduino Mega as follows:
Arduino    SD shield
5V  to  5V
GND to  GND
MOSI - pin 51 to  SD shield: 11 (green)
MISO - pin 50 to  SD shield: 12 (red)
CLK - pin 52  to  SD shield: 13 (orange)
CS - pin 53 to  SD shield: 10 (white)
6) LEDs (red and green LEDs used for error checking, green LED = OK, red LED = bad sensor reading): Negative (short) lead of both LEDs tied together, connected to 4.6 kOhm resistor and wired to GND. Long lead of green LED to digital pin D8, and long lead of red LED to digital pin D9.
7) Relay drivers: Relays 1 - 14 are controlled by digital pins D22 - D35 (note: two relays are not used). The relay board also needs connections to GND and 5V on Arduino Mega.
8) Relays should use reverse logic, what means LOW to open and HIGH to close in the program.
9) The serial monitor allows users to see information on the computer screen.  It can be started by going to Tools > Serial monitor or by clicking Ctrl + Shift + M).

UPDATES:
1) Return instruction added to the system verification and RunTime and IrrigTime declared as unsigned long variables.

*/

// ===========================================================================================

// Include the required libraries for interfacing with the hardware
#ifndef NATIVE
#include <SD.h>
#include <Wire.h>
#include "DHT.h"
#include "RTClib.h"
#else
// #include <ArduinoFake.h>
#endif


#include <irrigation.h>

#define N_SENSORS 1
#define SENSOR_VOLTAGE_REF 5

// Declare variables for 14 sensors (numbered from #1 - #14). The number of variables needs to be sensor n+1 due to the counting starts on 0 instead of 1
float VWC[15], Threshold[15], SubCalSlope, SubCalIntercept, h, t, e_sat, e, VPD;
int sensorValue[15], Counter[15], i;
unsigned long IrrigTime, RunTime;

// Define the model of RTC is used
RTC_DS1307 rtc;

// Temperature and relative humidity sensor connected to digital pin D2
#define DHTPIN 2

// Define the type of temperature and relative humidity sensor we are using
// #define DHTTYPE DHT22   // DHT 22 (AM2302)
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

Irrigation irrigation;


// ===========================================================================================

template <class T> void print(T msg) {
  #ifndef NATIVE
  Serial.print(msg);
  #endif
}

template <class T> void println(T msg) {
  #ifndef NATIVE
  Serial.println(msg);
  #endif
}

void print(float msg, int len) {
  #ifndef NATIVE
  Serial.print(msg, len);
  #endif
}

void println() {
  #ifndef NATIVE
  Serial.println();
  #endif
}


void setup() {

  //**************************************************************************************//
  //     NOTE: THE FOLLOWING SECTION CONTAINS ALL IMPORTANT USER-CHANGEABLE SET POINT     //
  //     DO NOT MODIFY OTEHR PARTS OF THE PROGRAM UNLESS YOU KNOW WHAT YOU'RE DOING       //
  //**************************************************************************************//

  // IRRIGATION TIME: Set according to your need (in seconds). The irrigation time is 60 s (=1 min). You can also set different irrigation times for each plot if needed, just changing the time in each sensor identification # accordingly
  IrrigTime = 30;

  // RUN TIME: Set according to your need (in seconds). This program run every 1800 s (=30 min)
  RunTime = 1800;

  // IRRIGATION THRESHOLDS: Values used to trigger irrigation when the sensor readings are below a specific VWC (in units of m3/m3 or L/L)
  Threshold[1]=40.0;
  Threshold[2]=0.4;
  Threshold[3]=0.4;
  Threshold[4]=0.4;
  Threshold[5]=0.4;
  Threshold[6]=0.4;
  Threshold[7]=0.4;
  Threshold[8]=0.4;
  Threshold[9]=0.4;
  Threshold[10]=0.4;
  Threshold[11]=0.4;
  Threshold[12]=0.4;
  Threshold[13]=0.4;
  Threshold[14]=0.4;

  // SUBSTRATE CALIBRATION: You have to convert the voltage to VWC using soil or substrate specific calibration. Decagon has generic calibrations (check the 10HS manual at http://manuals.decagon.com/Manuals/13508_10HS_Web.pdf) or you can determine your own calibration. We used our own calibration for Fafard 1P (peat: perlite, Conrad Fafard, Inc., Agawam, MA)
  SubCalSlope = 1.1785;
  SubCalIntercept = -0.4938;

  //***************************************************************************************//
  //                     END OF SECTION WITH USER-CHANGEABLE SETPOINT                                                                          //
  //     DO NOT MODIFY OTHER PARTS OF THE PROGRAM UNLESS YOU KNOW WHAT YOU'RE DOING                  //
  //**************************************************************************************//

  // Initialize serial communication (over the USB cable connecting the Arduino Uno to a computer) at 57,600 bits per second
  Serial.begin(57600);

  // Start the WIRE library
  Wire.begin();

  // Start the RTC library
  rtc.begin();

  // Initialize temperature and relative humidity measurements
  dht.begin();

  irrigation.begin();

  // Check if the RTC is running. If not, show error message on serial monitor
  if (! rtc.isrunning()) {
    println("RTC is NOT running!");
    // Following line sets the RTC to the date and time this sketch (program) was compiled
    rtc.adjust(DateTime(__DATE__, __TIME__));
  }
  else {
    // If RTC has been started, send message to serial port
    println("Real time clock initialized.");
  }

  // Pin to write to SD card, depends on SD board, check manufacturing specs (53 for Arduino Mega)
  const int chipSelect = 53;
  // Configure digital pin D10 (53?) as output. This pin is used by default for use with the SD shield (chipSelect)
  pinMode(chipSelect, OUTPUT);

  // See if the SD card is present and can be initialized. If not, send an error message to the serial port and prevent the program from running. Send a message to the screen that the SD card is being initialized
  print("Initializing SD card... ");

  if (!SD.begin()) {
    println("*******************************************");
    println("       Card failed, or not present         ");
    println("     WARNING: NO DATA WILL BE COLLECTED!   ");
    println("CHECK CARD AND ALL CONNECTIONS TO SD SHIELD");
    println("*******************************************");
  }
  else {
    // If SD card is available, send message to serial port
    println("SD card initialized.");
  }
  println();

  // Initialize file and write header
  File dataFile = SD.open("log.txt", FILE_WRITE);
  // If the file is available, write headers to it
  if (dataFile) {
    dataFile.println();
    dataFile.println("Date Time, temp, RH, e_sat, e, VPD, VWC[1], VWC[2], VWC[3], VWC[4], VWC[5], VWC[6], VWC[7], VWC[8], VWC[9], VWC[10], VWC[11], VWC[12], VWC[13], VWC[14], Counter[1], Counter[2], Counter[3], Counter[4], Counter[5], Counter[6], Counter[7], Counter[8], Counter[9], Counter[10], Counter[11], Counter[12], Counter[13], Counter[14]");
    dataFile.println();
    dataFile.close();
  }
  // If the file is not open, pop up an error
  else {
    println("error opening data file log.txt");
  }

  // Configure digital pins D43 - D49 as outputs to apply voltage to all fourteen sensors (D43: sensor 1 and 2; D44, sensor 3 and 4, D45: sensor 5 and 6; D46: sensor 7 and 8; D47: sensor 9 and 10; D48: sensor 11 and 12; D49: sensor 13 and 14)
  // pinMode(43, OUTPUT);
  // pinMode(44, OUTPUT);
  // pinMode(45, OUTPUT);
  // pinMode(46, OUTPUT);
  // pinMode(47, OUTPUT);
  // pinMode(48, OUTPUT);
  // pinMode(49, OUTPUT);

  // Set digital pins D22 - D35 HIGH. These digital pins control the relays. Setting these pins HIGH assures that the relays are open at the initial startup or when the Arduino is reseted
  for (i = 22; i < 36; i = i + 1) {
    digitalWrite(i, HIGH);
    // Then set pins as outputs
    pinMode(i, OUTPUT);
  }

  // Set pins that control LEDs as output
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);

  // Use the internal 2.56 volt on the Mega board as the reference for all analog voltage measurements
  // analogReference(INTERNAL2V56);
}





// ===========================================================================================

void loop() {

}



// The following section (loop) of the program will run until the power is disconnected, at an interval specified by the 'RunTime'
void loopOld() {

  // Check to make sure that the frequency at which the program runs (RunTime) is at least 15x longer than the irrigation duration (IrrigTime)
  if (RunTime < IrrigTime*15) {
    println("**********************************************");
    println("WARNING: RunTime is too short. Please increase");
    println("WARNING: THE PROGRAM WILL NOT RUN CORRECTLY!");
    println("**********************************************");
    return;
  }

  // Check the current date and time
  DateTime now = rtc.now();

  // Measure the AM2302 temperature and relative humidity sensor. Reading temperature or humidity takes about 250 milliseconds. Sensor readings may also be up to 2 seconds 'old' (it is a very slow sensor)
  h = dht.readHumidity();
  t = dht.readTemperature();

  // Check if returns are valid, if they are NaN (not a number) then something went wrong
  if (isnan(t) || isnan(h)) {
    println("Failed to read from DHT");
  }

  // Calculate saturation vapor pressure from the measured temperature
  e_sat = 0.6112*exp((17.67*t)/(t+243.5));
  // Calculate vapor pressure from saturated vapor pressure
  e = e_sat*h/100;
  // Calculate the vapor pressure deficit (VPD) from saturated and actual vapor pressure
  VPD = e_sat-e;

  // Turn on the green LED to show that the system is executing the program
  digitalWrite(8, HIGH);
  // And turn the red LED off so that it can be used later in the program to check all sensor readings
  digitalWrite(9, LOW);

  // Measure all the sensors. This gives a raw value between 0 and 1023. Because of limited power available from the digital pins on the Arduino Mega, 7 pins are used to power the 14 sensors (each pin powers two sensors)
  // Power sensor 1 and 2
  // digitalWrite(43, HIGH);
  // Wait 10 ms
  delay(10);
  // Measure sensor 1 and 2
  sensorValue[1] = analogRead(0);
  // sensorValue[2] = analogRead(1);
  // And turn the power to the sensors off
  // digitalWrite(43, LOW);

  //
  //
  // // Now repeat this process for the rest of the sensors
  // // Sensor 3 and 4
  // digitalWrite(44, HIGH);
  // delay(10);
  // sensorValue[3] = analogRead(2);
  // sensorValue[4] = analogRead(3);
  // digitalWrite(44, LOW);
  //
  // // Sensor 5 and 6
  // digitalWrite(45, HIGH);
  // delay(10);
  // sensorValue[5] = analogRead(6);
  // sensorValue[6] = analogRead(7);
  // digitalWrite(45, LOW);
  //
  // // Sensor 7 and 8
  // digitalWrite(46, HIGH);
  // delay(10);
  // sensorValue[7] = analogRead(8);
  // sensorValue[8] = analogRead(9);
  // digitalWrite(46, LOW);
  //
  // // Sensor 9 and 10
  // digitalWrite(47, HIGH);
  // delay(10);
  // sensorValue[9] = analogRead(10);
  // sensorValue[10] = analogRead(11);
  // digitalWrite(47, LOW);
  //
  // // Sensor 11 and 12
  // digitalWrite(48, HIGH);
  // delay(10);
  // sensorValue[11] = analogRead(12);
  // sensorValue[12] = analogRead(13);
  // digitalWrite(48, LOW);
  //
  // // Sensor 13 and 14
  // digitalWrite(49, HIGH);
  // delay(10);
  // sensorValue[13] = analogRead(14);
  // sensorValue[14] = analogRead(15);
  // digitalWrite(49, LOW);



  // For all 14 sensors (i goes from 1 - 14)
  for (i = 1; i <= N_SENSORS; i++) {
    // Convert the measured raw values to VWC using a calibration equation. Note the 2.56 is the reference voltage used for all analog measurements. Raw value of 0 = 0V, 1023 = 2.56V
    // VWC[i] = sensorValue[i] * (2.56/1023.0) * SubCalSlope + SubCalIntercept;
    VWC[i] = sensorValue[i]/ 10;
    // Write an error message to the screen, turn on the red LED, and turn off the green LED when a particular sensor is reading too low
    if (VWC[i] < 0) {
      print("WARNING: Sensor ");
      print(i);
      print(" out of range (too low). Current reading: ");
      print(VWC[i]);
      println(" m3/m3");
      digitalWrite(8,LOW);
      digitalWrite(9,HIGH);
    }
    // ... or too high
    if (VWC[i] > 0.8) {
      print("WARNING: Sensor ");
      print(i);
      print(" out of range (too high). Current reading: ");
      print(VWC[i]);
      println(" m3/m3");
      digitalWrite(8,LOW);
      digitalWrite(9,HIGH);
    }
  }

  // PRINT THE DATE AND TIME STAMP TO THE SERIAL MONITOR TO THE SERIAL PORT
  // Note the software has a quirk: Whenever the month/day/hour/minute/second is less then 10, by default it will use a single digit (e.g., '3' instead of '03'). That can result in odd formatting of the time or date. So whenever the value is less than 10, we first write a 0, followed by the single digit. This fixes the formatting. Note: Serial.print commands write information to the same line on the screen. Serial.println prints information and then goes to a new line. To send characters to the screen use '…', and to send text to the screen use "...". The value of variables can be written to screen by using the name of that variable (not in quotation marks)
  // Start with showing date and time
  // Write the month to the screen
  if (now.month() < 10) print('0');
  print(now.month(), DEC);
  print('/');
  // Write the day to the screen
  if (now.day() < 10) print('0');
  print(now.day(), DEC);
  print('/');
  // Write the year to the screen
  print(now.year(), DEC);
  print(' ');
  // Write the hour to the screen
  print(now.hour(), DEC);
  print(':');
  // Write the minute to the screen
  if (now.minute() < 10) print('0');
  print(now.minute(), DEC);
  print(':');
  // Write the second to the screen
  if (now.second() < 10) print('0');
  print(now.second(), DEC);
  print(", ");
  println();

  // On the next line, show relative humidity, temperature, saturation vapor pressure, vapor pressure and VPD
  print("Humidity: ");
  print(h);
  print("%, Temperature: ");
  print(t);
  print(" *C, e_sat: ");
  print(e_sat);
  print(" kPa, e: ");
  print(e);
  print(" kPa, VPD: ");
  print(VPD);
  println(" kPa");

  // On the next line, show the measured substrate VWC
  print("VWC (m3/m3): ");
  for (i = 1; i <= N_SENSORS; i++) {
    println();
    print("Plot #");
    print(i);
    print(" = ");
    print(VWC[i]);
    print(", ");
  }
  println();

  // On the next line, show the number of irrigations
  print("Number of irrigations: ");
  for (i = 1; i <= N_SENSORS; i++) {
    println();
    print("Plot #");
    print(i);
    print(" = ");
    print(Counter[i]);
    print(", ");
  }
  println();
  println();

  // For all 14 sensors (i goes from 1 - 14)
  for (i = 1; i <= N_SENSORS; i++) {
    // Determine whether the measured VWC is below the threshold (and irrigation is needed), and, if so, turn the digital pin LOW (which closes the NO and COM connection at the relay and opens the corresponding valve). Note: the i+21 refers to the fact that irrigation in plot 1 is controlled by digital pin 22, plot 2 by pin 23, etc. Those pins are defined at the start of the program
    if (VWC[i] < Threshold[i]) {
      digitalWrite(i+21, LOW);
      print("Plot ");
      print(i);
      print(" irrigation started. ");
      delay(IrrigTime*1000);
      println("Irrigation finished.");
      digitalWrite(i+21, HIGH);
      Counter[i] = Counter[i]+1;
    }
    // And if irrigation is not needed
    else {
      // The program simply pauses for the specified irrigation time. This is done to assure that the program takes the same amount of time to run regardless of which plots get irrigated
      delay(IrrigTime*1000);
      // And send a message to the screen that the plot does not need irrigation
      print("Plot #");
      print(i);
      println(" does not need irrigation.");
    }
  }

  println();
  println("************************************************************************");
  println();

  // THE FOLLOWING SECTION IS FOR SAVING AND COLLECTING DATA ON THE SD CARD
  // Open the data file on the SD card
  File dataFile = SD.open("log.txt", FILE_WRITE);
  // If the file is available, write to it
  if (dataFile) {
    // Start with writing current time to the output file
    dataFile.println();
    dataFile.print(now.year(), DEC);
    dataFile.print('/');
    dataFile.print(now.month(), DEC);
    dataFile.print('/');
    dataFile.print(now.day(), DEC);
    dataFile.print(' ');
    dataFile.print(now.hour(), DEC);
    dataFile.print(':');
    if (now.minute() <10) dataFile.print('0');
    dataFile.print(now.minute(), DEC);
    dataFile.print(':');
    if (now.second() <10) dataFile.print('0');
    dataFile.print(now.second(), DEC);
    // Now write a comma. This will result in a comma-delimited file, which is easily imported into spreadsheets
    dataFile.print(", ");
    // Write environmental conditions to the output file
    dataFile.print(t);
    dataFile.print(", ");
    dataFile.print(h);
    dataFile.print(", ");
    dataFile.print(e_sat);
    dataFile.print(", ");
    dataFile.print(e);
    dataFile.print(", ");
    dataFile.print(VPD);
    dataFile.print(", ");
    // Write substrate volumetric water contents to the output file (14 values)
    for (i = 1; i <= N_SENSORS; i++) {
      dataFile.print(VWC[i]);
      dataFile.print(", ");
    }
    // Write the number of irrigations to the output file (14 values)
    for (i = 1; i <= N_SENSORS; i++) {
      dataFile.print(Counter[i]);
      dataFile.print(", ");
    }
    dataFile.close();
  }

  // The program will run approximately as often as specified by the RunTime (at start of program). When this delay has passed, the program will run again. RunTime and IrrigTime are multiplied by 1,000 to convert it from seconds to milliseconds. 785 is an estimate of how long it takes the program to run (785 milliseconds to collect and process the data, not counting the irrigation delays). This may be a few milliseconds off, but should be very close
  // TODO: Don't forget to put this line back in when done
  // delay(RunTime*1000 - (IrrigTime*1000*14) - 785);
}
