// IMU.c
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "Timer.h"
#include "lcd.h"
#include "IMU.h"

#define BNO055_ADDR        0x29   // 7-bit address with ADR high
#define BNO055_ADDR_WRITE  (BNO055_ADDR << 1)
#define BNO055_ADDR_READ   ((BNO055_ADDR << 1) | 1)

// ---- I2C low-level helpers ----

static void I2C1_wait_while_busy(void) {
    while (I2C1_MCS_R & 0x01) {
        // wait until bus is not busy
    }
}

static void I2C1_write_reg(uint8_t reg, uint8_t value) {
    I2C1_wait_while_busy();
    I2C1_MSA_R = BNO055_ADDR_WRITE; // write
    I2C1_MDR_R = reg;
    I2C1_MCS_R = 0x03;              // START + RUN
    I2C1_wait_while_busy();

    I2C1_MDR_R = value;
    I2C1_MCS_R = 0x05;              // STOP + RUN
    I2C1_wait_while_busy();
}

static uint8_t I2C1_read_reg(uint8_t reg) {
    uint8_t data;

    // write register address
    I2C1_wait_while_busy();
    I2C1_MSA_R = BNO055_ADDR_WRITE;
    I2C1_MDR_R = reg;
    I2C1_MCS_R = 0x03;              // START + RUN
    I2C1_wait_while_busy();

    // restart as read
    I2C1_MSA_R = BNO055_ADDR_READ;
    I2C1_MCS_R = 0x07;              // START + RUN + STOP (single byte)
    I2C1_wait_while_busy();

    data = I2C1_MDR_R;
    return data;
}

// ---- IMU public functions ----

void IMU_init(void) {
    // Enable clock for Port A (I2C1 pins) and I2C1 module
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;  // Port A
    while ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R0) == 0) {}

    SYSCTL_RCGCI2C_R |= SYSCTL_RCGCI2C_R1;    // I2C1
    while ((SYSCTL_PRI2C_R & SYSCTL_PRI2C_R1) == 0) {}

    // Configure PA6 (I2C1 SCL) and PA7 (I2C1 SDA)
    GPIO_PORTA_AFSEL_R |= (1 << 6) | (1 << 7);
    GPIO_PORTA_DEN_R   |= (1 << 6) | (1 << 7);
    GPIO_PORTA_ODR_R   |= (1 << 7);  // SDA open-drain
    GPIO_PORTA_PCTL_R &= ~0xFF000000;
    GPIO_PORTA_PCTL_R |= (3 << 24) | (3 << 28);  // I2C1 on PA6/PA7

    // Configure I2C1 as master, 100 kHz
    I2C1_MCR_R = 0x10;     // Master mode
    I2C1_MTPR_R = 0x07;    // 100 kHz @ 16 MHz

    // Wait for sensor boot
    timer_waitMillis(700);

    // Put BNO055 into CONFIG mode first (required)
    I2C1_write_reg(0x3D, 0x00);
    timer_waitMillis(30);

    // Use register page 0
    I2C1_write_reg(0x07, 0x00);

    // Power mode: normal
    I2C1_write_reg(0x3E, 0x00);
    timer_waitMillis(10);

    // Units: degrees for Euler, Celsius, etc. (0x3B default = 0x00)
    I2C1_write_reg(0x3B, 0x00);

    // Finally, set operation mode: NDOF (full fusion accel+gyro+mag)
    I2C1_write_reg(0x3D, 0x0C);
    timer_waitMillis(30);
}

void IMU_calibrate(void) {
    uint8_t calib;

    lcd_printf("Move robot\nfor IMU calib");

    do {
        calib = I2C1_read_reg(0x35); // CALIB_STAT

        uint8_t sys   = (calib >> 6) & 0x03;
        uint8_t gyro  = (calib >> 4) & 0x03;
        uint8_t accel = (calib >> 2) & 0x03;
        uint8_t mag   = (calib >> 0) & 0x03;

        lcd_printf("Calibrating:\nSYS:%u G:%u\nA:%u M:%u", sys, gyro, accel, mag);
        timer_waitMillis(200);

        // loop until all = 3
    } while ( ((calib & 0xC0) != 0xC0) ||   // SYS
              ((calib & 0x30) != 0x30) ||   // GYRO
              ((calib & 0x0C) != 0x0C) ||   // ACCEL
              ((calib & 0x03) != 0x03) );   // MAG

    lcd_printf("IMU Calibrated!");
}


float IMU_get_heading_deg(void) {
    uint8_t lsb = I2C1_read_reg(0x1A);
    uint8_t msb = I2C1_read_reg(0x1B);

    uint16_t raw = ((uint16_t)msb << 8) | lsb;
    return raw / 16.0f;
}
