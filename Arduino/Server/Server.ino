#include <Arduino_MKRIoTCarrier.h>
#include "visuals.h"
#include "pitches.h"
#include <SPI.h>
#include <WiFiNINA.h>

char ssid[] = "";        // your network SSID (name)
int keyIndex = 0;                // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
WiFiServer server(80);

WiFiClient client;

int ledPin = 2;

MKRIoTCarrier carrier;

String state = "start";

String device_name;

int count = 3;
uint32_t colorRed = carrier.leds.Color(0, 200, 0);
uint32_t colorGreen = carrier.leds.Color(200, 0, 0);
uint32_t colorBlue = carrier.leds.Color(0, 0, 200);

// notes in the melody:
int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

//Declare pages
void configure();
void startPage();
void touchpage();
void sensorsPage();
void connectorsPage();
void actuatorsPage();
void printWifiStatus();

//Declare their flags
bool touchCheckCompleted = false;
bool shakeCheckCompleted = false;
bool colorCheckCompleted = false;
bool gestureCheckCompleted = false;
bool relay1CheckCompleted = false;
bool relay2CheckCompleted = false;

File myFile;

String sd_card_raw = "";

void setup()
{
  CARRIER_CASE = false;
  Serial.begin(9600);

  delay(2500);
  Serial.println("Turning on");

  carrier.display.setRotation(0);
  carrier.display.setTextWrap(true);
  carrier.display.setTextColor(0xFFFF);
  carrier.display.setTextSize(2);
  carrier.display.fillScreen(0x0000);

  if (!carrier.begin())
  {
    Serial.println("Carrier not connected, check connections");
    //        while (1);
  }

  if (!SD.begin(SD_CS)) {
    Serial.println("initialization failed!");
    carrier.display.setCursor(0, 30);
    carrier.display.print("initialization failed!");
    while (1);
  }

  Serial.println("initialization done.");

  // open the file for reading:
  if (SD.exists("SSID.txt") && SD.exists("NAME.txt")) {
    configure();
//    touchpage();
    actuatorsPage();
    
    myFile = SD.open("SSID.txt");
    Serial.println("SSID opened");
    String SD_data = "";
    while (myFile.available()) {
      SD_data += char(myFile.read());
    }
    int str_len = SD_data.length() + 1;
    char SD_char[str_len];
    SD_data.toCharArray(SD_char, str_len);
    strcat(ssid, SD_char);

    if (String(ssid).length() < 8) {
      carrier.display.fillScreen(0x0000);
      carrier.display.setCursor(0, 60);
      carrier.display.println("Make SSID length more than 8 chars");
      while (1);
    }

    myFile = SD.open("NAME.txt");
    Serial.println("NAME opened");
    while (myFile.available()) {
      device_name += char(myFile.read());
    }

    carrier.display.fillScreen(0x0000);
    carrier.display.setCursor(0, 0);
    carrier.display.print(ssid);
    carrier.display.setCursor(0, 30);
    carrier.display.print(device_name);
    delay(1000);

    initWifi();
    printWiFiStatus();
  } else {
    myFile = SD.open("SSID.txt", FILE_WRITE);
    myFile.print("ROOM####");
    myFile.close();

    myFile = SD.open("NAME.txt", FILE_WRITE);
    myFile.print("SAMANTHA");
    myFile.close();

    Serial.println("done.");
    carrier.display.setCursor(0, 30);
    carrier.display.println("Write the SSID in the SSID.txt and PASSWORD in the PSWD.txt");
    carrier.display.print("Write the name of the device in the NAME.txt");
    while (1);
  }
}

void loop()
{
  carrier.display.setTextColor(0xFFFF);
  if (status != WiFi.status()) {
    // it has changed update the variable
    status = WiFi.status();

    if (status == WL_AP_CONNECTED) {
      Serial.println("Device connected to AP");
      carrier.display.setTextColor(0x0000);
      carrier.display.setCursor(0, 120);
      carrier.display.print("            ");
      carrier.display.setCursor(0, 120);
      carrier.display.print("We connected");
    } else {
      Serial.println("Device disconnected from AP");
      carrier.display.setTextColor(0x0000);
      carrier.display.setCursor(0, 120);
      carrier.display.print("                ");
      carrier.display.setCursor(0, 120);
      carrier.display.print("We not connected");
    }
  }

  client = server.available();
  if (client) {
    carrier.display.setCursor(0, 150);
    carrier.display.print("Client");
    newClient();

  } else {
    carrier.display.setTextColor(0x0000);
    carrier.display.setCursor(0, 150);
    carrier.display.print("          ");
  }
}

