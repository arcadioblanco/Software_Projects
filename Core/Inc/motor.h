/*
 * motor.h
 *
 *  Created on: Apr 4, 2022
 *      Author: Tomasz Płóciennik
 */

#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_
#include "main.h"

extern float motorMinSpeed;

void MOTOR_Start_Forward(double speed);
void MOTOR_Start_Backward(double speed);
void MOTOR_Stop();
int16_t MOTOR_Set_Position(uint32_t position);
int16_t MOTOR_Keep_Position();
int16_t MOTOR_Set_Speed(float speed);
void MOTOR_Set_PID(double p, double i, double d);
void MOTOR_PID_Tick(int32_t encoderPosition, int steeringType);

#endif /* INC_MOTOR_H_ */
