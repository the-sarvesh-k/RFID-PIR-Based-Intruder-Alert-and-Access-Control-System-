RFID-PIR Based Intruder Alert and Access Control System ⚠️📱

Overview

This project is an IoT-based security and access control system that combines RFID authentication, PIR motion detection, and Bluetooth communication. The system detects unauthorized access attempts, monitors suspicious movement, and sends real-time alerts to a smartphone using the HC-05 Bluetooth module.

In addition to intrusion detection, the user can remotely manage RFID access permissions through a mobile application, making the system suitable for smart homes, offices, laboratories, and restricted areas.

---

Features

✅ RFID-based authentication

✅ PIR motion-based intruder detection

✅ Smartphone alerts via HC-05 Bluetooth module

✅ Remote blocking of RFID cards

✅ Remote granting/revoking of access

✅ Buzzer alarm and LED status indication

✅ Low-cost and easy-to-build solution

✅ Real-time monitoring and security control

---

Components Used

Component| Quantity
Arduino Uno| 1
RC522 RFID Reader| 1
RFID Tags/Cards| 2+
PIR Motion Sensor| 1
HC-05 Bluetooth Module| 1
Buzzer| 1
LEDs| 2
Breadboard| 1
Jumper Wires| As Required
Power Supply| 1

---

System Architecture

RFID Card
     │
     ▼
RC522 RFID Reader
     │
     ▼
 Arduino Uno
     │
 ┌───┼──────────────┐
 ▼   ▼              ▼
PIR HC-05       Buzzer/LED
Sensor Bluetooth  Alarm
     │
     ▼
Smartphone Alerts

---

Working Principle

1. The user scans an RFID card using the RC522 reader.
2. Arduino verifies whether the RFID tag is authorized.
3. If the card is authorized, access is granted.
4. If an unauthorized card is detected, an alert is generated.
5. The PIR sensor continuously monitors the area for movement.
6. If suspicious motion is detected, the buzzer and LEDs are activated.
7. The HC-05 Bluetooth module sends alert messages to the connected smartphone.
8. The user can remotely:
   - Block RFID cards
   - Grant access
   - Revoke access
   - Monitor system status

---

Applications

- Smart Home Security
- Office Access Control
- Laboratory Security
- Hostel Security Systems
- Restricted Area Monitoring
- Industrial Safety Systems

---

Future Improvements

- Wi-Fi Connectivity (ESP8266/ESP32)
- Cloud Database Integration
- Mobile Application Dashboard
- SMS and Email Notifications
- Face Recognition Authentication
- Event Logging and Analytics

---

Project Images


images/
├── setup.jpg
├── circuit.jpg
└── demo.jpg

---

Author

Sarvesh Kumar

B.Sc. (Hons.) Electronics

IoT and Embedded Systems Enthusiast

---

License

This project is licensed under the MIT License.