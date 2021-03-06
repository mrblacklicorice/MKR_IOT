#include <Arduino_MKRIoTCarrier.h>
#include "visuals.h"
#include "pitches.h"
#include <SPI.h>
#include <WiFiNINA.h>

char ssid[] = "";        // your network SSID (name)
int keyIndex = 0;                // your network key Index number (needed only for WEP)
int wifi[5] = { -1, -1, -1, -1, -1};
int curr_wifi = -1;

MKRIoTCarrier carrier;

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
void touchpage();
void actuatorsPage();
void connectWiFi();
void listNetworks();
void printMacAddress(byte mac[]);
void print_WiFI_connections();
//void c_arr();

//Declare their flags
bool touchCheckCompleted = false;
bool shakeCheckCompleted = false;
bool colorCheckCompleted = false;
bool gestureCheckCompleted = false;
bool relay1CheckCompleted = false;
bool relay2CheckCompleted = false;

File myFile;

void setup()
{
  CARRIER_CASE = false;
  Serial.begin(9600);

//    while (!Serial) {
//      ; // wait for serial port to connect. Needed for native USB
//    }
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
  }

  if (!SD.begin(SD_CS)) {
    Serial.println("initialization failed!");
    carrier.display.setCursor(0, 30);
    carrier.display.print("initialization failed!");
    while (1);
  }

  Serial.println("initialization done.");

  //   open the file for reading:
  if (SD.exists("NAME.txt")) {
    configure();
    //    touchpage();
    actuatorsPage();

    myFile = SD.open("NAME.txt");
    Serial.println("NAME opened");
    while (myFile.available()) {
      device_name += char(myFile.read());
    }

    carrier.display.fillScreen(0x0000);
    carrier.display.setCursor(0, 0);
    carrier.display.print(device_name);
    delay(1000);
  } else {
    myFile = SD.open("NAME.txt", FILE_WRITE);
    myFile.print("SAMANTHA");
    device_name = "SAMANTHA";
    myFile.close();

    Serial.println("done.");
    carrier.display.setCursor(0, 30);
    carrier.display.print("Named SAMANTHA");
  }
}

