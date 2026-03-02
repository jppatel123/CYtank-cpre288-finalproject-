CYtank - Combat Robotic System

Project Overview

CYtank is a tank-style combat robot built using the CyBot. The robot manually navigates a field, detects obstacles, and interacts with a target. Upon reaching the enemy base (target), it triggers a shooting simulation (LED blink and sound). Additionally, it features bump detection, where a second LED blinks when a collision occurs.

Key Features:

Tank-Like Navigation: Uses IR sensors, ultrasonic PING sensors, and an IMU for movement and obstacle avoidance.

Target Interaction: When the robot reaches the target, it simulates a shooting action with a LED blink and plays a shooting sound.

Bump Detection: A second LED blinks when the robot detects a collision, providing real-time feedback.

Real-Time Control: Controlled through a Python GUI, with movement commands and a 180-degree radar scan.

Purpose:

The goal of CYtank was to convert the CyBot into a tank-style robot with autonomous navigation and target interaction. When the robot reaches the enemy base, it simulates a shooting action. Additionally, bump detection alerts the user with a blinking LED when the robot collides with obstacles.

How It Was Made
Hardware:

CyBot Platform (iRobot Create 2, Tiva™ C Launchpad, Arduino, IMU, sensors, servos).

Arduino controls the LEDs for shooting and bump detection, as well as the sound output.

I2C Communication connects the Tiva™ C microcontroller and Arduino for seamless control.

Software:

Python GUI developed with PyQt5 for real-time control and monitoring.

Arduino handles the LED blink (shooting and bump detection) and sound output for the shooting action.

How It Works:

Movement and Navigation: The robot uses sensors to detect obstacles and navigate the field.

Target Detection: When the robot reaches the target, it triggers a LED blink and plays a shooting sound.

Bump Detection: If the robot hits an obstacle, a second LED blinks as a visual alert.

Communication: The Tiva™ C and Arduino communicate via I2C to control the LEDs and sound effects.

Technologies Used:

I2C Communication

UART Communication

PyQt5 for GUI

IMU Integration
![IMG_1210](https://github.com/user-attachments/assets/52f628b2-168f-47f8-b893-5620de9bff5e)
![image](https://github.com/user-attachments/assets/6ad1e9f4-c765-4448-8647-326a7f564601)
![IMG_4037](https://github.com/user-attachments/assets/aea91c6b-88b4-4765-97b8-131598275282)

