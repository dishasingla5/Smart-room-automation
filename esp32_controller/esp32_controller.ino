#include <WiFi.h>
#include <ThingSpeak.h>
#include <SPI.h>
#include <MFRC522.h>

/* ================= PIN DEFINITIONS ================= */

#define SS_PIN 5
#define RST_PIN 22

#define LDR_PIN 4

#define RGB_R 25
#define RGB_G 26
#define RGB_B 27

#define ROOM_LIGHT 33

/* ================= WIFI + THINGSPEAK ================= */

const char* ssid = "YOUR_WIFI";
const char* password = "YOUR_PASS";

unsigned long channelID = YOUR_CHANNEL_ID;
const char* writeAPIKey = "YOUR_WRITE_KEY";

WiFiClient client;

/* ================= RFID ================= */

MFRC522 mfrc522(SS_PIN, RST_PIN);
byte authorizedCard[4] = {0x23, 0xF6, 0xF5, 0x04};

/* ================= SERIAL TO ARDUINO ================= */

HardwareSerial mySerial(2);  // TX2 = GPIO17

/* ================= STATES ================= */

bool roomActive = false;
bool isDay = false;
bool lastLDRState = false;
bool wifiConnected = false;

unsigned long lastUpload = 0;
unsigned long lastWiFiCheck = 0;

/* ================= PWM CHANNELS ================= */

#define PWM_FREQ 5000
#define PWM_RES 8

#define CH_R 0
#define CH_G 1
#define CH_B 2

/* ==================================================== */

void setup() {

  Serial.begin(115200);
  delay(500);

  mySerial.begin(9600, SERIAL_8N1, -1, 17);

  pinMode(LDR_PIN, INPUT);
  pinMode(ROOM_LIGHT, OUTPUT);

  // PWM setup for RGB
  ledcSetup(CH_R, PWM_FREQ, PWM_RES);
  ledcSetup(CH_G, PWM_FREQ, PWM_RES);
  ledcSetup(CH_B, PWM_FREQ, PWM_RES);

  ledcAttachPin(RGB_R, CH_R);
  ledcAttachPin(RGB_G, CH_G);
  ledcAttachPin(RGB_B, CH_B);

  SPI.begin(18, 19, 23, 5);
  mfrc522.PCD_Init();

  connectWiFi();
  ThingSpeak.begin(client);

  Serial.println("System Ready.");
}

/* ================= LOOP ================= */

void loop() {

  checkWiFi();
  handleRFID();
  handleLDR();
  handleThingSpeak();
}

/* ================= WIFI ================= */

void connectWiFi() {

  Serial.println("Connecting to WiFi...");
  setRGB(255, 150, 0);  // Yellow

  WiFi.begin(ssid, password);

  unsigned long startAttempt = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 10000) {
    delay(300);
    Serial.print(".");
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi Connected!");
    Serial.println(WiFi.localIP());
    setRGB(0, 255, 0);
    wifiConnected = true;
    mySerial.println("WIFI_OK");
  } else {
    Serial.println("WiFi Failed.");
    setRGB(255, 0, 0);
    wifiConnected = false;
    mySerial.println("WIFI_FAIL");
  }
}

void checkWiFi() {

  if (millis() - lastWiFiCheck < 5000) return;
  lastWiFiCheck = millis();

  if (WiFi.status() != WL_CONNECTED) {
    wifiConnected = false;
    mySerial.println("WIFI_LOST");
    connectWiFi();
  }
}

/* ================= RFID ================= */

void handleRFID() {

  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  if (checkUID()) {

    roomActive = !roomActive;

    mySerial.println(roomActive ? "ROOM_ON" : "ROOM_OFF");
    Serial.println(roomActive ? "ROOM ON" : "ROOM OFF");

  } else {
    mySerial.println("ACCESS_DENIED");
    Serial.println("ACCESS DENIED");
  }

  mfrc522.PICC_HaltA();
}

bool checkUID() {

  for (byte i = 0; i < 4; i++) {
    if (mfrc522.uid.uidByte[i] != authorizedCard[i])
      return false;
  }
  return true;
}

/* ================= LDR ================= */

void handleLDR() {

  bool currentState = digitalRead(LDR_PIN);

  if (currentState != lastLDRState) {

    delay(50); // small stabilization
    currentState = digitalRead(LDR_PIN);

    if (currentState == LOW) {   // Bright
      isDay = true;
      digitalWrite(ROOM_LIGHT, LOW);
      setRGB(0, 255, 0);
      mySerial.println("DAY");
      Serial.println("DAY MODE");
    } 
    else {                       // Dark
      isDay = false;
      digitalWrite(ROOM_LIGHT, HIGH);
      setRGB(0, 0, 255);
      mySerial.println("NIGHT");
      Serial.println("NIGHT MODE");
    }

    lastLDRState = currentState;
  }
}

/* ================= THINGSPEAK ================= */

void handleThingSpeak() {

  if (!wifiConnected) return;

  if (millis() - lastUpload >= 15000) {

    ThingSpeak.setField(1, roomActive ? 1 : 0);
    ThingSpeak.setField(2, isDay ? 1 : 0);
    ThingSpeak.setField(3, digitalRead(LDR_PIN));

    int status = ThingSpeak.writeFields(channelID, writeAPIKey);

    Serial.println(status == 200 ? "ThingSpeak Updated." : "ThingSpeak Failed.");

    lastUpload = millis();
  }
}

/* ================= RGB ================= */

void setRGB(int r, int g, int b) {
  ledcWrite(CH_R, r);
  ledcWrite(CH_G, g);
  ledcWrite(CH_B, b);
}
