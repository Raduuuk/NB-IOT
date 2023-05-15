#include <DFRobot_SIM7000.h>
#include <SoftwareSerial.h>

#define PIN_TX     7
#define PIN_RX     8
#define THINGSPEAK_API_KEY "TBU138X3L580GACK"

#define VOLTAGE_PIN A0
#define CURRENT_PIN A1

SoftwareSerial     mySerial(PIN_RX, PIN_TX);
DFRobot_SIM7000    sim7000(&mySerial);

struct Reading {
  float voltage;
  float current;
  
};

void setup() {
  Serial.begin(9600);
  mySerial.begin(19200);

  // SIM7000 setup
  initSIM7000();

  // Add a delay before starting to take readings
  delay(1000);
}

void loop() {
  Reading reading = takeReading();
  printReading(reading);
  sendDataToThingspeak(reading);
  delay(15000); // 15 seconds delay
}

void initSIM7000() {
  // SIM7000 initialization and setup here (as in the first code)
}

Reading takeReading() {
  float voltage = analogRead(VOLTAGE_PIN) * 5.0 / 1023.0;
  float current = analogRead(CURRENT_PIN) * 5.0 / 1023.0;

  return {voltage, current};
}

void printReading(Reading reading) {
  Serial.print(" Voltage: ");
  Serial.print(reading.voltage);
  Serial.print("V, ");
  Serial.print("Current: ");
  Serial.print(reading.current);
  Serial.println("A");
}

void sendDataToThingspeak(Reading reading) {
  if (sim7000.openNetwork(DFRobot_SIM7000::eTCP, "api.thingspeak.com", 80) != 0) {
    Serial.println("Error attaching to network");
    return;
  }

  String url = "GET /update?api_key=" + String(THINGSPEAK_API_KEY) + "&field3=" + String(reading.voltage) + "&field4=" + String(reading.current) + " HTTP/1.1\r\nHost: api.thingspeak.com\r\nConnection: close\r\n\r\n";

  Serial.print("Sending request to ThingSpeak: ");
  Serial.println(url);

  sim7000.send(url.c_str());

  // Wait for the response
  uint32_t timeout = millis();
  while (!sim7000.recv("CLOSED", 1000)) {
    if (millis() - timeout > 30000) {
      Serial.println(" No response received within timeout");
      break;
    }
  }

  sim7000.closeNetwork();
}
