// New background colour
#define TFT_BROWN 0x38E0

// Pause in milliseconds between screens, change to 0 to time font rendering
#define WAIT 2000

// Wifi Connection
#include <WiFi.h>
//const char* ssid     = "Chng's Family";
//const char* password = "werty113";
const char* ssid     = "NTUSECURE";
const char* password = "!Werty113";
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
boolean stringComplete = false;
int countstr=0;
unsigned long millisendstr=0;


void setup(void) {
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, 25, 26); //25 is TX, 26 is RX

  tft.init();
  tft.setRotation(0);
  
  mlx.begin(); //22 is SCL, 21 is SDA
  
  inputString.reserve(50);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");

}

void loop() {
  serialScanerEvent();
  //
  if (stringComplete) {
    Serial.println(inputString);
    Serial.println("String Complete!");
    inputName = inputString;
    inputString = "";
    send_temperature = read_temperature;
    stringComplete = false;
    countstr=0;
   }
//  if (inputName != NULL){
//    send_temp_to_server();
//    inputName = "";
//  }
   
  drawInformation();

//  send_temp_to_server();
//  send_fixed_data_to_server();
  delay(20000);
  
//  targetTime = millis();

  // First we test them with a background colour set
//  tft.setTextSize(1);
//  tft.fillScreen(TFT_BLACK);
//  tft.setTextColor(TFT_GREEN, TFT_BLACK);
//
//  tft.drawString("Name:", 0, 0, 4);
//  tft.drawString("Chng Wee Ping", 0, 24, 2);
//  tft.drawString("ID:", 0, 48, 4);
//  tft.drawString("S1234567A", 0, 72, 2);
//  tft.drawString("Temperature:", 0, 96, 4);
//  tft.drawString(String(mlx.readObjectTempC()), 0, 120, 2);
    
  //delay(WAIT);
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

void send_temp_to_server() {
  client.stop();
  unsigned long dataCurrentTime = millis();
  
  if (client.connect(server, 80)) {    
    Serial.println("connected");
    //Make a HTTP request:
    String str="/get_temperature.php?inputName=";
    str+=inputName;
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

void drawInformation() {
  unsigned long currentMillis = millis();
  if (currentMillis - targetTime >= interval) {
    targetTime = currentMillis;
    tft.setTextSize(1);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
  
    tft.drawString("Name:", 0, 0, 4);
    tft.drawString(inputName, 0, 24, 2);
    tft.drawString("ID:", 0, 48, 4);
    tft.drawString("S1234567A", 0, 72, 2);
    tft.drawString("Temperature:", 0, 96, 4);
    
    read_temperature = String(mlx.readObjectTempC());
    tft.drawString(read_temperature, 0, 120, 2);
    //tft.drawString(String(mlx.readObjectTempC()), 0, 120, 2);
  }
}

void send_fixed_data_to_server(){
  client.stop();
  unsigned long dataCurrentTime = millis();
  
  if (client.connect(server, 80)) {    
    Serial.println("connected");
    //Make a HTTP request:
    String str="/get_temperature.php?inputName=";
    str+="TestName";
    str+="&send_temperature=";
    str+="3.1";
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
