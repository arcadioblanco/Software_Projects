/*
 * global.c
 *
 *  Created on: Mar 30, 2022
 *      Author: Tomasz Płóciennik
 */
#include "global.h"

int CANTimeoutTimer;
int workingMode = 0;
uint8_t eepromModified = 0;
uint8_t initialized = 0;
settings_str settings;
int8_t settedEffort = 0;
uint8_t globalCANID = 0x0F;
uint8_t NPK_EN = 0;


