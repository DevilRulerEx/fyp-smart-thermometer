// New background colour
#define TFT_BROWN 0x38E0

// Pause in milliseconds between screens, change to 0 to time font rendering
#define WAIT 2000

// Wifi Connection
#include <WiFi.h>
const char* ssid     = "<Your wifi/hotspot network name>";
const char* password = "<Your password for the wifi/hotspot>";
WiFiClient client;
char server[] = "<IP Address>";
unsigned long dataPreviousTime = 0;
const long dataInterval = 10000;

//OLED display
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>
TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h
unsigned long targetTime = 0; // Used for testing draw times
const long interval = 2000; //The 'delay' time.
String status_message="";

//Temperature Sensor
#include <Wire.h>
#include <Adafruit_MLX90614.h>
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
String read_temperature="";
String send_temperature="";
boolean tempComplete = false;
float temp_offset = 3.0;

//Barcode Scanner
//#include <Arduino.h>
String inputString ="";
String inputName ="";
String lastinputName ="";
String testName ="TestName";
boolean stringComplete = false;
int countstr=0;
unsigned long millisendstr=0;

//Buttons
const int buttonSend = 2;
int sendState;
int lastSendState = HIGH;
unsigned long lastSendTime = 0;  // the last time the output pin was toggled
unsigned long sendDelay = 100;    // the debounce time; increase if the output flickers

const int buttonRecord = 13;
int recordState;
int lastRecordState = HIGH;
unsigned long lastRecordTime = 0;  // the last time the output pin was toggled
unsigned long recordDelay = 50;    // the debounce time; increase if the output flickers

const int buttonClear = 12;
int clearState;
int lastClearState = HIGH;
unsigned long lastClearTime = 0;  // the last time the output pin was toggled
unsigned long clearDelay = 0;    // the debounce time; increase if the output flickers

const int buttonScan = 15;
int scanState;
int lastScanState = HIGH;
unsigned long lastScanTime = 0;  // the last time the output pin was toggled
unsigned long scanDelay = 50;    // the debounce time; increase if the output flickers


void setup(void) {
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, 25, 26); //25 is TX (Black), 26 is RX (Yellow)

  tft.init();
  tft.setRotation(45); //Set to 0 for portrait, 45 for landscape.
  
  mlx.begin(); //22 is SCL (Yellow), 21 is SDA (Green)
  
  inputString.reserve(20);

  pinMode(buttonSend, INPUT_PULLUP);
  pinMode(buttonRecord, INPUT_PULLUP);
  pinMode(buttonClear, INPUT_PULLUP);
  pinMode(buttonScan, INPUT_PULLUP);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
    status_message="Connecting to WiFi";
  }
  Serial.println("Connected to the WiFi network");
  status_message="WiFi connected!";

}

void loop() {
  int sendReading = digitalRead(buttonSend);
  if (sendReading != lastSendState) {
    // reset the debouncing timer
    lastSendTime = millis();
  }

  int recordReading = digitalRead(buttonRecord);
  if (recordReading != lastRecordState) {
    // reset the debouncing timer
    lastRecordTime = millis();
  }

  int clearReading = digitalRead(buttonClear);
  if (clearReading != lastClearState) {
    // reset the debouncing timer
    lastClearTime = millis();
  }    

  int scanReading = digitalRead(buttonScan);
  if (scanReading != lastScanState) {
    // reset the debouncing timer
    lastScanTime = millis();
  }
  serialScanerEvent();
  sendState = digitalRead(buttonSend);
  recordState = digitalRead(buttonRecord);
  clearState = digitalRead(buttonClear);
  scanState = digitalRead(buttonScan);
  
  if (stringComplete) {
    Serial.println(inputString);
    Serial.println("String Complete!");
    inputName = String(inputString);
    inputName.replace(" ","_");
    inputName.trim();
//    status_message = "ID Recorded!";
    inputString = "";
    stringComplete = false;
    countstr=0;
    if (inputName != NULL)
    {
      status_message = "ID Recorded!";
    }
  }

  //Sending the data
  if ((millis() - lastSendTime) > sendDelay) {
    if (sendReading != sendState) {
      sendState = sendReading;
      if (sendState == LOW && inputName == NULL || send_temperature == NULL) {
        status_message = "Missing inputs!";
      }
    }
  }
  if (sendState == LOW && inputName != NULL && send_temperature != NULL) {
    Serial.println("Send button Pressed");
    status_message = "Attempting to send data...";
    send_temp_to_server();
  }

  //Record Temperature
  if (tempComplete && recordState == HIGH){
    Serial.println("Temperature Recorded!");
    status_message = "Temperature Recorded!";
    tempComplete = false;
  }

  //Clearing the data
  if ((millis() - lastClearTime) > clearDelay) {
    if (clearReading != clearState) {
      clearState = clearReading;
      if (clearState == LOW) {
        Serial.println("Clear button Pressed");
        status_message = "Data Cleared!";
        clear_data();
      }
    }
  }
     
  drawInformation();

  lastSendState = sendReading;
  lastRecordState = recordReading;
  lastClearState = clearReading;

}

void serialScanerEvent() {
  //
  if (Serial2.available()>0) {
    // get the new byte:
    char inChar = (char)Serial2.read();
    // add it to the inputString:
    inputString += inChar;
    countstr++;
    millisendstr=millis();
  }
  else {
    if(millis()-millisendstr>1000 && countstr>0) {
       stringComplete=true;
    }
  }
}

void drawInformation() {
  unsigned long currentMillis = millis();
  if (currentMillis - targetTime >= interval) {
    targetTime = currentMillis;
    tft.setTextSize(1);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
  
    tft.drawString("Name or ID:", 0, 0, 4);
    tft.drawString(inputName, 0, 24, 2);

    tft.drawString("Temperature: ", 0, 48, 4);
    tft.setTextSize(2);
    read_temperature = String(mlx.readObjectTempC()+temp_offset);
    tft.drawString(send_temperature+"°C", 0, 72, 4);

    if (recordState == LOW) {
      Serial.println("Record button Pressed");
      status_message = "Recording temperature....";
      send_temperature = read_temperature;
      tempComplete = true;
      tft.setTextColor(TFT_YELLOW, TFT_BLACK);
      tft.drawString(read_temperature+"°C", 0, 72, 4);
    }
     

    tft.setTextSize(1);
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.drawString(status_message, 0, 120, 2);
  }
}

void send_temp_to_server() {
  client.stop();
  unsigned long dataCurrentTime = millis();
  
  if (client.connect(server, 80)) {    
    Serial.println("connected");
    //Make a HTTP request:
    String str="/get_temperature.php?inputName=";  //Calling the PHP file.
    String str2=String(inputName);
    str+=str2;
    str+="&send_temperature="+send_temperature;
    Serial.print("str=");Serial.println(str);
  
    client.println("GET "+str+" HTTP/1.1");
    client.println("Host: " + server); 
    client.println("Connection: close");
    client.println();

    status_message = "Data sent!";
    clear_data();

  } else {
    status_message = "Connection failed";
    Serial.println("connection failed");
  }
}

void clear_data(){
  inputName = "";
  send_temperature = "";
}
