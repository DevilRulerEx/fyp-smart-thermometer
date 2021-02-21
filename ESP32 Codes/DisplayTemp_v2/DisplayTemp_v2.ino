// New background colour
#define TFT_BROWN 0x38E0

// Pause in milliseconds between screens, change to 0 to time font rendering
#define WAIT 2000

// Wifi Connection
#include <WiFi.h>
const char* ssid     = "Chng's Family";
const char* password = "werty113";
//const char* ssid     = "NTUSECURE";
//const char* password = "!Werty113";
WiFiClient client;
char server[] = "192.168.1.93";
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

const int buttonClear = 13;
int clearState;
int lastClearState = HIGH;
unsigned long lastClearTime = 0;  // the last time the output pin was toggled
unsigned long clearDelay = 50;    // the debounce time; increase if the output flickers


void setup(void) {
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, 25, 26); //25 is TX, 26 is RX

  tft.init();
  tft.setRotation(45); //Set to 0 for portrait, 45 for landscape.
  
  mlx.begin(); //22 is SCL, 21 is SDA
  
  inputString.reserve(20);

  pinMode(buttonSend, INPUT_PULLUP);
  pinMode(buttonRecord, INPUT_PULLUP);

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
  
  serialScanerEvent();
  sendState = digitalRead(buttonSend);
  recordState = digitalRead(buttonRecord);
  
  if (stringComplete) {
    Serial.println(inputString);
    Serial.println("String Complete!");
    inputName = String(inputString);
    inputName.replace(" ","_");
    inputName.trim();
    status_message = "ID Recorded!";
    inputString = "";
    //send_temperature = read_temperature;
    stringComplete = false;
    countstr=0;
  }

  //Sending the data
  if ((millis() - lastSendTime) > sendDelay) {
    if (sendReading != sendState) {
      sendState = sendReading;
      if (sendState == LOW && inputName != NULL && send_temperature != NULL) {
//      if (sendState == LOW) {
        Serial.println("Send button Pressed");
        status_message ="Sending data...";
        send_temp_to_server();
        clear_data();
      }
      if (sendState == LOW && inputName == NULL || send_temperature == NULL) {
        status_message = "Missing inputs!";
      }
    }
  }

  //Reading the data
  if ((millis() - lastRecordTime) > recordDelay) {
    if (recordReading != recordState) {
      recordState = recordReading;
      if (recordState == LOW) {
        Serial.println("Record button Pressed");
        status_message = "Temp Recorded!";
        send_temperature = read_temperature;
      }
    }
  }  
     
  drawInformation();

  lastSendState = sendReading;
  lastRecordState = recordReading;
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
    tft.drawString("Temperature: "+send_temperature, 0, 48, 4);
    read_temperature = String(mlx.readObjectTempC());
    tft.drawString("Reading: "+read_temperature, 0, 72, 4);
    //tft.drawString("Reading:", 0, 96, 4);
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
    String str="/get_temperature.php?inputName=";
    String str2=String(inputName);
    str+=str2;
    str+="&send_temperature="+send_temperature;
    Serial.print("str=");Serial.println(str);
  
    client.println("GET "+str+" HTTP/1.1");
    client.println("Host: 192.168.1.93");
    client.println("Connection: close");
    client.println();

    status_message = "Data sent!";

  } else {
    Serial.println("connection failed");
    status_message = "Connection failed!";
  }
}

void clear_data(){
  inputName = "";
  send_temperature = "";
}

void send_fixed_data_to_server(){
  client.stop();
  unsigned long dataCurrentTime = millis();
  
  if (client.connect(server, 80)) {    
    Serial.println("connected");
    //Make a HTTP request:
    String str="/get_temperature.php?inputName=";
    str+="TestName";
//    str+="&send_temperature=";
//    str+="3.1";
    str+="&send_temperature="+send_temperature;
    Serial.print("str=");Serial.println(str);
  
    client.println("GET "+str+" HTTP/1.1");
    client.println("Host: 192.168.1.93");
    client.println("Connection: close");
    client.println();

//    delay(10);
//    if (dataCurrentTime - dataPreviousTime >= dataInterval){
//
//    }
  } else {
    Serial.println("connection failed");
  }
}