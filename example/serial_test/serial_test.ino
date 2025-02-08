/*
For ESP32 UWB AT Demo
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>

HardwareSerial mySerial2(2);

#define RESET 32

#define IO_RXD2 18
#define IO_TXD2 19

#define I2C_SDA 4
#define I2C_SCL 5

Adafruit_SSD1306 display(128, 64, &Wire, -1);

void setup()
{
    pinMode(RESET, OUTPUT);
    digitalWrite(RESET, HIGH);

    Serial.begin(115200);

    Serial.print(F("Hello! ESP32-S3 AT command V1.0 Test"));
    mySerial2.begin(115200, SERIAL_8N1, IO_RXD2, IO_TXD2);

    mySerial2.println("AT");
    Wire.begin(I2C_SDA, I2C_SCL);
    delay(1000);
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    { // Address 0x3C for 128x32
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ; // Don't proceed, loop forever
    }
    display.clearDisplay();

    logoshow();
}

long int runtime = 0;

void loop()
{
    // put your main code here, to run repeatedly:
    while (Serial.available() > 0)
    {
        mySerial2.write(Serial.read());
        yield();
    }
    while (mySerial2.available() > 0)
    {
        Serial.write(mySerial2.read());
        yield();
    }
}

// SSD1306

void logoshow(void)
{
    display.clearDisplay();

    display.setTextSize(2);              // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setCursor(0, 0);             // Start at top-left corner
    display.println(F("Makerfabs"));

    display.setTextSize(1);
    display.setCursor(0, 20); // Start at top-left corner
    display.println(F("DW3000 AT DEMO"));
    display.display();
    delay(2000);
}
