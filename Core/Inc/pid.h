/*
 * pid_controller.h
 *
 *  Created on: Sep 09, 2022
 *      Author: Tomasz Płóciennik
 */

#ifndef INC_PID_H_
#define INC_PID_H_

#define PID_POSITION 1
#define PID_SPEED 2

extern int32_t previous_error;  // Poprzedni błąd dla członu różniczkującego
extern double total_error;  // Suma uchybów dla członu całkującego

typedef struct {
  float Kp;  // Wzmocnienie członu proporcjonalnego
  float Ki;  // Wzmocnienie członu całkującego*/
  float Kd;  // Wzmocnienie członu różniczkującego*/
} pid_str;

typedef struct {
  uint8_t min_power_f;
  uint8_t max_power_f;
  uint8_t min_power_b;
  uint8_t max_power_b;
} limits_str;

void PID_setPID(float Kp, float Ki, float Kd, int steeringType);
void PID_reset();
float PID_calculate(int32_t error, int steeringType);
void setPidToAngle();
void setPidToSpeed();

#endif /* INC_PID_H_ */
