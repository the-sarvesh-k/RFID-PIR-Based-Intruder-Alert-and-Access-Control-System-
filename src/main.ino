#include <SPI.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define SS_PIN 10
#define RST_PIN 9
#define PIR_PIN 7
#define BUZZER 6
#define GREEN_LED 4
#define RED_LED 5

#define BT_RX 2
#define BT_TX 3

MFRC522 mfrc522(SS_PIN, RST_PIN);
SoftwareSerial BT(BT_RX, BT_TX);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// -------- STATES --------
bool armed = true;
bool authorized = false;
bool alarmActive = false;
bool waitingForRFID = false;
bool rfidBlocked = false;
bool systemOff = false;

unsigned long motionTime = 0;
unsigned long alertTime = 0;
unsigned long pirHighStart = 0;
unsigned long blinkTime = 0;

bool pirStableDetected = false;
bool redLedState = false;

const unsigned long gracePeriod = 10000;   // 10 sec to scan RFID
const unsigned long alertInterval = 5000;  // 5 sec recheck
const unsigned long pirConfirmTime = 2000; // 2 sec stable motion
const unsigned long blinkInterval = 300;   // red LED blink speed

// -------- YOUR RFID UID --------
byte authorizedUID[4] = {0x72, 0x38, 0xC0, 0x01};

