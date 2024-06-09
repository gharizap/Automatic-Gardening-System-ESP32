#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TimeLib.h>

#define DHTPIN 15     // Pin where the DHT22 sensor is connected
#define DHTTYPE DHT22   // DHT22 sensor type

DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal_I2C lcd(0x27, 20, 4);  // I2C LCD address: 0x27, 20 characters, 4 lines

int LDR_PIN = 35;  // Pin where the LDR sensor is connected
int POT_PIN = 34;  // Pin where the potentiometer is connected
int LED_PIN = 33;   // Pin where the LED is connected
int RELAY_PIN = 18; // Pin where the relay is connected
int SERVO_PIN = 19; // Pin where the first servo is connected
int SERVO_PIN2 = 5; // Pin where the second servo is connected

float temperature;
float humidity;
int ldrValue;
int potValue;

unsigned long previousMillis = 0;
const long interval = 60000; // 1 minute interval

Servo myservo;  // Create a servo object for the watering servo
Servo myservo2; // Create a servo object for the pesticide servo

// WiFi credentials
const char* ssid     = "Wokwi-GUEST";
const char* password = "";
#define WIFI_CHANNEL 6

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 7 * 3600);  // GMT+7 for Indonesia

bool hasDispensedToday = false; // Flag to indicate if the pesticide has been dispensed today

void setup() {
  Serial.begin(9600);
  dht.begin();
  lcd.init();
  lcd.backlight();
  
  pinMode(LDR_PIN, INPUT);
  pinMode(POT_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  
  myservo.attach(SERVO_PIN); // Attach the watering servo to the specified pin
  myservo2.attach(SERVO_PIN2); // Attach the pesticide servo to the specified pin
  myservo.write(0); // Initialize the watering servo to 0 degrees
  myservo2.write(0); // Initialize the pesticide servo to 0 degrees

  // Connect to Wi-Fi
  WiFi.begin(ssid, password, WIFI_CHANNEL);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize the time client
  timeClient.begin();

  // Initialize with random values
  updateRandomValues();

  // Display startup animation
  displayStartupAnimation();
}

void loop() {
  unsigned long currentMillis = millis();

  // Check if 1 minute has passed
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    updateRandomValues();
  }

  // Update the time
  timeClient.update();
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();

  // Display temperature and humidity on the LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print(" C");
  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(humidity);
  lcd.print(" %");
  
  // Display LDR and potentiometer values on the LCD
  lcd.setCursor(0, 2);
  lcd.print("LDR Value: ");
  lcd.print(ldrValue);
  lcd.setCursor(0, 3);
  lcd.print("Pot Value: ");
  lcd.print(potValue);
  
  // Display LDR and potentiometer values on the Serial Monitor
  Serial.print("LDR Value: ");
  Serial.println(ldrValue);
  Serial.print("Pot Value: ");
  Serial.println(potValue);
  
  // Set the LED state based on the LDR value
  if (ldrValue > 500) {
    digitalWrite(LED_PIN, HIGH); // Turn on the LED if light is low
  } else {
    digitalWrite(LED_PIN, LOW);  // Turn off the LED if light is sufficient
  }
  
  // Set the relay state based on the potentiometer value
  if (potValue > 512) {
    digitalWrite(RELAY_PIN, HIGH); // Activate the relay if the potentiometer is more than halfway
  } else {
    digitalWrite(RELAY_PIN, LOW);  // Deactivate the relay if the potentiometer is less than halfway
  }

  // Control the watering servo based on the humidity value
  if (humidity < 50) {
    myservo.write(90); // Rotate the watering servo to 90 degrees to start watering
  } else {
    myservo.write(0);  // Rotate the watering servo back to 0 degrees to stop watering
  }

  // Control the pesticide servo based on the time
  if (currentHour == 4 && currentMinute == 0 && !hasDispensedToday) {  // 4:00 AM
    myservo2.write(90); // Rotate the pesticide servo to 90 degrees to spray pesticide
    delay(60000); // Keep the servo in position for 1 minute (60000 ms)
    myservo2.write(0);  // Rotate the pesticide servo back to 0 degrees to stop spraying
    hasDispensedToday = true; // Set the flag to indicate that the pesticide has been dispensed today
  }

  // Reset the flag at midnight
  if (currentHour == 0 && currentMinute == 0) {
    hasDispensedToday = false;
  }
  
  delay(1000); // Wait 1 second before updating the display again
}

void updateRandomValues() {
  // Generate random temperature between 20°C and 30°C with slight variations
  temperature = 25 + random(-50, 51) / 10.0;
  
  // Generate random humidity between 40% and 60% with slight variations
  humidity = 50 + random(-100, 101) / 10.0;
  
  // Generate random LDR value between 300 and 700
  ldrValue = 500 + random(-200, 201);
  
  // Generate random potentiometer value between 400 and 600
  potValue = 500 + random(-100, 101);
}

void displayStartupAnimation() {
  String message1 = "Automatic";
  String message2 = "Gardening";
  String message3 = "System";
  String message4 = "Ghariza";
  String message5 = "Arif";
  String message6 = "Daffa";
  String message7 = "Jefta";
  String message8 = "Ardhev";

  lcd.clear();
  lcd.setCursor(0, 0);
  for (int i = 0; i < message1.length(); i++) {
    lcd.print(message1.charAt(i));
    delay(100);
  }

  lcd.setCursor(0, 1);               

  for (int i = 0; i < message2.length(); i++) {
    lcd.print(message2.charAt(i));
    delay(100);
  }

  lcd.setCursor(0, 2);
  
  for (int i = 0; i < message3.length(); i++) {
    lcd.print(message3.charAt(i));
    delay(100);
  }

  delay(2000);
   // Wait for 2 seconds before clearing the display
  lcd.clear();
  lcd.setCursor(0, 0);
  for (int i = 0; i < message4.length(); i++) {
    lcd.print(message4.charAt(i));
    delay(100);
  }

  lcd.setCursor(0, 1);               

  for (int i = 0; i < message5.length(); i++) {
    lcd.print(message5.charAt(i));
    delay(100);
  }

  lcd.setCursor(0, 2);
  
  for (int i = 0; i < message6.length(); i++) {
    lcd.print(message6.charAt(i));
    delay(100);
  }

  lcd.setCursor(0, 3);
  
  for (int i = 0; i < message7.length(); i++) {
    lcd.print(message7.charAt(i));
    delay(100);
  }

  lcd.setCursor(14, 0);
  
  for (int i = 0; i < message8.length(); i++) {
    lcd.print(message8.charAt(i));
    delay(100);
  }

  delay(2000);
   // Wait for 2 seconds before clearing the display
  lcd.clear();
}
