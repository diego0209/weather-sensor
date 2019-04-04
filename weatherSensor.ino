// Adafruit Feather M0 Adalogger

#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Time.h>
#include <RTClib.h>
#include <Adafruit_SI1145.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define CHIP_SELECT 4
#define BUTTON_A 9
#define BUTTON_B 6
#define BUTTON_C 5

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire); // SSD1306 display
Adafruit_SI1145 si1145 = Adafruit_SI1145();                  // SI1145 sensor
Adafruit_BME280 bme280;                                      // BME280 sensor
RTC_DS3231 rtc;                                              // DS3231 RTC module
bool displayBME = true;                                      // Determines whether to display the info of BME280 on screen or not
bool displaySI = false;                                      // Determines whether to display the info of SI1145 on screen or not
unsigned long displayTimer = 0;                              // Time counter (milliseconds)
const char FILE_NAME[] = "drake2.csv";                       // File name in SD card
const int DISPLAY_DELAY = 1000;                              // Delay for screen refresh

/**
 * setup function
 */
void setup() {
  Serial.begin(9600);
  digitalWrite(LED_BUILTIN, HIGH);

  displayBME = true;
  displaySI = false;

  screenSetup();
  checkSensors();
  // writeHeadersOnFile();

  displayTimer = 0;
  digitalWrite(LED_BUILTIN, LOW);
}

/**
 * loop function
 */
void loop() {
  if (!digitalRead(BUTTON_A)) {
    displayBME = true;
    displaySI = false;
  } else if (!digitalRead(BUTTON_B)) {
    displayBME = false;
    displaySI = true;
  } else if (!digitalRead(BUTTON_C)) {
    displayBME = false;
    displaySI = false;
    clearScreen();
  }

  if (millis() - displayTimer >= DISPLAY_DELAY) {
    displayTimer = millis();
    File dataFile; // Pointer to file in SD card
    dataFile = SD.open(FILE_NAME, FILE_WRITE);
    if (! dataFile) {
      display.clearDisplay();
      display.println("Could not open file");
      display.display();
      display.setCursor(0, 0);
      delay(500);
    }
    writeDateTime(dataFile, (displayBME || displaySI));
    writeBME280Data(dataFile, displayBME);
    writeSI1145Data(dataFile, displaySI);
    dataFile.close();
    yield();
  }
}

/**
 * Setups display
 */
void screenSetup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
  display.display();
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
}

/**
 * Checks that every sensor, the RTC and the SD card is working
 */
void checkSensors() {
  byte errors = 0;
  display.clearDisplay();
  if (!bme280.begin()) {
    display.println("No valid BME280 found");
    errors += 1;
  }
  if (!si1145.begin()) {
    display.println("No valid SI1145 found");
    errors += 1;
  }
  if (!rtc.begin()) {
    display.println("No RTC module found");
    errors += 1;
  }
  if (!SD.begin(CHIP_SELECT)) {
    display.println("SD card failed");
    errors += 1;
  } else {
    File dataFile;
    dataFile = SD.open(FILE_NAME, FILE_WRITE);
    if (!dataFile) {
      dataFile.println("Could not open file");
      errors += 1;
    }
  }
  if (errors > 0) {
    display.display();
    while (1);
  }
  display.setCursor(0, 0);
}

/**
 * Writes column headers on data file
 */
void writeHeadersOnFile() {
  File dataFile;
  dataFile = SD.open(FILE_NAME, FILE_WRITE);
  if(dataFile) {
    dataFile.println("\"DateTime\",\"Temperature\",\"Pressure\",\"Humidity\",\"Visible\",\"IR\",\"UV\"");
    dataFile.close();
  } else {
    display.clearDisplay();
    display.println("Could not open file");
    display.display();
    while(1);
  }
}

/**
 * Writes date and time on dataFile on SD card
 * @param dataFile       file on which to write
 * @param displayEnabled to display info on screen
 */
void writeDateTime(File dataFile, bool displayEnabled) {
  DateTime now = rtc.now();
  char buffer[13];
  sprintf(buffer, "\"%04lu-%02lu-%02luT", now.year(), now.month(), now.day());
  dataFile.print(buffer);
  sprintf(buffer, "%02lu:%02lu:%02lu\",", now.hour(), now.minute(), now.second());
  dataFile.print(buffer);

  if (displayEnabled) {
    displayDateTime(now);
  }
}

/**
 * Displays date and time on screen
 * @param dateTime DateTime
 */
void displayDateTime(DateTime now) {
  char buffer[13];
  display.clearDisplay();
  sprintf(buffer, "%02lu/%02lu/%04lu ", now.day(), now.month(), now.year());
  display.print(buffer);
  sprintf(buffer, "%02lu:%02lu:%02lu", now.hour(), now.minute(), now.second());
  display.println(buffer);
}

/**
 * Writes value of temperature, pressure and humidity on SD card
 * @param dataFile       file on which to write
 * @param displayEnabled to display info on screen
 */
void writeBME280Data(File dataFile, bool displayEnabled) {
  String dataString = "";
  float temp;
  temp = bme280.readTemperature();
  dataString += String(temp);
  dataString += ",";

  float pressure;
  pressure = bme280.readPressure() / 100.0F;
  dataString += String(pressure);
  dataString += ",";

  float humidity;
  humidity = bme280.readHumidity();
  dataString += String(humidity);
  dataString += ",";
  dataFile.print(dataString);

  if (displayEnabled) {
    displayBME280Data(temp, pressure, humidity);
  }
}

/**
 * Displays value of temperature, pressure and humidity on screen
 * @param temp     temperature
 * @param pressure atmospheric pressure
 * @param humidity relative humidity
 */
void displayBME280Data(float temp, float pressure, float humidity) {
  char buffer[18] = "";
  sprintf(buffer, "Temp: %f %cC", temp, (char)247);
  display.println(buffer);
  sprintf(buffer, "Press: %f hPa", pressure);
  display.println(buffer);
  sprintf(buffer, "Hum: %f %", humidity);
  display.println(buffer);
  display.display();
  display.setCursor(0, 0);
}

/**
 * Writes value of visible light, infrared and uv light on SD card
 * @param dataFile       file on which to write
 * @param displayEnabled to display info on screen
 */
void writeSI1145Data(File dataFile, bool displayEnabled) {
  String dataString = "";
  float visible;
  visible = si1145.readVisible();
  dataString += String(visible);
  dataString += ",";

  float irLight;
  irLight = si1145.readIR();
  dataString += String(irLight);
  dataString += ",";

  float uvIndex;
  uvIndex = si1145.readUV();
  uvIndex /= 100.0; // the index is multiplied by 100
  dataString += String(uvIndex);
  dataFile.println(dataString);

  if (displayEnabled) {
    displaySI1145Data(visible, irLight, uvIndex);
  }
}

/**
 * Displays value of visible light, infrared light and uv light on screen
 * @param visible visible light
 * @param irLight infrared light
 * @param uvIndex UV light index
 */
void displaySI1145Data(float visible, float irLight, float uvIndex) {
  char buffer[18] = "";
  sprintf(buffer, "Vis. light: %f", visible);
  display.println(buffer);
  sprintf(buffer, "IR light: %f", irLight);
  display.println(buffer);
  sprintf(buffer, "UV index: %f", uvIndex);
  display.println(buffer);
  display.display();
  display.setCursor(0, 0);
}

/**
 * Clears screen
 */
void clearScreen() {
  display.clearDisplay();
  display.display();
  display.setCursor(0, 0);
}