// ---------------- SETUP ----------------
void setup() {
  Serial.begin(9600);
  BT.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  pinMode(PIR_PIN, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  lcd.init();
  lcd.backlight();

  showLCD("PIR Calibrating", "Wait 30 sec");
  delay(30000);

  resetSystem();
}

// ---------------- LOOP ----------------
void loop() {
  handleBluetooth();

  if (systemOff) return;   // STOP everything if system is OFF

  checkRFID();
  checkMotionLogic();
  handleBlinkingRedLED();
}

// ---------------- RESET FUNCTION ----------------
void resetSystem() {
  systemOff = false;
  armed = true;
  authorized = false;
  alarmActive = false;
  waitingForRFID = false;
  rfidBlocked = false;
  pirHighStart = 0;
  pirStableDetected = false;
  redLedState = false;

  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(BUZZER, LOW);

  showLCD("System Reset", "Monitoring...");
  BT.println("System Reset -> Monitoring");
}

// ---------------- SYSTEM OFF ----------------
void turnSystemOff() {
  systemOff = true;
  armed = false;
  authorized = false;
  alarmActive = false;
  waitingForRFID = false;
  rfidBlocked = false;
  pirHighStart = 0;
  pirStableDetected = false;

  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(BUZZER, LOW);

  showLCD("SYSTEM OFF", "Paused");
  BT.println("System OFF");
}

// ---------------- ADMIN OVERRIDE ----------------
void grantAccessOverride() {
  systemOff = false;
  authorized = true;
  armed = false;
  alarmActive = false;
  waitingForRFID = false;

  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(RED_LED, LOW);
  digitalWrite(BUZZER, LOW);

  showLCD("Admin Override", "Access Granted");
  BT.println("Access Granted by Override");
  beepSuccess();
}

// ---------------- RFID ----------------
void checkRFID() {
  if (rfidBlocked) return;   // RFID blocked by Bluetooth

  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  if (isAuthorized()) {
    authorized = true;
    armed = false;
    alarmActive = false;
    waitingForRFID = false;

    showLCD("Access Granted", "System Safe");
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    digitalWrite(BUZZER, LOW);

    BT.println("Authorized Access");
    beepSuccess();
  } else {
    showLCD("Invalid RFID", "Denied");
    BT.println("Invalid RFID");
    beepError();
  }

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

// ---------------- UID CHECK ----------------
bool isAuthorized() {
  for (byte i = 0; i < 4; i++) {
    if (mfrc522.uid.uidByte[i] != authorizedUID[i]) return false;
  }
  return true;
}

// ---------------- MOTION LOGIC ----------------
void checkMotionLogic() {
  int motion = digitalRead(PIR_PIN);

  // -------- STABLE MOTION CONFIRMATION --------
  if (motion == HIGH) {
    if (pirHighStart == 0) {
      pirHighStart = millis();
    }

    if ((millis() - pirHighStart >= pirConfirmTime) && !pirStableDetected) {
      pirStableDetected = true;

      if (armed && !waitingForRFID && !alarmActive) {
        waitingForRFID = true;
        motionTime = millis();

        if (rfidBlocked) {
          showLCD("Motion Detected", "RFID BLOCKED");
          BT.println("Motion detected - RFID blocked");
        } else {
          showLCD("Motion Detected", "Scan RFID (10s)");
          BT.println("Motion detected - waiting 10s");
        }
      }
    }
  } else {
    pirHighStart = 0;
    pirStableDetected = false;
  }

  // -------- WAITING FOR RFID --------
  if (waitingForRFID) {
    if (millis() - motionTime >= gracePeriod) {
      waitingForRFID = false;
      alarmActive = true;
      alertTime = millis();

      showLCD("THEFT ALERT!", "Unauthorized!");
      BT.println("THEFT ALERT!");
    }
  }

  // -------- ALARM ACTIVE --------
  if (alarmActive) {
    triggerAlarm();

    if (millis() - alertTime >= alertInterval) {
      alertTime = millis();

      if (digitalRead(PIR_PIN) == LOW) {
        alarmActive = false;
        digitalWrite(BUZZER, LOW);
        digitalWrite(RED_LED, HIGH);
        digitalWrite(GREEN_LED, LOW);

        showLCD("Safe Now", "No Motion");
        BT.println("Safe - No Motion");
      } else {
        showLCD("THEFT ALERT!", "Still Motion!");
        BT.println("Still Intruder!");
      }
    }
  }
}

// ---------------- RED LED BLINKING ----------------
void handleBlinkingRedLED() {
  if (alarmActive) {
    if (millis() - blinkTime >= blinkInterval) {
      blinkTime = millis();
      redLedState = !redLedState;
      digitalWrite(RED_LED, redLedState);
    }
  }
}

// ---------------- BLUETOOTH ----------------
void handleBluetooth() {
  if (BT.available()) {
    char c = BT.read();

    if (c == 'R') {
      resetSystem();
    }

    else if (c == 'B') {
      rfidBlocked = true;
      authorized = false;
      armed = true;

      showLCD("RFID BLOCKED", "Lockdown Mode");
      digitalWrite(RED_LED, HIGH);
      digitalWrite(GREEN_LED, LOW);

      BT.println("RFID ACCESS BLOCKED");
      beepError();
    }

    else if (c == 'L') {
      armed = true;
      authorized = false;
      alarmActive = false;
      waitingForRFID = false;

      showLCD("System Locked", "Armed Mode");
      digitalWrite(RED_LED, HIGH);
      digitalWrite(GREEN_LED, LOW);
      digitalWrite(BUZZER, LOW);
    }

    else if (c == 'U') {
      armed = false;
      authorized = true;
      alarmActive = false;
      waitingForRFID = false;

      showLCD("System Unlocked", "Safe Mode");
      digitalWrite(GREEN_LED, HIGH);
      digitalWrite(RED_LED, LOW);
      digitalWrite(BUZZER, LOW);
    }

    else if (c == 'O') {
      turnSystemOff();
    }

    else if (c == 'X') {
      grantAccessOverride();
    }
  }
}

// ---------------- ALARM ----------------
void triggerAlarm() {
  digitalWrite(GREEN_LED, LOW);

  digitalWrite(BUZZER, HIGH);
  delay(200);
  digitalWrite(BUZZER, LOW);
  delay(200);
}

// ---------------- BUZZER ----------------
void beepSuccess() {
  tone(BUZZER, 1500, 200);
  delay(300);
}

void beepError() {
  tone(BUZZER, 500, 400);
  delay(500);
}

// ---------------- LCD ----------------
void showLCD(String l1, String l2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(l1);
  lcd.setCursor(0, 1);
  lcd.print(l2);
}
