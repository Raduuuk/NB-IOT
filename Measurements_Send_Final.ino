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

int count=250; 

struct Reading { // define datatime,milliseconds,V,C
  DateTime dateTime;
  unsigned long milliseconds;
  float voltage;
  float current;
};

Reading readings[250]; // Array to store 250 readings
volatile int readingCount = 0; // Counter for the number of readings taken

void initSIM7000();
Reading takeReading();
void printReading(Reading reading);
void sendDataToThingspeak();

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
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // Add a delay before starting to take readings
  delay(1000);
}

void loop() {
  Reading newReading = takeReading(); 
  readings[readingCount] = newReading;
  readingCount++;

  // The loop only processes and sends data when readingCount reaches 'readingCount'
  if (readingCount == count) {
    // Process and print the readings
    for (int i = 0; i < readingCount; i++) {
      printReading(readings[i]);
    }
    sendDataToThingspeak();
    readingCount = 0; // Reset readingCount after processing
  }
}

void initSIM7000() {
  // SIM7000 initialization and setup here
  // Initialize the SIM7000 module, set APN, etc.
}

Reading takeReading() {
  DateTime now = rtc.now();//set up a time for RTC module
  unsigned long milliseconds = millis(); 
  float voltage = analogRead(VOLTAGE_PIN) * (5.0 / 1023.0);
  float current = analogRead(CURRENT_PIN) * (5.0 / 1023.0);
   
  return {now,milliseconds,voltage, current};
}

void printReading(Reading reading) {
  Serial.print("Voltage: ");
  Serial.print(reading.voltage, 2); // Print voltage with 2 decimal places
  Serial.print("V, Current: ");
  Serial.print(reading.current, 2); // Print current with 2 decimal places
  Serial.print("A, Date and Time: ");
  Serial.println(reading.dateTime.toString("YYYY-MM-DDThh:mm:ss"));
   Serial.print(".");
  if (reading.milliseconds < 10) {
    Serial.print("00");
  } else if (reading.milliseconds < 100) {
    Serial.print("0");
  }
  Serial.print(reading.milliseconds);
  
  Serial.println("Z");
 
}

void sendDataToThingspeak() {
  for (int currentIndex = 0; currentIndex <= count; currentIndex++) {
    if (sim7000.openNetwork(DFRobot_SIM7000::eTCP, "api.thingspeak.com", 80) != 0) { // Check port of ThingSpeak
      Serial.println("Failed to open network connection. Retrying...");
      delay(5000); // Wait for a few seconds before retrying
      continue; // Retry opening the network connection
    }

    // Get the date and time as a formatted string
   String url = "GET /update?api_key=" + String(THINGSPEAK_API_KEY) +    //url 
              "&field3=" + String(readings[currentIndex].voltage, 2) +
              "&field4=" + String(readings[currentIndex].current, 2) +
              "&field5=" + readings[currentIndex].dateTime.toString("YYYY-MM-DDThh:mm:ss") +
              "." + readings[currentIndex].milliseconds +
              "Z HTTP/1.1\r\nHost: api.thingspeak.com\r\nConnection: close\r\n\r\n";

    Serial.print("Sending data to ThingSpeak: ");
    Serial.println(url);

    if (sim7000.send((char*)url.c_str())) {
      // Data sent successfully
      sim7000.closeNetwork();
    } else {
      Serial.println("Failed to send data. Retrying...");
      delay(2000); // Wait for a few seconds before retrying
      sim7000.closeNetwork(); // Close the network connection
      currentIndex--; // Retry the current index
    }

    delay(10000); 
  }
}
