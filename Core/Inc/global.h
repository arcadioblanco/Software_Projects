
/*
 * global.h
 *
 *  Created on: Mar 30, 2022
 *      Author: Tomasz Płóciennik
 */

#ifndef INC_GLOBAL_H_
#define INC_GLOBAL_H_

// definicje ramek błędów
#define STATUS_OK -1
#define STATUS_WRONG_FRAME 0x00
#define STATUS_NEED_MORE_ARGUMENTS 0x01
#define STATUS_CANNOT_USE_ENCODER 0x02
#define STATUS_POSITION_LIMIT 0x03
#define STATUS_CAN_ADDRESS_IS_USED 0x04
#define STATUS_DATA_ERROR 0x05
#define STATUS_TOO_MANY_FRAMES_IN_CAN_QUEUE 0x06

#include "main.h"
#include "pid.h"

extern int CANTimeoutTimer;
extern int workingMode;
extern uint32_t settedPosition;
extern float settedSpeed;
extern uint32_t actualPosition;
extern float actualSpeed;
extern uint8_t initialized;
extern uint8_t eepromModified;
extern uint8_t offsetRequestReceived;

extern uint8_t test1;

extern uint8_t test2;

typedef struct {
  uint8_t ID;
  pid_str anglePid;
  pid_str speedPid;
  uint8_t canFrequency;
  int32_t offset;
  limits_str limits;
} settings_str;

extern settings_str settings;

extern int8_t settedEffort;
extern uint8_t encoderConnected;

extern uint32_t lastEncoderPosition;

extern int8_t actualEffort;

extern uint8_t globalCANID;
extern int32_t encoderPositionForHolding;

int32_t degreesToPosition(int16_t degrees);
int16_t positionToDegrees(uint32_t position);
#endif /* INC_GLOBAL_H_ */
