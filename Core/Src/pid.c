/*
 * pid_controller.c
 *
 *  Created on: Sep 09, 2022
 *      Author: Tomasz Płóciennik
 */

#include "main.h"

float p_term, i_term, d_term;
float pid_divider = 1.0;
float Kp = 0;                  // Wzmocnienie członu proporcjonalnego
float Ki = 0;                  // Wzmocnienie członu całkującego*/
float Kd = 0;                  // Wzmocnienie członu różniczkującego*/
int anti_windup_limit = 1000;  // Anti-Windup - ograniczenie członu całkującego*/
int delay_counter = 0;
int32_t previous_error;  // Poprzedni błąd dla członu różniczkującego
double total_error;  // Suma uchybów dla członu całkującego


// pid_str speedPid;
// pid_str anglePid;
pid_str* pid;
int usedPid = 0;

void setPid(pid_str* pid, int usedPid) {
  pid = pid;
  usedPid = usedPid;
  previous_error = 0;
  total_error = 0;
}

void setPidToAngle() {
  pid = &settings.anglePid;
  usedPid = 1;
}

void setPidToSpeed() {
  pid = &settings.speedPid;
  usedPid = 2;
}

void PID_setPID(float Kp, float Ki, float Kd, int steeringType) {
  if (steeringType == 1) {
    settings.anglePid.Kp = Kp;
    settings.anglePid.Ki = Ki;
    settings.anglePid.Kd = Kd;
  } else if (steeringType == 2) {
    settings.speedPid.Kp = Kp;
    settings.speedPid.Ki = Ki;
    settings.speedPid.Kd = Kd;
  }
}

void PID_reset() {
  total_error = 0;
  previous_error = 0;
}

float PID_calculate(int32_t error, int steeringType) {
  if (steeringType != usedPid) {
    if (steeringType == 1) {
      setPidToAngle();
      pid_divider = 64.0;

    } else {
      setPidToSpeed();
      pid_divider = 1.0;
    }
  }

  //  if (error > 0 && pid->total_error < 0) total_error = 0;     // reset error after reaching target
  //  if (error < 0 && pid->total_error > 0) total_error = 0;     // reset error after reaching target
  total_error += error / (1000.0 * pid_divider);         // sumowanie uchybu (pid_divider ma za zadanie odwzorować działanie regulatora PID przy 12-bitowym enkoderze.)
  p_term = (float)(pid->Kp * error);                     // odpowiedź członu proporcjonalnego
  i_term = (float)(pid->Ki * total_error);               // odpowiedź członu całkującego
  d_term = (float)(pid->Kd * (error - previous_error));  // odpowiedź członu różniczkującego

  if (i_term >= anti_windup_limit)
    i_term = anti_windup_limit;  // Anti-Windup - ograniczenie odpowiedzi członu całkującego
  else if (i_term <= -anti_windup_limit)
    i_term = -anti_windup_limit;

  delay_counter++;
  if (delay_counter > 10) {
    previous_error = error;  // aktualizacja zmiennej z poprzednią wartością błędu
    delay_counter = 0;
  }

  return (float)(p_term + i_term + d_term) / 1000;  // odpowiedź regulatora
}
