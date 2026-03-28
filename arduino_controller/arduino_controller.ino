#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

SoftwareSerial espSerial(2, 3);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pins
#define AC_LED 4
#define GEYSER_LED A1
#define BUTTON 6
#define SOLAR A0

#define SERVO1 9
#define SERVO2 8

#define CURTAIN_R 10
#define CURTAIN_G 11
#define CURTAIN_B 12

Servo servo1;
Servo servo2;

// States
bool roomActive = false;
bool curtainOpen = false;
bool isDay = false;

bool overrideScreen = false;
unsigned long overrideStart = 0;

int screen = 0;

// Button debounce
bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

// Timing
unsigned long lastDisplayUpdate = 0;

void setup() {
  espSerial.begin(9600);

  pinMode(AC_LED, OUTPUT);
  pinMode(GEYSER_LED, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);

  pinMode(CURTAIN_R, OUTPUT);
  pinMode(CURTAIN_G, OUTPUT);
  pinMode(CURTAIN_B, OUTPUT);

  servo1.attach(SERVO1);
  servo2.attach(SERVO2);

  lcd.init();
  lcd.backlight();
  lcd.print("System Ready");
  delay(1500);
  lcd.clear();
}

void loop() {
  receiveESP();
  handleButton();

  // Update display every 300ms (non-blocking)
  if (millis() - lastDisplayUpdate > 300) {
    displayScreen();
    lastDisplayUpdate = millis();
  }
}

/* RECEIVE FROM ESP32 */
void receiveESP() {
  if (!espSerial.available()) return;

  String msg = espSerial.readStringUntil('\n');
  msg.trim();

  overrideScreen = true;
  overrideStart = millis();

  if (msg == "ROOM_ON") {
    roomActive = true;
    digitalWrite(AC_LED, HIGH);
    digitalWrite(GEYSER_LED, HIGH);
    showMessage("ROOM ACTIVE");
  }

  else if (msg == "ROOM_OFF") {
    roomActive = false;
    digitalWrite(AC_LED, LOW);
    digitalWrite(GEYSER_LED, LOW);
    closeCurtains();
    showMessage("ROOM OFF");
  }

  else if (msg == "DAY") {
    isDay = true;
    openCurtains();
    showMessage("DAY MODE");
  }

  else if (msg == "NIGHT") {
    isDay = false;
    closeCurtains();
    showMessage("NIGHT MODE");
  }
}

/* BUTTON (Debounced) */
void handleButton() {
  bool reading = digitalRead(BUTTON);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading == LOW && lastButtonState == HIGH) {
      screen++;
      if (screen > 3) screen = 0;
      lcd.clear();
    }
  }

  lastButtonState = reading;
}

/* DISPLAY */
void displayScreen() {

  if (overrideScreen) {
    if (millis() - overrideStart > 3000) {
      overrideScreen = false;
      lcd.clear();
    } else return;
  }

  lcd.setCursor(0, 0);

  if (screen == 0) {
    lcd.print("ROOM STATUS   ");
    lcd.setCursor(0, 1);
    lcd.print(roomActive ? "ACTIVE " : "OFF    ");
  }

  else if (screen == 1) {
    lcd.print("CURTAIN       ");
    lcd.setCursor(0, 1);
    lcd.print(curtainOpen ? "OPEN   " : "CLOSED ");
  }

  else if (screen == 2) {
    float v = analogRead(SOLAR) * (5.0 / 1023.0);
    lcd.print("SOLAR:");
    lcd.print(v, 2);
    lcd.print("V   ");
  }

  else if (screen == 3) {
    lcd.print("LIGHT MODE    ");
    lcd.setCursor(0, 1);
    lcd.print(isDay ? "DAY   " : "NIGHT ");
  }
}

/* CURTAINS */

void openCurtains() {
  digitalWrite(CURTAIN_R, HIGH);

  servo1.write(90);
  servo2.write(90);

  delay(500);  // minimal safe delay

  digitalWrite(CURTAIN_R, LOW);
  digitalWrite(CURTAIN_G, HIGH);
  digitalWrite(CURTAIN_B, LOW);

  curtainOpen = true;
}

void closeCurtains() {
  digitalWrite(CURTAIN_R, HIGH);

  servo1.write(0);
  servo2.write(180);

  delay(500);

  digitalWrite(CURTAIN_R, LOW);
  digitalWrite(CURTAIN_G, LOW);
  digitalWrite(CURTAIN_B, HIGH);

  curtainOpen = false;
}

/* LCD MESSAGE HELPER */
void showMessage(String msg) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(msg);
}
