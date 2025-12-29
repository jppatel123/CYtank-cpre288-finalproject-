

// movement.h
#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "open_interface.h"

void move_forward(oi_t *sensor, int centimeters);
int move_forward_with_bump(oi_t *sensor, int centimeters);

void turn_clockwise(oi_t *sensor, int degrees);
void turn_counterclockwise(oi_t *sensor, int degrees);
void move_backward(oi_t *sensor, int centimeters);

#endif
