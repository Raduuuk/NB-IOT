#include <DFRobot_SIM7000.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <RTClib.h>

#define PIN_TX 7
#define PIN_RX 10
#define THINGSPEAK_API_KEY "TBU138X3L580GACK"

#define VOLTAGE_PIN A0
#define CURRENT_PIN A1

SoftwareSerial mySerial(PIN_RX, PIN_TX);
DFRobot_SIM7000 sim7000(&mySerial);

RTC_DS3231 rtc; //rtc module

int count = 250;

struct Reading { // define datatime,milliseconds,V,C
  DateTime dateTime;
  unsigned long milliseconds;
  float voltage;
  float current;
};

Reading readings[250]; // Array to store 40 readings
volatile int readingCount = 0; // Counter for the number of readings taken

void initSIM7000();
Reading takeReading();
void printReading(Reading reading);

void setup() {
  Serial.begin(9600);
  mySerial.begin(19200);

  // SIM7000 setup
  initSIM7000();

  if (!rtc.begin()) { //define rtc midule
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) { // check Power
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(DATE), F(TIME)));
  }

}

void loop() {
  unsigned long previousMicros = 0; // will store the last time a reading was taken

  for (int i = 0; i < count; i++) {
    unsigned long currentMicros = micros();

    if (currentMicros - previousMicros >= 4000) { // at least 4ms passed since last reading
      Reading newReading = takeReading();
      readings[readingCount] = newReading;
      readingCount++;

      previousMicros = currentMicros; // save the time a reading was taken
    }

    while (micros() - previousMicros < 4000); // wait for the remainder of the 4ms period
  }

  // Process and print the readings
  for (int i = 0; i < count; i++) {
    printReading(readings[i]);
  }

  readingCount = 0; // Reset readingCount

  delay(5000); // delay between sets of readings
}


void initSIM7000() {
  // SIM7000 initialization and setup here
  // Initialize the SIM7000 module, set APN, etc.
}

Reading takeReading() {
  DateTime now = rtc.now(); //set up a time for RTC module
  
  unsigned long milliseconds = millis();
  float voltage = analogRead(VOLTAGE_PIN) * (5.0 / 1023.0);
  float current = analogRead(CURRENT_PIN) * (5.0 / 1023.0);

  return {now, milliseconds, voltage, current};
}

void printReading(Reading reading) {
  Serial.print("Voltage: ");
  Serial.print(reading.voltage, 2);
  Serial.print("V, Current: ");
  Serial.print(reading.current, 2);
  Serial.print("A, Timestamp: ");
  Serial.print(reading.dateTime.year(), DEC);
  Serial.print('/');
  Serial.print(reading.dateTime.month(), DEC);
  Serial.print('/');
  Serial.print(reading.dateTime.day(), DEC);
  Serial.print(" ");
  Serial.print(reading.dateTime.hour(), DEC);
  Serial.print(':');
  Serial.print(reading.dateTime.minute(), DEC);
  Serial.print(':');
  Serial.print(reading.dateTime.second(), DEC);
  Serial.print('.');
  Serial.println(reading.milliseconds, DEC);
  
}
