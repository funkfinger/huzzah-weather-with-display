#include <Arduino.h>

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager



#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"


// Adafruit alphanumeric LED Feather Wing...
Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();

// Adafruit BME280...
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme; // I2C


double temp;
double humd;
double pres;

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    WiFiManager wifiManager;
    //reset saved settings
    //wifiManager.resetSettings();
    
    wifiManager.autoConnect("KoolbreezeSensor");

    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");
    
    setupAlphaNumeric();
    setupBme280();
    
}

void loop() {
  readValues();  
  // sendToDisp("temp", temp);
  sendToDisp("pres", pres, "h");
  delay(5000);
  sendToDisp("temp", temp, "f");
  delay(5000);
  sendToDisp("humd", humd, "%");
  delay(5000);
}

void sendToDisp(const char name[], double value, const char unit[]) {
  alpha4.writeDigitAscii(0, name[0]);
  alpha4.writeDigitAscii(1, name[1]);
  alpha4.writeDigitAscii(2, name[2]);
  alpha4.writeDigitAscii(3, name[3]);
  alpha4.writeDisplay();
  delay(1000);
  
  int numOfChars = 4;
  numOfChars = (double)((int)value) == value ? 4 : 3;
  // numOfChars = (value % 1 == 0) ? 5 : 4;
  
  char buffer[8] = "";
  dtostrf(value, numOfChars, 2, buffer);
  
  bool dec0 = false;
  bool dec1 = false;
  bool dec2 = false;
  bool dec3 = false;

  if(String(buffer[1]) == ".") {
    dec0 = true;
    buffer[1] = buffer[2];
    buffer[2] = buffer[3];
    buffer[3] = buffer[4];
  }

  if(String(buffer[2]) == ".") {
    dec1 = true;
    buffer[2] = buffer[3];
    buffer[3] = buffer[4];
  }

  if(String(buffer[3]) == ".") {
    dec2 = true;
    buffer[3] = buffer[4];
  }

  alpha4.writeDigitAscii(0, buffer[0], dec0);
  alpha4.writeDigitAscii(1, buffer[1], dec1);
  alpha4.writeDigitAscii(2, buffer[2], dec2);
  alpha4.writeDigitAscii(3, unit[0]);
  alpha4.writeDisplay();
  delay(5000);
}

void setupAlphaNumeric() {
  alpha4.begin(0x70);
  alpha4.setBrightness(2);
  uint16_t segs = 0b1111111111111111;
  // segment test...
  for (int i = 0; i<16; i++) {
    alpha4.writeDigitRaw(0, segs);
    alpha4.writeDigitRaw(1, segs);
    alpha4.writeDigitRaw(2, segs);
    alpha4.writeDigitRaw(3, segs);
    alpha4.writeDisplay();
    delay(50);
    segs = segs << 1;
  }
  alpha4.clear();
  alpha4.writeDisplay();
}

void setupBme280() {
  bool status;
  // default settings
  status = bme.begin();
  if (!status) {
      Serial.println("Could not find a valid BME280 sensor, check wiring!");
      while (1);
  }
}


void readValues() {
  temp = c2f(bme.readTemperature());
  humd = bme.readHumidity();
  pres = bme.readPressure() * 0.000295300;
  
  Serial.print("Temperature = ");
  Serial.print(temp);
  Serial.println(" *F");

  Serial.print("Pressure = ");

  Serial.print(pres);
  Serial.println(" hPa");

  Serial.print("Humidity = ");
  Serial.print(humd);
  Serial.println(" %");

  Serial.println();
  
  
}


double c2f (double c) {
	return 1.8 * c + 32;
}