void configure()
{
  carrier.display.fillScreen(0x0000);
  //Basic configuration for the text
  carrier.display.setRotation(0);
  carrier.display.setTextWrap(true);
  carrier.display.drawBitmap(50, 40, arduino_logo, 150, 106, 0x253); //0x00979D);
  carrier.display.setTextColor(0xFFFF);
  carrier.display.setTextSize(3);
  carrier.display.setCursor(35, 160);
  carrier.display.print("Loading");
  //Printing a three dots animation
  for (int i = 0; i < 3; i++)
  {
    carrier.display.print(".");
    delay(1000);
  }

  carrier.display.fillScreen(0x0000);
  carrier.display.setTextColor(0xFFFF);
  carrier.display.setTextSize(3);
  carrier.display.setCursor(65, 30);
  carrier.display.print("Arduino ");
  carrier.display.setTextSize(2);
  carrier.display.setCursor(30, 60);
  carrier.display.print("MKR IoT Carrier");

  carrier.display.setTextSize(2);
  carrier.display.setCursor(24, 120);
  carrier.display.println("Let's start with");
  carrier.display.setCursor(25, 140);
  carrier.display.println("the setup of the");
  carrier.display.setCursor(75, 160);
  carrier.display.println("hardware");
  delay(4500);
}

//Page to verify all the buttons
void touchpage()
{
  carrier.display.fillScreen(0x0000);
  carrier.display.setTextColor(0xFFFF);
  carrier.display.setTextSize(3);
  carrier.display.setCursor(44, 40);
  carrier.display.print("Press all");
  carrier.display.setCursor(24, 70);
  carrier.display.print("the buttons");
  carrier.display.setCursor(34, 120);
  carrier.display.setTextSize(2);
  carrier.display.print("Button touched:");
  carrier.display.setTextSize(5);
  carrier.display.setCursor(100, 180);
  carrier.display.print("__");

  bool buttonChecked[] = {false, false, false, false, false};
  carrier.leds.fill(carrier.leds.Color(0, 200, 0), 0, 5);

  carrier.leds.show();
  while (!touchCheckCompleted)
  {
    carrier.display.fillRect(90, 170, 100, 60, 0x00);
    carrier.display.setCursor(100, 180);
    carrier.display.print("__");

    // carrier.leds.show();
    carrier.Buttons.update();

    // Checks if new data are available
    if (carrier.Buttons.onTouchDown(TOUCH0))
    {
      Serial.println("Touching Button 0");
      carrier.display.fillRect(90, 160, 100, 60, 0x00);
      carrier.display.setCursor(95, 170);
      carrier.display.print("00");
      buttonChecked[0] = true;
      carrier.leds.setPixelColor(0, colorGreen);
    }
    if (carrier.Buttons.onTouchUp(TOUCH1))
    {
      Serial.println("Touching Button 1");
      carrier.display.fillRect(90, 160, 100, 60, 0x00);
      carrier.display.setCursor(95, 170);
      carrier.display.print("01");
      buttonChecked[1] = true;
      carrier.leds.setPixelColor(1, colorGreen);
    }
    if (carrier.Buttons.getTouch(TOUCH2))
    {
      Serial.println("Touching Button 2");
      carrier.display.fillRect(90, 160, 100, 60, 0x00);
      carrier.display.setCursor(95, 170);
      carrier.display.print("02");
      buttonChecked[2] = true;
      carrier.leds.setPixelColor(2, colorGreen);
    }
    if (carrier.Buttons.onTouchUp(TOUCH3))
    {
      Serial.println("Touching Button 3");
      carrier.display.fillRect(90, 160, 100, 60, 0x00);
      carrier.display.setCursor(95, 170);
      carrier.display.print("03");
      buttonChecked[3] = true;
      carrier.leds.setPixelColor(3, colorGreen);
    }
    if (carrier.Buttons.onTouchDown(TOUCH4))
    {
      Serial.println("Touching Button 4");
      carrier.display.fillRect(90, 160, 100, 60, 0x00);
      carrier.display.setCursor(95, 170);
      carrier.display.print("04");
      buttonChecked[4] = true;
      carrier.leds.setPixelColor(4, colorGreen);
    }
    carrier.leds.show();
    delay(150);

    if (buttonChecked[0] && buttonChecked[1] && buttonChecked[2] && buttonChecked[3] && buttonChecked[4])
    {
      delay(500);
      carrier.leds.fill(carrier.leds.Color(0, 0, 0), 0, 5);
      carrier.leds.show();
      carrier.display.fillScreen(0x0000);
      carrier.display.setTextColor(0xFFFF);
      carrier.display.setTextSize(3);
      carrier.display.setCursor(64, 40);
      carrier.display.print("BUTTONS");
      carrier.display.drawBitmap(70, 100, check, 100, 100, 0x1D10); //0x1DA086);

      delay(1500);
      touchCheckCompleted = true;
    }
  }
}

