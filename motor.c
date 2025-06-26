/*
 * motor.c
 *
 *  Created on: Apr 4, 2022
 *      Author: Tomasz Płóciennik
 */

#include "motor.h"


void MOTOR_Start_Forward(double speed) {  // ustawienie kierunku obrotu silnika na przód i ustawienie prędkości
  TIM_PWM_Set(speed);
  HAL_GPIO_WritePin(INA_GPIO_Port, INA_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(INB_GPIO_Port, INB_Pin, GPIO_PIN_SET);
}
void MOTOR_Start_Backward(double speed) {  // ustawienie kierunku obrotu silnika na tył i ustawienie prędkości
  TIM_PWM_Set(speed);
  HAL_GPIO_WritePin(INA_GPIO_Port, INA_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(INB_GPIO_Port, INB_Pin, GPIO_PIN_RESET);
}
void MOTOR_Stop() {  // zatrzymanie silnika
  HAL_GPIO_WritePin(INA_GPIO_Port, INA_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(INB_GPIO_Port, INB_Pin, GPIO_PIN_RESET);
}
