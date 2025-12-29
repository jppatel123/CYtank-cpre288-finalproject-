

#include <IMU.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <movement_1.h>
#include "Timer.h"
#include "light.h"
#include "cyBot_uart.h"
#include "uart.h"
#include "open_interface.h"
#include "servo.h"
#include "ping.h"
#include "adc.h"
#include "arduinototiva.h"
#include "song.h"
// ============== PARAMETERS ==============
#define MOVE_SPEED           200
#define MOVE_DISTANCE_CM     20    // cm per 'w' or 's' command
#define TURN_DEGREES         15    // degrees per 'a' or 'd' command
#define BACKUP_CM             3    // cm to backup after bump

// ============== DETECTION RESULTS ==============
#define RESULT_OK            0
#define RESULT_LEFT_BUMP     1
#define RESULT_RIGHT_BUMP    2
#define RESULT_BOTH_BUMP     3
#define RESULT_TAPE          4

// ============== OBJECT DETECTION ==============
#define TARGET_WIDTH_CM      6.0
#define DETECTION_THRESHOLD  50

// ============== TAPE THRESHOLDS ==============
#define TAPE_THRESHOLD_HIGH  2700
#define TAPE_THRESHOLD_LOW   600

// ============== OBJECT STRUCTURE ==============
typedef struct {
    double start_angle;
    double end_angle;
    double center_angle;
    double distance;
    double width;
} Object;

#define MAX_OBJECTS 20
static Object objects[MAX_OBJECTS];
static int object_count = 0;

// ============== SEND STRING TO GUI ==============

void send_to_gui(const char *str) {
    while (*str) {
        cyBot_sendByte(*str++);
    }
    timer_waitMillis(10);
}

// ============== IR DISTANCE ==============

float get_ir_distance(uint16_t raw) {
    const float A = 34934.0f;
    const float B = -53.0f;
    const float C = -5.0f;
    
    float x = raw + B;
    if (x < 1) x = 1;
    
    float d = A / x + C;
    if (d < 9) d = 9;
    if (d > 80) d = 80;
    
    return d;
}

// ============== CALCULATE WIDTH ==============

double calculate_width(double distance_cm, double start_angle, double end_angle) {
    double theta = fabs(end_angle - start_angle) * M_PI / 180.0;
    return 2.0 * distance_cm * sin(theta / 2.0);
}

// ============== CHECK TAPE ==============

int check_tape(oi_t *sensor) {
    uint16_t fl = sensor->cliffFrontLeftSignal;
    uint16_t fr = sensor->cliffFrontRightSignal;
    
    if (fl > TAPE_THRESHOLD_HIGH || fr > TAPE_THRESHOLD_HIGH) return 1;
    if (fl < TAPE_THRESHOLD_LOW || fr < TAPE_THRESHOLD_LOW) return 1;
    if (sensor->cliffFrontLeft || sensor->cliffFrontRight) return 1;
    
    return 0;
}

// ============== MOVE FORWARD WITH BUMP AND TAPE DETECTION ==============

int move_forward_with_detection(oi_t *sensor, int centimeters) {
    double sum = 0;
    int target = centimeters * 10;  // Convert to mm
    int result = RESULT_OK;
    
    oi_setWheels(100, 100);
    
    while (sum < target) {
        oi_update(sensor);
        sum += sensor->distance;

        // Check for bumps
        if (sensor->bumpLeft && sensor->bumpRight) {
            send_to_gui("Both Bump \r\n");
            result = RESULT_BOTH_BUMP;
            beep();
            break;
        } else if (sensor->bumpLeft) {
            send_to_gui("Left Bump\r\n");
            result = RESULT_LEFT_BUMP;
            beep();
            break;
        } else if (sensor->bumpRight) {
            result = RESULT_RIGHT_BUMP;
            beep();
            break;
        }

        // Check for tape
        if (check_tape(sensor)) {
            result = RESULT_TAPE;
            tapeDetected();
            break;
        }
    }
    
    oi_setWheels(0, 0);

    // If bump detected, backup a little
    if (result == RESULT_LEFT_BUMP || result == RESULT_RIGHT_BUMP || result == RESULT_BOTH_BUMP) {
        timer_waitMillis(100);
        move_backward(sensor, BACKUP_CM);
        blink();// Use your movement function
    }
    
    return result;
}

// ============== SCAN AND SEND TO GUI ==============

