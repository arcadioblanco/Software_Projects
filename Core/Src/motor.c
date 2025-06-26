#include "motor.h"

float motorMinSpeed = 0.2;
float result = 0;
int trace3 = 0;
int trace3rev = 0;

void MOTOR_Start_Forward(double speed) {  // ustawienie kierunku obrotu silnika na przód i ustawienie prędkości

	if ((speed > 1.0) || (speed < -1.0)) {
		return;
	}
	Motor_SetSpeed((int32_t)(speed * 1800000.0));
}
void MOTOR_Start_Backward(double speed) {  // ustawienie kierunku obrotu silnika na tył i ustawienie prędkości
	return;

}
void MOTOR_Stop() {  // zatrzymanie silnika
   Motor_SetSpeed(0);
}
int16_t MOTOR_Set_Position(uint32_t position) {  // ustawienie pozycji silnika
  if (position > 262143) return STATUS_DATA_ERROR;
  if (workingMode != 1) {  // jeżeli nie jesteśmy w trybie pozycji to zresetuj PID
    setPidToAngle();
    PID_reset();
  }
  settedPosition = position;
  workingMode = 1;  // ustaw tryb pracy na 1 (sterowanie pozycją)
  return STATUS_OK;
}

int16_t MOTOR_Keep_Position() {  // utrzymanie aktualnej pozycji
  if (workingMode != 4) {        // jeżeli nie jesteśmy w trybie utrzymywania pozycji to zresetuj PID
    encoderPositionForHolding = actualPosition;
    setPidToAngle();
    PID_reset();
  }
  settedPosition = encoderPositionForHolding;
  workingMode = 4;  // ustaw tryb pracy na 4 (utrzymywanie pozycji)
  return STATUS_OK;
}

int16_t MOTOR_Set_Speed(float speed) {  // ustawienie prędkości silnika
  if (workingMode != 2) {               // jeżeli nie jesteśmy w trybie prędkości to zresetuj PID
    setPidToSpeed();
    PID_reset();
    lastEncoderPosition = actualPosition;
  }
  settedSpeed = speed;
  workingMode = 2;  // ustaw tryb pracy na 2 (sterowanie prędkością)

  return STATUS_OK;
}
void MOTOR_PID_Tick(int32_t error, int steeringType) {  // funkcja wywoływana co 10ms w celu obliczenia nowej wartości PWM
  result = PID_calculate(-error, steeringType);

  if (result > 1.0) {
    result = 1.0;
  }
  if (result < -1.0) {
    result = -1.0;
  }

  if (result < 0) {
    result = (result * (settings.limits.max_power_b - settings.limits.min_power_b) / 100) - (settings.limits.min_power_b / 100.0);
  } else {
    result = (result * (settings.limits.max_power_f - settings.limits.min_power_f) / 100) + (settings.limits.min_power_f / 100.0);
  }
  actualEffort = result * 100.0;
  if (result > (settings.limits.min_power_f / 100.0) + 0.01) {
    MOTOR_Start_Forward(result);
  } else if (result < (-settings.limits.min_power_b / 100.0) - 0.01) {
	  MOTOR_Start_Forward(-result);
  } else {
    MOTOR_Stop();
  }
}
