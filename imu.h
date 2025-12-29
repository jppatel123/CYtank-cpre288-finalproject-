// IMU.h
#ifndef IMU_H_
#define IMU_H_

#include <stdint.h>

void IMU_init(void);


void IMU_calibrate(void);


float IMU_get_heading_deg(void);

#endif /* IMU_H_ */
