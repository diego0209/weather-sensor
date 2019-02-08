// Adafruit Feather M0 Adalogger

#include <Wire.h>
#include <SPI.h>
#include <Time.h>
#include <SD.h>
#include "RTClib.h"
// SI1145
#include "Adafruit_SI1145.h"
// BME280
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
// SSD1306
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define chipSelect 4

RTC_DS3231 rtc; // RTC module

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

#define BUTTON_A 9
#define BUTTON_B 6
#define BUTTON_C 5
#define BUILTIN_LED 13

Adafruit_SI1145 uv = Adafruit_SI1145(); // SI1145 sensor

Adafruit_BME280 bme; // BME280 sensor

bool displayBME; // Determines whether to display the info of BME280 on screen
bool displaySI; // Determines whether to display the info of SI1145 on screen

/*
 * setup function
 */
void setup() {
  Serial.begin(9600);
  digitalWrite(BUILTIN_LED, HIGH);

  displayBME = true;
  displaySI = false;

  // Screen setup
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
  // display.display();

  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);

  // Checks that every sensor, the RTC and the SD card is working
  if (! rtc.begin()) {
    display.clearDisplay();
    display.println("Could not find RTC");
    display.display();
    while (1);
  } else if (! bme.begin()) {
    display.clearDisplay();
    display.println("Could not find a valid BME280 sensor");
    display.display();
    while(1);
  } else if (! uv.begin()) {
    display.clearDisplay();
    display.println("Could not find a valid SI1145 sensor");
    display.display();
    while(1);
  }

  // SD setup
  // Checks if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    display.clearDisplay();
    display.println("Card failed, or not present");
    display.display();
    while (1);
  }
  // File dataFile;
  // dataFile = SD.open("data.csv", FILE_WRITE);
  // if(dataFile) {
  //   dataFile.println("DateTime,Temp,Pressure,Humidity,Visible,IR,UV");
  //  dataFile.close();
  //  Serial.println("Wrote on SD card");
  //} else {
  //  Serial.println("Could not open file");
  //}
  
  digitalWrite(BUILTIN_LED, LOW);
}

/*
 * loop function
 */
void loop() {
  if(!digitalRead(BUTTON_A)) {
    displayBME = true; // Displays BME280
    displaySI = false;
  } else if(!digitalRead(BUTTON_B)) {
    displayBME = false;
    displaySI = true; // Displays SI1145
  } else if(!digitalRead(BUTTON_C)) {
    displayBME = false;
    displaySI = false;
  }
  
  display.clearDisplay();
  File dataFile; // Pointer to file in SD card
  dataFile = SD.open("data.csv", FILE_WRITE);
  if(!dataFile) {
    display.clearDisplay();
    display.println("Could not open file");
    display.display();
  }
  saveDateTime(dataFile, (displayBME || displaySI));
  readPressure(dataFile, displayBME);
  readUV(dataFile, displaySI);
  dataFile.close();
  delay(100);
  yield();
  display.display();
  display.setCursor(0,0);
}

/*
 * Saves date and time on dataFile
 * @param dataFile  file on which to write
 */
void saveDateTime(File dataFile, bool enableDisplay) {
  DateTime now = rtc.now();
  char buffer[12];
  sprintf(buffer, "%04lu-%02lu-%02luT", now.year(), now.month(), now.day());
  dataFile.print(buffer);

  sprintf(buffer, "%02lu:%02lu:%02lu,", now.hour(), now.minute(), now.second());
  dataFile.print(buffer);

  if(enableDisplay) {
    sprintf(buffer, "%02lu/%02lu/%04lu ", now.day(), now.month(), now.year());
    display.print(buffer);

    sprintf(buffer, "%02lu:%02lu:%02lu", now.hour(), now.minute(), now.second());
    display.println(buffer);
  }
}

/*
 * Gets a value of temperature, pressure and humidity
 * @param dataFile      file on which to write
 * @param enableDisplay to display info on screen
 */
void readPressure(File dataFile, bool enableDisplay) {
  String dataString = "";
  Serial.print("Temperature = ");
  float temp;
  temp = bme.readTemperature();
  Serial.print(temp);
  Serial.print(" °C ");
  dataString += String(temp);
  dataString += ",";

  float pressure;
  pressure = bme.readPressure() / 100.0F;
  Serial.print("Pressure = ");
  Serial.print(pressure);
  Serial.print(" hPa ");
  dataString += String(pressure);
  dataString += ",";

  float humidity;
  humidity = bme.readHumidity();
  Serial.print("Humidity = ");
  Serial.print(humidity);
  Serial.println(" %");
  dataString += String(humidity);
  dataString += ",";
  dataFile.print(dataString);

  if(enableDisplay) {
    display.print("Temp.: ");
    display.print(temp);
    display.print(" C\n");
    display.print("Press.: ");
    display.print(pressure);
    display.print(" hPa\n");
    display.print("Hum.: ");
    display.print(humidity);
    display.print(" %");
  }
}

/*
 * Gets a value of visible light, infrared and uv light
 * @param dataFile      file on which to write
 * @param enableDisplay to display info on screen
 */
void readUV(File dataFile, bool enableDisplay) {
  String dataString = "";
  Serial.print("Vis: ");
  float visible;
  visible = uv.readVisible();
  Serial.print(visible);
  dataString += String(visible);
  dataString += ",";

  Serial.print(" IR: ");
  float ir;
  ir = uv.readIR();
  Serial.print(ir);
  dataString += String(ir);
  dataString += ",";

  float UVindex;
  UVindex = uv.readUV();
  // the index is multiplied by 100
  UVindex /= 100.0;  
  Serial.print(" UV: ");
  Serial.println(UVindex);
  dataString += String(UVindex);
  dataFile.println(dataString);

  if(enableDisplay) {
    display.print("Vis.: ");
    display.print(visible);
    display.print("\n");
    display.print("IR: ");
    display.print(ir);
    display.print("\n");
    display.print("UV: ");
    display.print(UVindex);
  }
}
