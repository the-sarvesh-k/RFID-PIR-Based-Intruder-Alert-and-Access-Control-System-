# 🔐 RFID-PIR Based Intruder Alert System

---

## 📌 Overview

The **RFID-PIR Based Intruder Alert System** is a smart security system built with Arduino that combines **RFID-based access control**, **PIR motion detection**, a **16x2 I2C LCD display**, and an **HC-05 Bluetooth module** for real-time phone alerts and remote control.

When motion is detected, the system gives a **10-second grace period** to scan an authorized RFID card. If no valid card is scanned in time, a **THEFT ALERT** is triggered — buzzer sounds, red LED blinks, and an instant alert is sent to your phone via Bluetooth. You can monitor and control everything remotely from your smartphone using the **Serial Bluetooth Terminal** app.

---

## ✨ Features

- 🏷️ **RFID Access Control** — Only your authorized card can disarm the system
- 🚶 **PIR Motion Detection** — Detects stable motion with a 2-second confirmation filter
- ⏱️ **10-Second Grace Period** — Time window to scan RFID after motion is detected
- 🔔 **Theft Alert** — Buzzer alarm + blinking red LED + instant Bluetooth alert to phone
- 📺 **I2C LCD Display** — Real-time status shown on 16x2 LCD screen
- 📱 **HC-05 Phone Control** — 6 remote commands from your smartphone
- 🚫 **Block RFID** — Remotely disable RFID access (Lockdown Mode)
- 👑 **Admin Override** — Grant access from phone without RFID
- 🔁 **Auto Recheck** — System rechecks motion every 5 seconds during alarm
- ⚡ **System ON/OFF** — Pause/resume the entire system remotely

---

## 🧰 Components Used

| Component | Details |
|---|---|
| **Arduino Uno/Nano** | Main microcontroller |
| **RFID-RC522 Module** | SPI-based RFID card reader |
| **PIR Sensor (HC-SR501)** | Motion detection |
| **HC-05 Bluetooth Module** | Wireless phone communication (SoftwareSerial) |
| **I2C LCD (16x2)** | Status display (Address: 0x27) |
| **Buzzer** | Alarm and feedback beeps |
| **Green LED** | Access granted / safe indicator |
| **Red LED** | Alert / armed indicator (blinks during alarm) |
| **RFID Card / Tag** | Authorized access credential |

---

## 📐 Pin Configuration

### RFID RC522 → Arduino

| RC522 Pin | Arduino Pin |
|---|---|
| SDA | D10 (`SS_PIN`) |
| RST | D9 (`RST_PIN`) |
| SCK | D13 |
| MOSI | D11 |
| MISO | D12 |
| 3.3V | 3.3V |
| GND | GND |

### HC-05 Bluetooth → Arduino

| HC-05 Pin | Arduino Pin |
|---|---|
| TX | D2 (`BT_RX`) |
| RX | D3 (`BT_TX`) |
| VCC | 5V |
| GND | GND |

> ⚠️ HC-05 RX works at 3.3V logic — use a **voltage divider** (1kΩ + 2kΩ) on the TX → RX line.

### PIR Sensor → Arduino

| PIR Pin | Arduino Pin |
|---|---|
| OUT | D7 (`PIR_PIN`) |
| VCC | 5V |
| GND | GND |

### Other Components

| Component | Arduino Pin |
|---|---|
| Buzzer (+) | D6 |
| Green LED | D4 |
| Red LED | D5 |
| LCD SDA | A4 |
| LCD SCL | A5 |

---

## 📱 Bluetooth Phone Commands

Open **Serial Bluetooth Terminal** app → connect to HC-05 → send these single-character commands:

| Command | Action | LCD Shows |
|---|---|---|
| `R` | **Reset system** — back to monitoring | `System Reset / Monitoring...` |
| `B` | **Block RFID** — lockdown mode, RFID disabled | `RFID BLOCKED / Lockdown Mode` |
| `L` | **Lock / Arm** — arms the system | `System Locked / Armed Mode` |
| `U` | **Unlock / Safe mode** — disarms system | `System Unlocked / Safe Mode` |
| `O` | **Turn system OFF** — pauses everything | `SYSTEM OFF / Paused` |
| `X` | **Admin override** — grant access without RFID | `Admin Override / Access Granted` |

---

## 🔔 Bluetooth Alerts Sent to Your Phone

The system automatically sends these messages to your phone:

```
System Reset -> Monitoring
System OFF
Access Granted by Override
Authorized Access
Invalid RFID
Motion detected - waiting 10s
Motion detected - RFID blocked
THEFT ALERT!
Still Intruder!
Safe - No Motion
RFID ACCESS BLOCKED
```

---

## ⚙️ How the System Works

