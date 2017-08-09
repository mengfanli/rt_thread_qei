/*
 * dev_motor.h
 *
 *  Created on: 2016Äê7ÔÂ22ÈÕ
 *      Author: dbw
 */

#ifndef SRC_DRIVERS_DEV_MOTOR_H_
#define SRC_DRIVERS_DEV_MOTOR_H_

#include <stdint.h>

void rt_dev_motor_init(void);
void rt_dev_motor_set_speed(int left, int right);
void rt_dev_motor_get_location(int32_t *left, int32_t *right);
void rt_dev_motor_get_velocity(int32_t *left, int32_t *right);

#endif /* SRC_DRIVERS_DEV_MOTOR_H_ */
