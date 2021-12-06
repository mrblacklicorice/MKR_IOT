#include <Arduino_MKRIoTCarrier.h>
#include "visuals.h"
#include "pitches.h"
#include <SPI.h>
#include <WiFiNINA.h>
#include <LinkedList.h>

char ssid[] = "";        // your network SSID (name)
LinkedList<String> wifi_list;
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
void c_arr();

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

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }
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
    myFile = SD.open("SSID.txt");
    Serial.println("SSID opened");
    String SD_data = "";

    while (myFile.available()) {
      char ltr = myFile.read();
      if (ltr == 10) {
        wifi_list.add(SD_data);
      } else {
        SD_data += char(myFile.read());
      }
    }
    wifi_list.add(SD_data);



    myFile = SD.open("NAME.txt");
    Serial.println("NAME opened");
    while (myFile.available()) {
      device_name += char(myFile.read());
    }

    carrier.display.fillScreen(0x0000);
//    carrier.display.setCursor(0, 0);
//    carrier.display.print(ssid);
    carrier.display.setCursor(0, 30);
    carrier.display.print(device_name);
    delay(1000);

    //    configure();
    //    touchpage();
    //    sensorsPage();
    //    actuatorsPage();
  } else {
    myFile = SD.open("SSID.txt", FILE_WRITE);
    myFile.print("");
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
listNetworks();
delay(10000);
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
void sensorsPage()
{
  carrier.display.fillScreen(0x0000);

  // displaying temperature
  carrier.display.setCursor(25, 60);
  carrier.display.setTextSize(3);
  carrier.display.print("Temperature");
  float temperature = carrier.Env.readTemperature();   //storing temperature value in a variable
  carrier.display.drawBitmap(80, 80, temperature_logo, 100, 100, 0xDAC9); //0xDA5B4A); //draw a thermometer on the MKR IoT carrier's display
  carrier.display.setCursor(60, 180);
  carrier.display.print(temperature); // display the temperature on the screen
  carrier.display.print(" C");
  delay(2500);

  // displaying humidity
  // displaying humidity
  carrier.display.fillScreen(0x0000);
  carrier.display.setCursor(54, 40);
  carrier.display.setTextSize(3);
  carrier.display.print("Humidity");
  carrier.display.drawBitmap(70, 70, humidity_logo, 100, 100, 0x0D14); //0x0CA1A6); //draw a humidity figure on the MKR IoT carrier's display
  float humidity = carrier.Env.readHumidity(); //storing humidity value in a variable
  carrier.display.setCursor(60, 180);
  carrier.display.print(humidity); // display the humidity on the screen
  carrier.display.print(" %");
  delay(2500);

  // displaying pressure
  carrier.display.fillScreen(0x0000);
  carrier.display.setCursor(54, 40);
  carrier.display.setTextSize(3);
  carrier.display.print("Pressure");
  carrier.display.drawBitmap(70, 60, pressure_logo, 100, 100, 0xF621); //0xF1C40F); //draw a pressure figure on the MKR IoT carrier's display
  float press = carrier.Pressure.readPressure(); //storing pressure value in a variable
  carrier.display.setCursor(40, 160);
  carrier.display.println(press); // display the pressure on the screen
  carrier.display.setCursor(160, 160);
  carrier.display.print("KPa");
  delay(2500);

  // reading acceleration values to detect shaking
  float x, y, z, idleX;
  while (!carrier.IMUmodule.accelerationAvailable())
    ;
  if (carrier.IMUmodule.accelerationAvailable())
  {
    carrier.IMUmodule.readAcceleration(x, y, z);
  }

  idleX = x; // stroing the intial acceleration on the x axis

  //displaying the shaking page on the display
  carrier.display.fillScreen(0x0000);
  carrier.display.setTextSize(3);
  carrier.display.setCursor(75, 40);
  carrier.display.println("Shake");
  carrier.display.setCursor(30, 70);
  carrier.display.println("your device");
  carrier.display.drawBitmap(28, 90, shake_logo, 196, 100, 0x9C2D); //0x9E846D); // displaying a shake figure
  carrier.leds.fill(carrier.leds.Color(0, 0, 90), 0, 5);
  carrier.leds.show();

  delay(2500);

  //Wait for the shake event
  while (!shakeCheckCompleted)
  {

    if (carrier.IMUmodule.accelerationAvailable())
    {
      carrier.IMUmodule.readAcceleration(x, y, z);
      Serial.println(x);
    }
    if (x > idleX + 2) //if the acceleration is more than the intial value, a movement is detected
    {
      shakeCheckCompleted = true;
    }
  }

  // provide a feedback on the display when movment is successfully detected
  carrier.display.fillScreen(0x0000);
  carrier.display.setTextColor(0xFFFF);
  carrier.display.setTextSize(3);
  carrier.display.setCursor(50, 40);
  carrier.display.print("MOVEMENT");
  carrier.display.drawBitmap(70, 100, check, 100, 100, 0x1D10); //0x1DA086);
  carrier.leds.fill(carrier.leds.Color(200, 0, 0), 0, 5);
  carrier.leds.show();
  delay(2500);
  carrier.leds.fill(carrier.leds.Color(0, 0, 0), 0, 5);
  carrier.leds.show();


  //Color detection
  carrier.display.fillScreen(0x0000);
  carrier.display.setTextColor(0xFFFF);
  carrier.display.setTextSize(3);
  carrier.display.setCursor(80, 40);
  carrier.display.print("Color");
  carrier.display.setCursor(40, 70);
  carrier.display.print("detection");
  carrier.display.drawBitmap(70, 90, color_logo, 100, 100, 0xF324); //0xF26727);
  delay (2500);

  carrier.display.fillScreen(0x0000);
  carrier.display.setTextSize(3);
  carrier.display.setCursor(74, 72);
  carrier.display.setTextSize(2);
  carrier.display.setCursor(30, 132);
  carrier.display.print("Cover the sensor");
  carrier.display.setCursor(60, 152);
  carrier.display.println("with a white");
  carrier.display.setCursor(90, 172);
  carrier.display.println("object");
  carrier.display.drawBitmap(90, 190, arrow_down, 61, 61, 0xFFFF); //displaying an arrow figure
  carrier.leds.fill(carrier.leds.Color(120, 120, 120), 0, 5);
  carrier.leds.show();
  while (!colorCheckCompleted)
  {
    int r, g, b;
    while (!carrier.Light.colorAvailable())
    {
      delay(5);
    }

    carrier.Light.readColor(r, g, b); //read rgb colour values
    carrier.display.fillRect(0, 0, 240, 60, carrier.display.color565(r, g, b));

    if (r >= 135 && g >= 135 && b >= 135) // check if color/light is bright enough
    {
      delay(1000);
      carrier.leds.fill(carrier.leds.Color(0, 0, 0), 0, 5);
      carrier.leds.show();
      carrier.display.fillScreen(0x0000);
      carrier.display.setTextColor(0xFFFF);
      carrier.display.setTextSize(3);
      carrier.display.setCursor(80, 40);
      carrier.display.print("COLOR");
      carrier.display.drawBitmap(70, 100, check, 100, 100, 0x1D10); //0x1DA086);

      delay(500);
      colorCheckCompleted = true;
    }

    delay(1000);
  }

  delay(2500);

  // Gesture detection
  carrier.display.fillScreen(0x0000);
  carrier.display.setTextColor(0xFFFF);
  carrier.display.setTextSize(3);
  carrier.display.setCursor(60, 40);
  carrier.display.print("Gesture");
  carrier.display.setCursor(38, 65);
  carrier.display.print("detection");
  carrier.display.drawBitmap(70, 90, gesture_logo, 100, 100, 0x0D14); //0x0CA1A6);
  delay (2500);

  carrier.display.fillScreen(0x0000);
  carrier.display.setTextSize(2);
  carrier.display.setCursor(40, 75);
  carrier.display.print("Move your hand ");
  carrier.display.setCursor(70, 95);
  carrier.display.print("UP, DOWN,");
  carrier.display.setCursor(55, 115);
  carrier.display.print("LEFT, RIGHT");
  carrier.display.setCursor(45, 135);
  carrier.display.print("in parallel");
  carrier.display.setCursor(30, 155);
  carrier.display.print("from the board");
  carrier.leds.fill(carrier.leds.Color(0, 0, 200), 0, 5);
  carrier.leds.show();

  carrier.display.setCursor(0, 0);
  carrier.display.drawBitmap(-15, 90, arrow_left, 61, 61, 0xFFFF); //displaying an arrow figure
  carrier.display.drawBitmap(90, -15, arrow_up, 61, 61, 0xFFFF); //displaying an arrow figure
  carrier.display.drawBitmap(190, 90, arrow_right, 61, 61, 0xFFFF); //displaying an arrow figure
  carrier.display.drawBitmap(90, 190, arrow_down, 61, 61, 0xFFFF); //displaying an arrow figure

  bool gestureChecked[] = {false, false, false, false}; // An array to keep track of the detected gestures

  // Wait Gesture detection event
  while (!gestureCheckCompleted)
  {
    if (carrier.Light.gestureAvailable())
    {
      uint8_t gesture = carrier.Light.readGesture(); // a variable to store the type of gesture read by the light sensor
      Serial.print("Gesture: ");
      // when gesture is UP
      if (gesture == UP)
      {
        gestureChecked[0] = true; // assign the first item in the array to true
        carrier.display.drawBitmap(90, -15, arrow_up, 61, 61, 0x07E0); //displaying an arrow figure
        carrier.leds.fill(carrier.leds.Color(0, 0, 0), 0, 5);
        carrier.leds.show();
        delay(150);
        carrier.leds.fill(carrier.leds.Color(0, 0, 200), 0, 5);
        carrier.leds.show();
        Serial.println("UP");
        Serial.println(gestureChecked[0]);
      }
      // when gesture is DOWN
      if (gesture == DOWN)
      {
        gestureChecked[1] = true; // assign the second item in the array to true
        carrier.display.drawBitmap(90, 190, arrow_down, 61, 61, 0x07E0); //displaying an arrow figure
        carrier.leds.fill(carrier.leds.Color(0, 0, 0), 0, 5);
        carrier.leds.show();
        delay(150);
        carrier.leds.fill(carrier.leds.Color(0, 0, 200), 0, 5);
        carrier.leds.show();
        Serial.println("DOWN");
        Serial.println(gestureChecked[1]);
      }
      //when gesture is to the RIGHT
      if (gesture == RIGHT)
      {
        gestureChecked[2] = true; // assign the third item in the array to true
        carrier.display.drawBitmap(190, 90, arrow_right, 61, 61, 0x07E0); //displaying an arrow figure
        carrier.leds.fill(carrier.leds.Color(0, 0, 0), 0, 5);
        carrier.leds.show();
        delay(150);
        carrier.leds.fill(carrier.leds.Color(0, 0, 200), 0, 5);
        carrier.leds.show();
        Serial.println("RIGH");
        Serial.println(gestureChecked[2]);
      }
      // when gesture is to the LEFT
      if (gesture == LEFT)
      {
        gestureChecked[3] = true; // assign the forth item in the array to true
        carrier.display.drawBitmap(-15, 90, arrow_left, 61, 61, 0x07E0); //displaying an arrow figure
        carrier.leds.fill(carrier.leds.Color(0, 0, 0), 0, 5);
        carrier.leds.show();
        delay(150);
        carrier.leds.fill(carrier.leds.Color(0, 0, 200), 0, 5);
        carrier.leds.show();
        Serial.println("LEFT");
        Serial.println(gestureChecked[3]);
      }
      // if all items in the array are true (all gestures are detected)
      if (gestureChecked[0] && gestureChecked[1] && gestureChecked[2] && gestureChecked[3])
      {
        delay(500);
        carrier.display.fillScreen(0x0000);
        carrier.display.setTextColor(0xFFFF);
        carrier.display.setTextSize(3);
        carrier.display.setCursor(64, 40);
        carrier.display.print("GESTURE");
        carrier.display.drawBitmap(70, 100, check, 100, 100, 0x1D10); //0x1DA086);
        carrier.leds.fill(carrier.leds.Color(200, 0, 0), 0, 5);
        carrier.leds.show();
        delay(1500);
        carrier.leds.fill(carrier.leds.Color(0, 0, 0), 0, 5);
        gestureCheckCompleted = true;
      }
    }
  }

  delay(1500);
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
  Serial.println("** Scan Networks **");
  int numSsid = WiFi.scanNetworks();

  if (numSsid == -1) {
    Serial.println("Couldn't get a wifi connection");
    while (true);
  }

  Serial.print("number of available networks:");
  Serial.println(numSsid);

  for (int thisNet = 0; thisNet < numSsid; thisNet++) {
    Serial.print(thisNet);
    Serial.print(") ");
    Serial.print(WiFi.SSID(thisNet));
    Serial.print("\tSignal: ");
    Serial.print(WiFi.RSSI(thisNet));
    Serial.print(" dBm");
    Serial.print("\tEncryption: ");
    printEncryptionType(WiFi.encryptionType(thisNet));

  }
}

void printEncryptionType(int thisType) {
  switch (thisType) {
    case ENC_TYPE_WEP:
      Serial.println("WEP");
      break;

    case ENC_TYPE_TKIP:
      Serial.println("WPA");
      break;

    case ENC_TYPE_CCMP:
      Serial.println("WPA2");
      break;

    case ENC_TYPE_NONE:
      Serial.println("None");
      break;

    case ENC_TYPE_AUTO:
      Serial.println("Auto");
      break;

    case ENC_TYPE_UNKNOWN:
    default:
      Serial.println("Unknown");
      break;
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

void c_arr(String init_str, char* fin_arr) {
  int str_len = init_str.length() + 1;
  char str_char[str_len];
  init_str.toCharArray(str_char, str_len);
  strcat(fin_arr, str_char);
}
