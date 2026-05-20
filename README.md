# 🏠 Smart Home Security System using PIC18F4580

> Advanced Embedded Systems capstone project integrating 8 hardware peripherals into a complete industrial-style smart security and access control system using the PIC18F4580 Microcontroller.

![Embedded](https://img.shields.io/badge/Embedded-Systems-blue)
![MCU](https://img.shields.io/badge/Microcontroller-PIC18F4580-green)
![Protocol](https://img.shields.io/badge/Communication-UART-orange)
![Language](https://img.shields.io/badge/Language-Embedded%20C-red)

---

# 📌 Project Overview

This project demonstrates the design and implementation of a Smart Home Security System capable of:

- Secure password-based door access
- Real-time intrusion detection using interrupts
- UART event logging to PC
- Automatic porch lighting using an LDR sensor
- Hardware PWM-based door motor control
- LCD-based user interaction
- Multitasking embedded firmware architecture

The project simulates a real commercial security product used in:

- Smart Homes
- Biometric Door Locks
- Smart Buildings
- Industrial Access Systems
- IoT-based Monitoring Systems

---

# ✨ Major Features

## 🔐 Password-Based Access Control

Uses:

- 4x3 Matrix Keypad
- 16x2 LCD

Features:

- Secure 4-digit password entry
- Password masking using `*`
- Door unlock after authentication
- Auto-lock functionality
- Wrong password warning

---

## 🚨 Intrusion Detection System

Uses external interrupt:

> RB0 / INT0

Simulated using:

- Push Button (PIR Trigger)

Triggers:

- Alarm buzzer
- Forced door lock
- UART emergency log
- LCD warning display

---

## 💻 UART Serial Logging

All system events are transmitted to a PC through UART.

### Example Logs

```text
SMART HOME SECURITY BOOTED
ACCESS GRANTED. Door unlocking.
DOOR AUTO-LOCKED.
WARNING: WRONG PASSWORD ENTERED!
CRITICAL: INTRUSION DETECTED!!
```

Applications:

- IoT monitoring
- Remote logging
- Cloud connectivity
- Smart security analytics

---

## ⏱️ Embedded Multitasking

While waiting for keypad input, the firmware simultaneously:

- Polls the LDR sensor
- Controls porch lighting
- Monitors intrusion interrupt
- Updates LCD
- Handles UART communication

Demonstrates:

- Cooperative multitasking
- Real-time embedded response handling

---

## 🌙 Smart Porch Lighting

Uses:

- LDR sensor
- ADC module

Behavior:

| Environment | Porch Light |
|-------------|-------------|
| Dark | ON |
| Bright | OFF |

---

## ⚙️ PWM Door Motor Control

Uses:

- CCP1 PWM Module
- L293D Motor Driver

Functions:

- Door Open
- Door Close
- Auto-lock after 5 seconds

---

# 🧠 System Architecture

```text
                    SMART HOME SECURITY SYSTEM
               ==================================

                +-----------------------------+
                |       PIC18F4580 MCU        |
                |                             |
[LDR Sensor] -->| RA0 (AN0)                  |
                |                             |
[Keypad Rows] <-| RB1-RB4                    |
[Keypad Cols] ->| RB5-RB7                    |
                |                             |
[PIR Sensor] -->| RB0 / INT0                 |
                |                             |
[Buzzer] <------| RC0                        |
[Red LED] <-----| RC1                        |
[Motor PWM] <---| RC2 / CCP1                 |
[Green LED] <---| RC3                        |
[Porch LED] <---| RC4                        |
                |                             |
[UART TX] <-----| RC6                        |
[UART RX] ----->| RC7                        |
                |                             |
[LCD D0-D7] <--> | PORTD                     |
[LCD RS/EN] <--> | RE0 / RE1                 |
                +-----------------------------+
```

---

# 🔌 Hardware Components

| Component | Purpose |
|------------|----------|
| PIC18F4580 | Main Microcontroller |
| 4x3 Keypad | Password Entry |
| 16x2 LCD | User Interface |
| LDR Sensor | Light Detection |
| L293D Driver | Motor Driver |
| DC Motor | Door Actuator |
| Push Button | PIR Simulation |
| Buzzer | Alarm System |
| LEDs | Status Indication |
| Virtual Terminal | UART Monitoring |

---

# 🔍 Proteus Component Search Keywords

Use these exact names inside Proteus:

| Component | Proteus Keyword |
|------------|------------------|
| PIC18F4580 | `PIC18F4580` |
| Keypad | `KEYPAD-PHONE` |
| LCD | `LM016L` |
| LDR | `LDR` |
| Motor Driver | `L293D` |
| Motor | `MOTOR` |
| UART Terminal | `VIRTUAL TERMINAL` |
| Push Button | `BUTTON` |
| Buzzer | `BUZZER` |
| LEDs | `LED-RED`, `LED-GREEN`, `LED-YELLOW` |

---

# 📍 Pin Configuration Table

| Peripheral | PIC18F4580 Pin | Function |
|-------------|----------------|----------|
| LDR Sensor | RA0 (AN0) | ADC Analog Input |
| Keypad Rows | RB1-RB4 | Row Scanning Outputs |
| Keypad Columns | RB5-RB7 | Column Inputs |
| PIR Sensor | RB0 / INT0 | External Interrupt |
| Alarm Buzzer | RC0 | Audio Alarm |
| Locked LED | RC1 | Red Status LED |
| Door Motor PWM | RC2 / CCP1 | PWM Output |
| Unlocked LED | RC3 | Green Status LED |
| Porch Light LED | RC4 | Automatic Light |
| UART TX | RC6 | Serial Transmit |
| UART RX | RC7 | Serial Receive |
| LCD Data Bus | PORTD | LCD Data |
| LCD RS | RE0 | LCD Register Select |
| LCD EN | RE1 | LCD Enable |

---

# ⚡ Embedded Concepts Used

This project integrates multiple embedded system concepts into one application:

- GPIO Programming
- Matrix Keypad Scanning
- External Interrupts
- ADC (Analog to Digital Conversion)
- PWM Generation
- UART Communication
- LCD Interfacing
- Timer1 Delay Generation
- Cooperative Multitasking
- Embedded State Management

---

# 🔄 System Workflow

```text
Power ON
   |
   v
Initialize Peripherals
   |
   v
Wait for Password Input
   |
   +----> Monitor LDR Continuously
   |
   +----> Monitor PIR Interrupt
   |
   v
Password Entered?
   |
   +---- NO ---> Continue Waiting
   |
   v
Correct Password?
   |
   +---- YES ----------------------------+
   |                                     |
   |                              Unlock Door
   |                                     |
   |                              Green LED ON
   |                                     |
   |                              UART Log
   |                                     |
   |                              Wait 5 Seconds
   |                                     |
   |                              Auto Lock Door
   |
   +---- NO -----------------------------+
                                         |
                                   Wrong Password
                                         |
                                   Buzzer Beeps
                                         |
                                   UART Warning
```

---

# 🚨 Interrupt-Based Emergency Handling

The intrusion detection system uses:

```c
void __interrupt() ISR()
```

This ensures:

- Immediate response
- Highest priority execution
- Instant alarm activation
- Door forced into locked state

Unlike polling systems, interrupts allow:

- Real-time emergency response
- Fast hardware reaction
- Industrial-grade security handling

---

# 🌙 ADC-Based Smart Lighting

The LDR is connected to:

```text
RA0 / AN0
```

ADC continuously reads ambient light levels:

```c
if (light_level < 300)
    PORTCbits.RC4 = 1;
else
    PORTCbits.RC4 = 0;
```

This enables:

- Automatic porch lighting
- Energy efficiency
- Smart home behavior

---

# ⚙️ PWM Door Control

PWM is generated using:

- CCP1 Module
- Timer2

Door states:

| PWM Duty | Door State |
|-----------|-------------|
| 0 | Locked |
| 1023 | Open |

---

# ⏲️ Timer1 Auto-Lock Mechanism

After successful authentication:

- Door opens
- Timer1 waits for 5 seconds
- Door automatically locks again

This simulates:

- Automatic security relocking
- Real-world smart lock systems

---

# 💻 UART Configuration

UART Baud Rate:

```text
9600 bps
```

UART functions:

- Event monitoring
- Debugging
- PC communication
- IoT gateway foundation

---

# 🛡️ Security Features

| Feature | Purpose |
|----------|----------|
| Password Masking | Prevent shoulder surfing |
| Auto Locking | Prevent unattended access |
| Interrupt Alarm | Immediate intrusion response |
| UART Logging | Security monitoring |
| Forced Locking | Emergency protection |

---

# 📂 Project Structure

```text
Smart-Home-Security-System/
│
├── main.c
├── README.md
├── Proteus/
│   └── smart_home_security.pdsprj
│
├── HEX/
│   └── smart_home.hex
│
└── Images/
    ├── circuit.png
    └── simulation.png
```

---

# 🛠️ Compiler & Tools

| Tool | Usage |
|------|--------|
| MPLAB X IDE | Firmware Development |
| XC8 Compiler | C Compilation |
| Proteus | Hardware Simulation |

---

# ▶️ How to Run

## Step 1 — Compile Firmware

- Open project in MPLAB X
- Select XC8 Compiler
- Build project
- Generate `.hex` file

---

## Step 2 — Open Proteus

- Load Proteus project
- Attach generated HEX file to PIC18F4580

---

## Step 3 — Run Simulation

Enter password using keypad.

Observe:

- LCD messages
- UART logs
- Motor operation
- Intrusion response
- Automatic lighting

---

# 📸 Expected Outputs

## ✅ Correct Password

```text
ACCESS GRANTED
DOOR OPEN (5s)
```

---

## ❌ Wrong Password

```text
WRONG PASSWORD
```

Buzzer beeps 3 times.

---

## 🚨 Intrusion Triggered

```text
INTRUDER ALERT!
```

System enters permanent emergency lock state.

---
#  📸  Project Screen Shot
<img width="1920" height="1080" alt="image" src="https://github.com/user-attachments/assets/d4601c34-861f-428a-ab5e-9d4fb9345d85" />

# 📚 Educational Value

This project is extremely valuable for learning:

- Real-time Embedded Systems
- Industrial Firmware Architecture
- Peripheral Integration
- Embedded Security Systems
- Hardware-Software Co-design
- Interrupt-driven programming
- Embedded multitasking

---

# 🧑‍💻 Author

Developed as an advanced embedded systems learning project using:

- PIC18F4580
- XC8 Compiler
- MPLAB X IDE
- Proteus Simulation

---

# ⭐ Future Improvements

Possible future upgrades:

- RFID Authentication
- Fingerprint Sensor
- GSM SMS Alerts
- WiFi/IoT Integration
- Mobile App Control
- EEPROM Password Storage
- Camera-based Surveillance
- Cloud Monitoring Dashboard

---

# 📜 License

This project is open-source and available for educational purposes.

---