// Check the Env, IMU and light sensors

//testing the relays
void actuatorsPage()
{
  // end of the activity
  carrier.display.fillScreen(0x0000);
  carrier.display.setTextSize(3);
  carrier.display.drawBitmap(60, 50, carrier_logo, 120, 121, 0xFFFF); // display the MKR IoT Carrier image
  carrier.display.setCursor(45, 170);
  carrier.display.println("HW setup");
  carrier.display.setCursor(90, 200);
  carrier.display.println("done");

  // Playing a song
  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 8; thisNote++) {

    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    carrier.Buzzer.sound(melody[thisNote]);
    delay(noteDuration);
    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.0;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    carrier.Buzzer.noSound();
  }
}

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);

  carrier.display.fillScreen(0x0000);
  carrier.display.setTextColor(0xFFFF);
  carrier.display.setTextSize(2);

  carrier.display.setCursor(0, 30);
  carrier.display.print("SSID: ");
  carrier.display.print(WiFi.SSID());

  carrier.display.setCursor(0, 60);
  carrier.display.print("http://");
  carrier.display.print(ip);

}

void initWifi() {
  Serial.println("Access Point Web Server");

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    carrier.display.fillScreen(0x0000);
    carrier.display.setCursor(0, 60);
    carrier.display.print("Communication with WiFi module failed!");
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  Serial.print("Creating access point named: ");
  Serial.println(ssid);

  IPAddress ip(192, 168, 10, 1);

  WiFi.config(ip);

  status = WiFi.beginAP(ssid);
  if (status != WL_AP_LISTENING) {
    Serial.println("Creating access point failed");
    carrier.display.fillScreen(0x0000);
    carrier.display.setCursor(0, 60);
    carrier.display.print("Creating access point failed");
    while (true);
  }

  // wait 10 seconds for connection:
  delay(10000);

  // start the web server on port 80
  server.begin();
}

void checkWifi() {
  if (status != WiFi.status()) {
    // it has changed, so update the variable
    status = WiFi.status();

    if (status == WL_AP_CONNECTED) {
      byte remoteMac[6];

      // a device has connected to the AP
      Serial.print("Device connected to AP, MAC address: ");
    } else {
      // a device has disconnected from the AP, and we are back in listening mode
      Serial.println("Device disconnected from AP");
    }
  }
}

void newClient() {
  Serial.println("new client");           // print a message out the serial port
  String currentLine = "";                // make a String to hold incoming data from the client
  while (client.connected()) {            // loop while the client's connected
    if (client.available()) {             // if there's bytes to read from the client,
      char c = client.read();             // read a byte, then
      Serial.write(c);                    // print it out the serial monitor
      if (c == '\n') {                    // if the byte is a newline character

        // if the current line is blank, you got two newline characters in a row.
        // that's the end of the client HTTP request, so send a response:
        if (currentLine.length() == 0) {
          // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
          // and a content-type so the client knows what's coming, then a blank line:
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/html");
          client.println();

          // the content of the HTTP response follows the header:
          client.print("Click <a href=\"/H\">here</a> turn the LED on<br>");
          client.print("Click <a href=\"/L\">here</a> turn the LED off<br>");
          client.print("Random reading from analog pin: ");
          //          client.print(randomReading);

          // The HTTP response ends with another blank line:
          client.println();
          // break out of the while loop:
          break;
        }
        else {      // if you got a newline, then clear currentLine:
          currentLine = "";
        }
      }
      else if (c != '\r') {    // if you got anything else but a carriage return character,
        currentLine += c;      // add it to the end of the currentLine
      }
    }
  }
  // close the connection:
  client.stop();
}
