
#include <movement_1.h>
#include "open_interface.h"
#include <stdio.h>

#define TURN_CALIBRATION_FACTOR 0.79
#define DISTANCE_CALIBRATION_FACTOR 0.95

void move_forward(oi_t *sensor,int centimeters) {
    int sum = 0;
    int target = (int)(centimeters*DISTANCE_CALIBRATION_FACTOR*10);
       oi_update(sensor);
    oi_setWheels(100, 100);
         while (sum<target) {
        oi_update(sensor);
        sum += sensor->distance;
    }
   oi_setWheels(0, 0);
}
int move_forward_with_bump(oi_t *sensor,int centimeters) {
    int sum = 0;
    int target=(int)(centimeters*DISTANCE_CALIBRATION_FACTOR*10);

    oi_update(sensor);
    oi_setWheels(100, 100);

    while (sum<target) {
        oi_update(sensor);
 if (sensor->bumpLeft || sensor->bumpRight) {
            oi_setWheels(0, 0);
            return sum / 10;
        }
sum += sensor->distance;
    }
oi_setWheels(0, 0);
    return sum / 10;
}
void move_backward(oi_t *sensor, int centimeters) {
    int sum = 0;
    int target = (int)(centimeters*DISTANCE_CALIBRATION_FACTOR * 10);

    oi_update(sensor);
    oi_setWheels(-100,-100);

    while (sum<target) {
        oi_update(sensor);
        sum += abs(sensor->distance);
    }
 oi_setWheels(0, 0);
}

void turn_clockwise(oi_t *sensor, int degrees) {
    int sum = 0;
    int target = -1 * (int)(degrees * TURN_CALIBRATION_FACTOR);
 oi_setWheels(-100, 100);
 while (sum > target) {
        oi_update(sensor);
        sum += sensor->angle;
    }
oi_setWheels(0, 0);
}

void turn_counterclockwise(oi_t *sensor, int degrees) {
    int sum = 0;
    int target = (int)(degrees * TURN_CALIBRATION_FACTOR);
    oi_setWheels(100, -100);
     while (sum < target) {
        oi_update(sensor);
        sum += sensor->angle;
    }

    oi_setWheels(0, 0);
}
