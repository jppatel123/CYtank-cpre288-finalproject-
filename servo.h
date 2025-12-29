

#ifndef SERVO_H_
#define SERVO_H_

#include <stdint.h>

// Initializes the servo motor using Timer1B in PWM mode
void servo_init(void);

// Moves the servo to the specified angle (0–180 degrees)
// Returns the match value used
int servo_move(int degrees);

#endif /* SERVO_H_ */