void do_scan_for_gui(void) {
    char msg[100];
    object_count = 0;
    
    int in_object = 0;
    double start_angle = 0;
    double min_dist = 999;
    int gap_count = 0;
    
    int angle;
    for (angle = 0; angle <= 180; angle += 2) {
        
        servo_move(angle);
        timer_waitMillis(50);

        uint16_t raw = adc_read();
        float ir_cm = get_ir_distance(raw);
        
        pulse_ping();
        timer_waitMillis(30);
        int ticks = start_time - stop_time;
        start_time = 0;
        stop_time = 0;
        float ping_cm = ticks * 0.001058;
        if (ping_cm < 0) ping_cm = 0;
        if (ping_cm > 100) ping_cm = 100;
        
        sprintf(msg, "%d %.1f %.1f\r\n", angle, ir_cm, ping_cm);
        send_to_gui(msg);
        
        if (!in_object && ir_cm < DETECTION_THRESHOLD) {
            in_object = 1;
            start_angle = angle;
            min_dist = ir_cm;
            gap_count = 0;
        }
        else if (in_object) {
            if (ir_cm < min_dist) {
                min_dist = ir_cm;
            }
            
            if (ir_cm >= DETECTION_THRESHOLD) {
                gap_count++;
            } else {
                gap_count = 0;
            }
            
            if (gap_count >= 3 || angle == 180) {
                double end_angle = angle - (gap_count * 2);
                double width = calculate_width(min_dist, start_angle, end_angle);
                double center = (start_angle + end_angle) / 2.0;
                
                if (width > 1.0 && object_count < MAX_OBJECTS) {
                    objects[object_count].start_angle = start_angle;
                    objects[object_count].end_angle = end_angle;
                    objects[object_count].center_angle = center;
                    objects[object_count].distance = min_dist;
                    objects[object_count].width = width;
                    object_count++;
                }
                
                in_object = 0;
            }
        }

    }
    
    servo_move(90);
    timer_waitMillis(200);
    
    int i;
    for (i = 0; i < object_count; i++) {
        sprintf(msg, "OBJECT %.0f %.1f %.1f\r\n", 
                objects[i].center_angle, 
                objects[i].distance, 
                objects[i].width);
        send_to_gui(msg);
    }
    
    send_to_gui("END\r\n");
}



int main(void) {
    
    timer_init();
    cyBot_uart_init();
    adc_init();
    servo_init();
    ping_init();
    light_init();
    UART2_Init();
    oi_t *sensor = oi_alloc();
    oi_init(sensor);
    servo_move(90);
    
    lcd_printf("GUI Ready  11");
    
    while (1) {
        
        char cmd = cyBot_getByte_blocking();
        
        if (cmd == 0) continue;
        
        switch (cmd) {
            case 'w':
            case 'W': {
                lcd_printf("Forward");
                
                int result = move_forward_with_detection(sensor, MOVE_DISTANCE_CM);
                
                if (result == RESULT_LEFT_BUMP) {}
                else if (result == RESULT_RIGHT_BUMP) {
                    lcd_printf("RIGHT BUMP!");
                    send_to_gui("RIGHT_BUMP\r\n");
                    timer_waitMillis(500);

                }
                else if (result == RESULT_BOTH_BUMP) {
                    lcd_printf("BOTH BUMP!");
                    send_to_gui("BOTH_BUMP\r\n");
                    timer_waitMillis(500);

                }
                else if (result == RESULT_TAPE) {
                    lcd_printf("TAPE!");
                    send_to_gui("TAPE\r\n");
                    timer_waitMillis(500);
                }
                else {
                    send_to_gui("OK\r\n");
                }
                break;
            }
            case 's':
            case 'S':
                lcd_printf("Backward");
                move_backward(sensor, MOVE_DISTANCE_CM);
                send_to_gui("OK\r\n");
                break;
            
            case 'a':
            case 'A':
                lcd_printf("Turn Left");
                turn_counterclockwise(sensor, TURN_DEGREES);
                send_to_gui("OK\r\n");
                break;
            
            case 'd':
            case 'D':
                lcd_printf("Turn Right");
                turn_clockwise(sensor, TURN_DEGREES);
                send_to_gui("OK\r\n");
                break;
            
            case 'x':
            case 'X':
                lcd_printf("Stop");
                oi_setWheels(0, 0);
                send_to_gui("OK\r\n");
                break;
            
            case 'm':
            case 'M':
                lcd_printf("Scanning...");
                do_scan_for_gui();
                lcd_printf("Scan Done");
                break;

            case 't':
            case 'T':
                UART2_WriteString("t");
                break;

            case 'f':
            case 'F':
                finish();
                break;

            default:
                send_to_gui("OK\r\n");
                break;
        }
        
        timer_waitMillis(150);
    }
}