void loop()
{
  carrier.display.setTextColor(0xFFFF);
  print_WiFI_connections();
  //listNetworks();
  delay(500);
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


void listNetworks() {
  int numSsid = WiFi.scanNetworks();
  curr_wifi = -1;

  if (numSsid == -1) {
    Serial.println("Couldn't get a wifi connection");
    carrier.display.setTextColor(0xFFFF);
    carrier.display.setTextSize(5);
    carrier.display.setCursor(0, 0);
    carrier.display.print("                              ");

    carrier.display.setTextColor(0x0000);
    carrier.display.setTextSize(3);
  } else {
    carrier.display.fillScreen(0x0000);

    for (int c = 0; c < 5; c++) {
      wifi[c] = -1;
    }

    Serial.print("number of available networks:");
    Serial.println(numSsid);

    int count = 0;
    for (int thisNet = 0; thisNet < numSsid; thisNet++) {
      if (WiFi.encryptionType(thisNet) == ENC_TYPE_NONE && count < 5) {
        wifi[count] = thisNet;

        Serial.println(WiFi.SSID(thisNet));
        carrier.display.setTextSize(2);
        carrier.display.setTextColor(0xFFFF);

        carrier.display.setCursor(0 , 30 * count);
        carrier.display.print(String(count) + " - " + String(WiFi.SSID(thisNet)));

        count++;
      }
    }

    bool done = false;
    while (!done) {
      carrier.Buttons.update();
      if (carrier.Buttons.onTouchDown(TOUCH0) && wifi[0] > -1)
      {
        curr_wifi = 0;
        done = true;
      }
      else if (carrier.Buttons.onTouchUp(TOUCH1) && wifi[1] > -1)
      {
        curr_wifi = 1;
        done = true;
      }
      else if (carrier.Buttons.getTouch(TOUCH2) && wifi[2] > -1)
      {
        curr_wifi = 2;
        done = true;
      }
      else if (carrier.Buttons.onTouchUp(TOUCH3) && wifi[3] > -1)
      {
        curr_wifi = 3;
        done = true;
      }
      else if (carrier.Buttons.onTouchDown(TOUCH4) && wifi[4] > -1)
      {
        curr_wifi = 4;
        done = true;
      }
      carrier.leds.show();
    }
    
    WiFi.begin(WiFi.SSID(wifi[curr_wifi]));
    carrier.leds.setPixelColor(curr_wifi, colorGreen);
    delay(1000);
    
    Serial.println((String(WiFi.SSID(wifi[curr_wifi]))) + ".txt");
    myFile = SD.open(String(WiFi.SSID(wifi[curr_wifi])) + ".txt", FILE_WRITE);
    myFile.print("");
    myFile.close();
    
    Serial.println("turning leds off");
    carrier.leds.fill(carrier.leds.Color(0, 0, 0), 0, 5);
    carrier.leds.show();
    carrier.Buttons.update();
    carrier.display.fillScreen(0x0000);
    delay(10000);
    
    carrier.display.setTextColor(0x0000);
    carrier.display.setTextSize(5);
    carrier.display.setCursor(0, 0);
    carrier.display.print("                     ");

    carrier.display.setTextColor(0xFFFF);
    carrier.display.setTextSize(3);
    carrier.display.setCursor(0, 0);
    carrier.display.print(WiFi.SSID(wifi[curr_wifi]));
  }
}

void connectWiFi() {
  Serial.println("wifi connected started");

  carrier.display.fillScreen(0x0000);

  bool wifi_connected = false;
  int num = 0;
  int total_networks = WiFi.scanNetworks();

  while (num < 10 && !wifi_connected) {
    if (total_networks > num && WiFi.encryptionType(num) == ENC_TYPE_NONE && SD.exists(String(WiFi.SSID(num)) + ".txt")) {
      WiFi.begin(WiFi.SSID(num));
      delay(1000);

      carrier.display.setTextColor(0x0000);
      carrier.display.setTextSize(5);
      carrier.display.setCursor(0, 0);
      carrier.display.print("                     ");

      carrier.display.setTextColor(0xFFFF);
      carrier.display.setTextSize(3);
      carrier.display.setCursor(0, 0);
      carrier.display.print(WiFi.SSID(num));
      wifi_connected = true;
    } else {
      Serial.print("Not connected to: ");
      Serial.println(WiFi.SSID(num));
      wifi_connected = false;
    }
    num++;
  }

  if (num == 10) {
    listNetworks();
  }
}

void printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
}

void print_WiFI_connections() {
  int status = WiFi.status();
  if (status != WL_CONNECTED) {
    carrier.display.setTextColor(0x0000);
    carrier.display.setTextSize(5);
    carrier.display.setCursor(0, 60);
    carrier.display.print("                     ");

    carrier.display.setTextColor(0xF800);
    carrier.display.setTextSize(3);
    carrier.display.setCursor(0, 60);
    carrier.display.print("Not Connected");

    connectWiFi();
  } else {
      carrier.display.setTextColor(0x0000);
      carrier.display.setTextSize(5);
      carrier.display.setCursor(0, 0);
      carrier.display.print("                     ");

      carrier.display.setTextColor(0xFFFF);
      carrier.display.setTextSize(3);
      carrier.display.setCursor(0, 0);
      carrier.display.print(WiFi.SSID());

      carrier.display.setTextColor(0x0000);
      carrier.display.setTextSize(3);
      carrier.display.setCursor(0, 30);
      carrier.display.print("           ");

      carrier.display.setTextColor(0xFFFF);
      carrier.display.setTextSize(3);
      carrier.display.setCursor(0, 30);
      carrier.display.print(WiFi.RSSI());
    
    carrier.display.setTextColor(0x0000);
    carrier.display.setTextSize(5);
    carrier.display.setCursor(0, 60);
    carrier.display.print("                     ");

    carrier.display.setTextColor(0x07E0);
    carrier.display.setTextSize(3);
    carrier.display.setCursor(0, 60);
    carrier.display.print("Connected");
  }
}

//void c_arr(String init_str, char fin_arr[]) {
//  int str_len = init_str.length() + 1;
//  char str_char[str_len];
//  init_str.toCharArray(str_char, str_len);
//  strcat(fin_arr, str_char);
//}