```
[STARTUP]
 └─ PIR Calibration wait: 30 seconds
 └─ System boots in ARMED mode, Red LED ON

[MOTION DETECTED]
 └─ PIR must stay HIGH for 2 seconds (noise filter)
 └─ LCD: "Motion Detected / Scan RFID (10s)"
 └─ BT Alert: "Motion detected - waiting 10s"

[GRACE PERIOD — 10 SECONDS]
 ├─ Authorized RFID scanned?
 │   ├─ YES → Green LED ON, Safe, BT: "Authorized Access"
 │   └─ NO (timeout) → ALARM TRIGGERED

[ALARM ACTIVE]
 └─ Buzzer beeps, Red LED blinks (300ms interval)
 └─ LCD: "THEFT ALERT! / Unauthorized!"
 └─ BT: "THEFT ALERT!"
 └─ Rechecks every 5 seconds:
     ├─ Motion still present → BT: "Still Intruder!"
     └─ No motion → Alarm off, BT: "Safe - No Motion"

[RFID BLOCKED (via 'B' command)]
 └─ Even if motion detected, RFID scan is ignored
 └─ Only phone commands can control the system
```

---

## ⏱️ Timing Configuration

These values can be adjusted in the code:

| Constant | Value | Purpose |
|---|---|---|
| `gracePeriod` | 10,000 ms | Time to scan RFID after motion |
| `alertInterval` | 5,000 ms | How often alarm rechecks motion |
| `pirConfirmTime` | 2,000 ms | Stable motion filter to avoid false triggers |
| `blinkInterval` | 300 ms | Red LED blink speed during alarm |

---

## 📚 Libraries Required

Install these in Arduino IDE → **Sketch → Include Library → Manage Libraries**:

| Library | Search For |
|---|---|
| `MFRC522` | `MFRC522 by GithubCommunity` |
| `LiquidCrystal_I2C` | `LiquidCrystal I2C by Frank de Brabander` |
| `SoftwareSerial` | Built-in (no install needed) |
| `Wire` | Built-in (no install needed) |
| `SPI` | Built-in (no install needed) |

---

## 🗂️ Project Structure

```
RFID-PIR-Intruder-Alert/
│
├── src/
│   └── main.ino                  # Main Arduino sketch
│
├── circuit/
│   ├── schematic.png             # Circuit diagram
│   └── breadboard_layout.png     # Breadboard layout
│
├── images/
│   ├── project_photo.jpg         # Project build photo
│   └── demo.gif                  # Working demo
│
└── README.md
```

---

## 🚀 Getting Started

1. **Clone this repo:**
   ```bash
   git clone https://github.com/YOUR_USERNAME/RFID-PIR-Intruder-Alert.git
   ```

2. **Install required libraries** in Arduino IDE

3. **Set your RFID UID** in the code:
   ```cpp
   byte authorizedUID[4] = {0x72, 0x38, 0xC0, 0x01};
   ```
   > To find your card's UID, use MFRC522's `DumpInfo` example sketch and check the Serial Monitor.

4. **Upload** `src/main.ino` to your Arduino

5. **Wait 30 seconds** after powering on — PIR calibrates automatically

6. **Pair HC-05** on your phone (PIN: `1234` or `0000`)

7. **Open Serial Bluetooth Terminal** → connect → start sending commands!

---

## 📲 Setting Up Serial Bluetooth Terminal

1. Download **Serial Bluetooth Terminal** → [Play Store](https://play.google.com/store/apps/details?id=de.kai_morich.serial_bluetooth_terminal)
2. Go to **Settings → Bluetooth** on your phone → pair `HC-05`
3. Open the app → tap the **connect icon** → select **HC-05**
4. In the app's macro buttons, you can set up quick-tap shortcuts:
   - Button 1 → `R` (Reset)
   - Button 2 → `B` (Block RFID)
   - Button 3 → `L` (Lock)
   - Button 4 → `U` (Unlock)
   - Button 5 → `O` (OFF)
   - Button 6 → `X` (Admin Override)

---

## 🔮 Future Improvements

- [ ] Add **ESP8266/ESP32** for Wi-Fi alerts (WhatsApp / Telegram / Email)
- [ ] Build a custom **Android App** with dashboard UI
- [ ] Add **OV7670 camera** to capture intruder photo on alert
- [ ] Store access logs on **SD card module**
- [ ] Add **fingerprint sensor** for triple-layer security
- [ ] Add **multiple authorized UIDs** support

---

## 🛡️ License

This project is licensed under the **MIT License** — free to use, modify, and build upon.

---

## 🙋 Author

Sarvesh Kumar
- GitHub: (https://github.com/the-sarvesh-k)
- Project: (https://github.com/the-sarvesh-k/RFID-PIR-Based-Intruder-Alert-and-Access-Control-System-)

---

⭐ **Star this repo** if it helped you — it keeps me motivated to build more IoT projects!

*Built with ❤️ using Arduino, RC522, PIR, HC-05 & I2C LCD*
