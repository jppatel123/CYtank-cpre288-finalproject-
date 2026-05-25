# CyTank — Bluetooth Combat Robot

A combat robot system built for CPRE 288 at Iowa State University. Bare-metal C firmware on a Tiva C microcontroller, with a Python PyQt5 GUI for wireless control and live sensor visualization.

---

## What it does

- **Bluetooth manual control** — W/A/S/D keyboard inputs in the GUI send drive commands to the robot wirelessly
- **180° radar scan** — servo sweeps IR sensor across 180°, sending distance/angle data back to the GUI in real time
- **Laser firing** — GUI shoot command triggers the Tiva C to send an I2C signal to an Arduino, which fires a KY-008 laser module
- **Live radar display** — detected objects plotted with distance and angle on a polar radar in the GUI
- **IMU position tracking** — robot's position and heading shown on a test field map in the GUI

---

## System overview

```
Python PyQt5 GUI (Laptop)
        |
        |  Bluetooth (UART)
        |  commands: "w", "a", "s", "d", "scan", "shoot"
        v
Tiva C TM4C123G LaunchPad
        |               |
        | UART           | I2C
        v               v
  iRobot Create 2    Arduino Uno
  (drive motors)     (KY-008 laser)
```

The Tiva C handles everything in the middle — it receives commands over Bluetooth, drives the robot via UART, fires the laser via I2C, sweeps the servo via PWM, reads sensors, and streams data back to the GUI.

---

## Hardware

| Component | Purpose |
|---|---|
| Tiva C TM4C123G | Main microcontroller |
| iRobot Create 2 | Robot base / drive motors |
| Arduino Uno + KY-008 | Laser firing |
| IR Sensor + HC-SR04 | Radar distance sensing |
| Servo Motor | 180° radar sweep |
| HC-05 Bluetooth Module | Wireless UART link |
| IMU | Position and heading |

---

## Tech used

| Layer | Details |
|---|---|
| Firmware | Embedded C — GPIO, UART, I2C, PWM, hardware interrupts |
| IDE | Code Composer Studio |
| GUI | Python, PyQt5, pyserial |
| Protocols | UART (Bluetooth), UART (iRobot), I2C (Arduino), PWM (servo) |

---

## Running it

### Firmware
Open the project in Code Composer Studio, build, and flash to the TM4C123G.

### GUI
```bash
pip install PyQt5 pyserial
python cytank_gui.py
```

---

## Author

**Jay Patel**
[LinkedIn](https://linkedin.com/in/jayprakashbhai-patel) · [GitHub](https://github.com/jppatel123) · jayppatel5078@gmail.com
