#include <Wire.h>
// SI1145
#include "Adafruit_SI1145.h"
// BME280
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
// SSD1306
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

#define BUTTON_A 9
#define BUTTON_B 6
#define BUTTON_C 5
#define BUILTIN_LED 13

Adafruit_SI1145 uv = Adafruit_SI1145(); // UV sensor

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C sensor

bool displayBME;
bool displaySI;

/*
 * setup function
 */
void setup() {
  Serial.begin(9600);

  displayBME = true;
  displaySI = false;

  // Screen setup
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
  display.display();

  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);

  // text display tests
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);

  // Checks that every sensor is working
  if (! bme.begin()) {
    Serial.println("Could not find a valid BME280 sensor");
  } else if (! uv.begin()) {
    Serial.println("Didn't find SI1145");
  }

  digitalWrite(BUILTIN_LED, HIGH);
}

/*
 * Gets a value of temperature, pressure and humidity
 */
void readPressure(bool enableDisplay) {
  Serial.print("Temperature = ");
  float temp = bme.readTemperature();
  Serial.print(temp);
  Serial.print(" °C ");

  float pressure = bme.readPressure() / 100.0F;
  Serial.print("Pressure = ");
  Serial.print(pressure);
  Serial.print(" hPa ");
  
//  Serial.print("Approx. Altitude = ");
//  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
//  Serial.print(" m ");

  float humidity = bme.readHumidity();
  Serial.print("Humidity = ");
  Serial.print(humidity);
  Serial.println(" %");

  if(enableDisplay) {
    display.print("Temp.: ");
    display.print(temp);
    display.print(" C");
    display.print("\n");
    display.print("Press.: ");
    display.print(pressure);
    display.print(" hPa");
    display.print("\n");
    display.print("Hum.: ");
    display.print(humidity);
    display.print(" %");
  }
}

/*
 * Gets a value of visible, infrared and uv light
 */
void readUV(bool enableDisplay) {
  Serial.print("Vis: ");
  float visible = uv.readVisible();
  Serial.print(visible);
  
  Serial.print(" IR: ");
  float ir = uv.readIR();
  Serial.print(ir);
  
  float UVindex = uv.readUV();
  // the index is multiplied by 100
  UVindex /= 100.0;  
  Serial.print(" UV: ");
  Serial.println(UVindex);
  
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

/*
 * loop function
 */
void loop() {
  if(!digitalRead(BUTTON_A)) {
    displayBME = true; // Displays BME280
    displaySI = false;
  }
  if(!digitalRead(BUTTON_B)) {
    displayBME = false; // Displays SI1145
    displaySI = true;
  }
  if(!digitalRead(BUTTON_C)) {
    displayBME = false;
    displaySI = false;
  }
  display.clearDisplay();
  readPressure(displayBME);
  readUV(displaySI);
  delay(100);
  yield();
  display.display();
  display.setCursor(0,0);
}
