# 🤖 CyTank — Bluetooth Combat Robot System

![Language](https://img.shields.io/badge/Language-C-00599C?style=for-the-badge&logo=c&logoColor=white)
![Platform](https://img.shields.io/badge/Platform-Tiva%20C%20TM4C123G-E34F26?style=for-the-badge&logoColor=white)
![GUI](https://img.shields.io/badge/GUI-Python%20PyQt5-3776AB?style=for-the-badge&logo=python&logoColor=white)
![Status](https://img.shields.io/badge/Status-Completed-brightgreen?style=for-the-badge)
![Course](https://img.shields.io/badge/Course-CPRE%20288-1565C0?style=for-the-badge)

A **full-stack autonomous combat robot system** — bare-metal C firmware on a Tiva C microcontroller, Arduino laser control via I2C, and a live Python PyQt5 GUI with radar visualization and Bluetooth control.

---

## 📋 Features

- 🔭 **180° Radar Scanning** — servo-mounted IR sensor sweeps and detects objects with distance + angle
- 🎮 **Bluetooth Manual Control** — W/A/S/D keyboard control over Bluetooth from the GUI
- 🔫 **Laser Firing System** — KY-008 laser triggered from GUI shoot button via I2C
- 📡 **Live Radar Display** — real-time radar visualization in the Python GUI
- 🗺️ **IMU Position Tracking** — tracks robot's position and heading using IMU data
- ⚡ **Interrupt-Driven** — GPIO interrupts for responsive sensor reading and control

---

## 🏗️ System Architecture

```
┌─────────────────────────────────────────────────────────┐
│                    Python PyQt5 GUI                     │
│        (Radar Display · W/A/S/D · Shoot Button)         │
└──────────────────────┬──────────────────────────────────┘
                       │ Bluetooth (UART)
┌──────────────────────▼──────────────────────────────────┐
│              Tiva C TM4C123G LaunchPad                  │
│   UART · GPIO · Interrupts · Servo PWM · ADC · IMU      │
└──────┬───────────────────────────────────┬──────────────┘
       │ I2C                               │ iRobot UART
┌──────▼──────────┐               ┌────────▼──────────────┐
│  Arduino Uno    │               │  iRobot Create 2 Base │
│  KY-008 Laser   │               │  Drive Motors · Sensor│
└─────────────────┘               └───────────────────────┘
```

---

## 🛠️ Built With

<p>
  <img src="https://skillicons.dev/icons?i=c,python,arduino,git,vscode" />
</p>

| Component | Technology |
|---|---|
| Firmware | C (Embedded C, Code Composer Studio) |
| Robot Base | iRobot Create 2 + CyBot Platform |
| Microcontroller | Tiva C Series TM4C123G LaunchPad |
| Laser Control | Arduino Uno + KY-008 Laser Module |
| GUI | Python PyQt5 |
| Communication | UART (Bluetooth), I2C |
| Sensing | IR Sensor, HC-SR04 Ultrasonic, IMU |
| Motion | Servo Motor (180° scan), DC Drive Motors |

---

## 📡 Communication Protocols

| Protocol | Used For |
|---|---|
| **UART** | Tiva C ↔ iRobot base, Bluetooth ↔ GUI |
| **I2C** | Tiva C → Arduino (laser trigger command) |
| **PWM** | Servo motor control for radar scanning |
| **GPIO + Interrupts** | Sensor reads, button inputs |

---

## 🚀 How to Run

### Hardware Required
- CyBot Platform (iRobot Create 2 base)
- Tiva C Series TM4C123G LaunchPad
- Arduino Uno
- KY-008 Laser Module
- IR Sensor + HC-SR04 Ultrasonic Sensor
- Servo Motor
- Bluetooth Module (HC-05 or similar)
- LCD Board

### Firmware (Tiva C)
```bash
# Open in Code Composer Studio
# Import project → Build → Flash to TM4C123G
```

### Python GUI
```bash
# Install dependencies
pip install PyQt5 pyserial

# Run GUI
python cytank_gui.py
```

---

## 📸 Demo

```
╔══════════════════════════════════╗
║        CyTank Control GUI        ║
╠══════════════════════════════════╣
║  [Live Radar Display]            ║
║       Object @ 45° — 23cm       ║
║       Object @ 90° — 41cm       ║
╠══════════════════════════════════╣
║  Position: X=12  Y=8  Heading=N  ║
╠══════════════════════════════════╣
║   [W] Forward   [S] Backward     ║
║   [A] Left      [D] Right        ║
║            [SHOOT 🔫]            ║
╚══════════════════════════════════╝
```

---

## 📁 Project Structure

```
CYtank-cpre288-finalproject/
│
├── firmware/
│   ├── main.c               # Main firmware entry point
│   ├── uart.c / uart.h      # UART communication
│   ├── servo.c / servo.h    # Servo PWM control
│   ├── sensor.c / sensor.h  # IR + Ultrasonic sensor reads
│   ├── i2c.c / i2c.h        # I2C for Arduino laser
│   └── movement.c           # Robot drive control
│
├── arduino/
│   └── laser_control.ino    # Arduino I2C slave for laser
│
├── gui/
│   └── cytank_gui.py        # Python PyQt5 GUI
│
└── README.md
```

---

## 💡 What I Learned

- **Bare-metal firmware** development in C for ARM Cortex-M4
- **UART communication** protocol — sending/receiving structured data packets
- **I2C master/slave** setup between two microcontrollers
- **PWM generation** for precise servo motor angle control
- **Interrupt-driven programming** for real-time sensor response
- **Python GUI development** with PyQt5 and serial communication
- **System integration** — hardware + firmware + software working together

---

## 👤 Author

**Jay Patel** — [@jppatel123](https://github.com/jppatel123)

[![LinkedIn](https://img.shields.io/badge/LinkedIn-0077B5?style=flat&logo=linkedin&logoColor=white)](https://linkedin.com/in/jayprakashbhai-patel)
[![Email](https://img.shields.io/badge/Email-D14836?style=flat&logo=gmail&logoColor=white)](mailto:jayppatel5078@gmail.com)

---

## 🏷️ Topics

`embedded-systems` `robotics` `c` `uart` `i2c` `pwm` `tiva-c` `arduino` `python` `pyqt5` `bluetooth` `arm-cortex-m4` `cpre288`
