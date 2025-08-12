#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

// I2C LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address might be 0x3F

// GSM module
SoftwareSerial gsm(10, 11); // RX, TX

// Motor Driver pins
#define IN1 2
#define IN2 3
#define IN3 4
#define IN4 5

// Sensors
#define TRIG_PIN 6
#define ECHO_PIN 7
#define PIEZO_PIN A0
#define BUZZER 8

long duration;
int distance;

void setup() {
  Serial.begin(9600);
  gsm.begin(9600);

  // Motor pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Sensors and buzzer
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(PIEZO_PIN, INPUT);
  pinMode(BUZZER, OUTPUT);

  // LCD init
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(" Accident Robot ");
  delay(2000);
  lcd.clear();
}

void loop() {
  // Measure distance from ultrasonic
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration * 0.034 / 2;

  int piezoValue = analogRead(PIEZO_PIN);

  lcd.setCursor(0, 0);
  lcd.print("D:");
  lcd.print(distance);
  lcd.print("cm ");

  lcd.setCursor(8, 0);
  lcd.print("P:");
  lcd.print(piezoValue);
  lcd.print("  ");

  // Obstacle condition
  if (distance <= 20) {
    stopMotors();
    lcd.setCursor(0, 1);
    lcd.print("Obstacle Detected ");
    digitalWrite(BUZZER, HIGH);
    delay(1000);
    digitalWrite(BUZZER, LOW);
  }
  // Accident condition
  else if (piezoValue >= 500) {
    stopMotors();
    lcd.setCursor(0, 1);
    lcd.print("Accident! Alert! ");
    digitalWrite(BUZZER, HIGH);
    sendSMS();
    delay(5000);
    digitalWrite(BUZZER, LOW);
  }
  // Safe condition
  else {
    moveForward();
    lcd.setCursor(0, 1);
    lcd.print("Moving Forward   ");
  }

  delay(300);
}

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void moveForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void sendSMS() {
  gsm.println("AT+CMGF=1"); 
  delay(1000);
  gsm.println("AT+CMGS=\"+919916126763\""); // Replace with real number
  delay(1000);
  gsm.println("Accident Detected by Robot!");
  gsm.println("Location: https://www.google.com/maps?q=17.329731,76.834819");
  gsm.write(26); // Ctrl+Z to send
  delay(3000);
}
