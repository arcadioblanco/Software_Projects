/*
 * global.c
 *
 *  Created on: Mar 30, 2022
 *      Author: Tomasz Płóciennik
 */
#include "global.h"

int CANTimeoutTimer;
int workingMode = 0;
uint32_t settedPosition = 0;
float settedSpeed = 0;
uint32_t actualPosition = 0;
float actualSpeed = 0;
uint8_t eepromModified = 0;
uint8_t initialized = 0;
settings_str settings;
int8_t settedEffort = 0;
int8_t actualEffort = 0;
uint8_t encoderConnected = 0;
uint32_t lastEncoderPosition = 0;
uint8_t globalCANID = 0x0F;
int32_t encoderPositionForHolding=0;
uint8_t offsetRequestReceived = 0;

uint8_t test1 = 0;
uint8_t test2 = 0;


int32_t degreesToPosition(int16_t degrees){
	return ((int64_t)(degrees + 18000) * 262144 / 36000) - 131072;
}

int16_t positionToDegrees(uint32_t position){
	// 1. Ogranicz wejście do 18 bitów (0-262143)
//	if (position > 262143) position = 262143;

	return (int16_t)(((int64_t)position * 36000) / 262144 - 18000);
}
