/*
For ESP32 UWB AT Demo
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>

HardwareSerial mySerial2(2);

#define RESET 16

#define IO_RXD2 18
#define IO_TXD2 17

#define I2C_SDA 39
#define I2C_SCL 38

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

String response = "";
String rec_head = "AT+RANGE";

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
        char c = mySerial2.read();

        if (c == '\r')
            continue;
        else if (c == '\n' || c == '\r')
        {
            // if(0)
            if (response.indexOf(rec_head) != -1)
            {

                range_analy(response);

                // Serial.println("-----------Get range msg-----------");

                // String result = response.substring(response.indexOf(rec_head) + rec_head.length());

                // Serial.println(result);
                // Serial.println("-----------Over-----------");
            }
            else
            {
                Serial.println(response);
            }

            response = "";
        }
        else
            response += c;
    }
}

// SSD1306

void logoshow(void)
{
    display.clearDisplay();

    display.setTextSize(2);              // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setCursor(0, 0);             // Start at top-left corner
    display.println(F("Get Range"));

    // display.setTextSize(1);
    display.setCursor(0, 20);
    display.println(F("JSON"));
    display.setCursor(0, 40);
    display.println(F("A0"));
    display.display();
    delay(2000);
}

// AT+RANGE=tid:1,mask:04,seq:63,range:(0,0,30,0,0,0,0,0),rssi:(0.00,0.00,-77.93,0.00,0.00,0.00,0.00,0.00)

void range_analy(String data)
{
    String id_str = data.substring(data.indexOf("tid:") + 4, data.indexOf(",mask:"));
    String range_str = data.substring(data.indexOf("range:"), data.indexOf(",rssi:"));
    String rssi_str = data.substring(data.indexOf("rssi:"));

    // Serial.println(id_str);
    // Serial.println(range_str);
    // Serial.println(rssi_str);

    // range:(0,0,51,0,0,0,0,0)
    // rssi:(0.00,0.00,-78.54,0.00,0.00,0.00,0.00,0.00)

    int range_list[8];
    double rssi_list[8];
    int count = 0;

    count = sscanf(range_str.c_str(), "range:(%d,%d,%d,%d,%d,%d,%d,%d)",
                   &range_list[0], &range_list[1], &range_list[2], &range_list[3],
                   &range_list[4], &range_list[5], &range_list[6], &range_list[7]);

    if (count != 8)
    {
        Serial.println("RANGE ANALY ERROR");
        Serial.println(count);
        return;
    }

    count = sscanf(rssi_str.c_str(), "rssi:(%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf)",
                   &rssi_list[0], &rssi_list[1], &rssi_list[2], &rssi_list[3],
                   &rssi_list[4], &rssi_list[5], &rssi_list[6], &rssi_list[7]);

    if (count != 8)
    {
        Serial.println("RSSI ANALY ERROR");
        Serial.println(count);
        return;
    }

    // Serial Port
    // Serial.print("TAG ");
    // Serial.println(id_str);
    // for (int i = 0; i < 8; i++)
    // {
    //     Serial.print("A");
    //     Serial.print(i);
    //     Serial.print(" RANGE ");
    //     Serial.print(range_list[i]);
    //     Serial.print(" RSSI ");
    //     Serial.println(rssi_list[i]);
    // }

    // Serial Port Json
    String json_str = "";
    json_str = json_str + "{\"id\":" + id_str + ",";
    json_str = json_str + "\"range\":[";
    for (int i = 0; i < 8; i++)
    {
        if (i != 7)
            json_str = json_str + range_list[i] + ",";
        else
            json_str = json_str + range_list[i] + "]}";
    }

    Serial.println(json_str);
}
