#include <WiFi.h>
#include <HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>
#include "time.h"

// WiFi credentials
const char* ssid = "HotspotPro";
const char* password = "12345678";

// Server details
const char* serverURL = "http://192.168.186.73/Annamrita/test.php";

// Time Settings
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 19800; 
const int   daylightOffset_sec = 0;

// Sensor Pin Definitions
#define MQ2_ANALOG_PIN 34   
#define MQ135_ANALOG_PIN 33  
#define FC28_PIN 35         
#define DS18B20_PIN 4       

// LCD Pin Definitions (Using verified test code settings)
#define LCD_RS 23
#define LCD_EN 5
#define LCD_D4 18
#define LCD_D5 19
#define LCD_D6 21
#define LCD_D7 22

OneWire oneWire(DS18B20_PIN);
DallasTemperature sensors(&oneWire);
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

float temperature = 0.0;
int moisture = 0;
int mq2Value = 0;
int mq135Value = 0;

unsigned long previousMillis = 0;
const long interval = 5000;

void setup() {
  Serial.begin(115200);
  delay(100);

  // Initialize LCD
  Serial.println("Initializing LCD...");
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi...");
  Serial.println("LCD Initialized!");

  // Initialize WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connected!");

  // Get time from NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  
  // Initialize Sensors
  sensors.begin();

  delay(2000);
  lcd.clear();
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    readSensorData();
    displayOnLCD();
    sendDataToServer();
  }
}

void readSensorData() {
  sensors.requestTemperatures();
  temperature = sensors.getTempCByIndex(0);
  
  moisture = analogRead(FC28_PIN);
  mq2Value = analogRead(MQ2_ANALOG_PIN);
  mq135Value = analogRead(MQ135_ANALOG_PIN);

  Serial.print("Temp: "); Serial.print(temperature); Serial.print("C | ");
  Serial.print("Moist: "); Serial.print(moisture); Serial.print(" | ");
  Serial.print("MQ2: "); Serial.print(mq2Value); Serial.print(" | ");
  Serial.print("MQ135: "); Serial.println(mq135Value);
}

void displayOnLCD() {
  lcd.clear();

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    lcd.setCursor(0, 0);
    lcd.print("No Time Sync");
    return;
  }

  char timeStr[9]; 
  sprintf(timeStr, "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

  // First Line: Time and Temperature
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperature, 1);
  lcd.print("C ");

  lcd.setCursor(10, 0);
  lcd.print(timeStr);

  // Second Line: Sensor Values
  lcd.setCursor(0, 1);
  lcd.print("M:");
  lcd.print(moisture);

  lcd.setCursor(8, 1);
  lcd.print("MQ2:");
  lcd.print(mq2Value);

  delay(2000);
}

void sendDataToServer() {
  HTTPClient http;
  http.begin(serverURL);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  String data = "temperature=" + String(temperature, 1) + 
                "&moisture=" + String(moisture) + 
                "&mq2=" + String(mq2Value) + 
                "&mq135=" + String(mq135Value);

  Serial.println("Sending Data: " + data);

  int httpResponseCode = http.POST(data);

  Serial.print("HTTP Response Code: ");
  Serial.println(httpResponseCode);  

  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("Server Response: " + response);
  } else {
    Serial.println("Error sending data!");
  }

  http.end();
}

