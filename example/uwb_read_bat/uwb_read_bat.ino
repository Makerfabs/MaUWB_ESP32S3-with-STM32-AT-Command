/*
For ESP32S3 UWB Read voltage Demo


Use 2.0.0   Wire
Use 1.11.11   Adafruit_GFX_Library
Use 1.16.2   Adafruit_BusIO
Use 2.0.0   SPI
Use 2.5.12   Adafruit_SSD1306

*/

// User config          ------------------------------------------

// User config end       ------------------------------------------

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>

#define SERIAL_LOG Serial

#define RESET 16

//SSD1306
#define I2C_SDA 39
#define I2C_SCL 38
Adafruit_SSD1306 display(128, 64, &Wire, -1);

//Bat
#define BAT_PIN 4
float bat_vol = 0.0;

void setup()
{
    pinMode(RESET, OUTPUT);
    digitalWrite(RESET, HIGH);

    SERIAL_LOG.begin(115200);
    SERIAL_LOG.print(F("ESP32S3 UWB Read voltage Demo"));

    Wire.begin(I2C_SDA, I2C_SCL);
    delay(1000);
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    { // Address 0x3C for 128x32
        SERIAL_LOG.println(F("SSD1306 allocation failed"));
        for (;;)
            ; // Don't proceed, loop forever
    }
    display.clearDisplay();
}

void loop()
{
    bat_vol = 3.3 * analogRead(BAT_PIN) / 4096 * 2; //Read battery voltage
    display.clearDisplay();
    display.setTextSize(2);              // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setCursor(0, 0);             // Start at top-left corner
    display.println("Battery:");
    display.setCursor(0, 20);
    display.println(bat_vol);
    display.setCursor(50, 20);
    display.println("V");
    display.display();
    delay(2000);
}
