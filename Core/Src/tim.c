/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    tim.c
 * @brief   This file provides code for the configuration
 *          of the TIM instances.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component

 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "tim.h"

/* USER CODE BEGIN 0 */
#define window 5
int counter = 0;
int hearbeatCounter = 0;
int32_t reading = 0;
int32_t trace = 0;
int32_t positionDelta[window] = {0};
uint8_t positionDeltaPointer = 0;
double sum = 0;
// filter variables
uint8_t dir = 0;
int32_t filter_previous = 0;
uint8_t systemStart = 1;

/* USER CODE END 0 */

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim17;

/* TIM2 init function */
void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 360000;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */
  HAL_TIM_Base_Start_IT(&htim2);
  /* USER CODE END TIM2_Init 2 */

}
/* TIM17 init function */
void MX_TIM17_Init(void)
{

  /* USER CODE BEGIN TIM17_Init 0 */

  /* USER CODE END TIM17_Init 0 */

  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM17_Init 1 */

  /* USER CODE END TIM17_Init 1 */
  htim17.Instance = TIM17;
  htim17.Init.Prescaler = 0;
  htim17.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim17.Init.Period = 65535;
  htim17.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim17.Init.RepetitionCounter = 0;
  htim17.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim17) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim17) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim17, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim17, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM17_Init 2 */
  HAL_TIM_PWM_Start(&htim17, TIM_CHANNEL_1);
  /* USER CODE END TIM17_Init 2 */
  HAL_TIM_MspPostInit(&htim17);

}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspInit 0 */

  /* USER CODE END TIM2_MspInit 0 */
    /* TIM2 clock enable */
    __HAL_RCC_TIM2_CLK_ENABLE();

    /* TIM2 interrupt Init */
    HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
  /* USER CODE BEGIN TIM2_MspInit 1 */

  /* USER CODE END TIM2_MspInit 1 */
  }
  else if(tim_baseHandle->Instance==TIM17)
  {
  /* USER CODE BEGIN TIM17_MspInit 0 */

  /* USER CODE END TIM17_MspInit 0 */
    /* TIM17 clock enable */
    __HAL_RCC_TIM17_CLK_ENABLE();
  /* USER CODE BEGIN TIM17_MspInit 1 */

  /* USER CODE END TIM17_MspInit 1 */
  }
}
void HAL_TIM_MspPostInit(TIM_HandleTypeDef* timHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(timHandle->Instance==TIM17)
  {
  /* USER CODE BEGIN TIM17_MspPostInit 0 */

  /* USER CODE END TIM17_MspPostInit 0 */

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**TIM17 GPIO Configuration
    PA7     ------> TIM17_CH1
    */
    GPIO_InitStruct.Pin = PWM_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM17;
    HAL_GPIO_Init(PWM_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN TIM17_MspPostInit 1 */

  /* USER CODE END TIM17_MspPostInit 1 */
  }

}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspDeInit 0 */

  /* USER CODE END TIM2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM2_CLK_DISABLE();

    /* TIM2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM2_IRQn);
  /* USER CODE BEGIN TIM2_MspDeInit 1 */

  /* USER CODE END TIM2_MspDeInit 1 */
  }
  else if(tim_baseHandle->Instance==TIM17)
  {
  /* USER CODE BEGIN TIM17_MspDeInit 0 */

  /* USER CODE END TIM17_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM17_CLK_DISABLE();
  /* USER CODE BEGIN TIM17_MspDeInit 1 */

  /* USER CODE END TIM17_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {  // co 10ms
  if (initialized == 0) return;                                // jeżeli nie zainicjalizowano głównych ustawień to nie rób nic
  hearbeatCounter++;
  if (hearbeatCounter > 9) {  // co 100ms
    hearbeatCounter = 0;
    CAN_sendHeartbeat();  // wysłanie heartbeatu
  }

  HAL_GPIO_TogglePin(LED_ORANGE_GPIO_Port, LED_ORANGE_Pin);
  CANTimeoutTimer--;          // odliczanie do timeoutu magistrali CAN - wyłączamy silnik gdy magistrala nie dostarczy nowych danych sterowania przez 500ms
  if (CANTimeoutTimer < 0) {  // jeżeli minął timeout to wyłącz silnik
    MOTOR_Stop();             // wyłączenie silnika
    workingMode = 0;          // ustawienie trybu pracy na 0
    CANTimeoutTimer = 0;      // reset timera
  }
  reading = SPI_Recive();  // odczyt danych z enkodera
  if (reading != -1) {     // jeżeli odczytano poprawnie
    // filtracja szumów enkodera start
    if (dir == 1 && reading - filter_previous > 0) {
      filter_previous = reading - 1;
    } else if (dir == 1 && filter_previous - reading > 1) {
      filter_previous = reading + 1;
      dir = 0;
    } else if (dir == 0 && filter_previous - reading > 0) {
      filter_previous = reading + 1;
    } else if (dir == 0 && reading - filter_previous > 1) {
      filter_previous = reading - 1;
      dir = 1;
    }
    reading = filter_previous;
    // filtracja szumów enkodera end
    if (systemStart == 1) {  // po pierwszym odczycie musimy ustawić "poprzedni odczyt" w celu uniknięcia wysokiej prędkości przy starcie
      lastEncoderPosition = reading;
      systemStart = 0;
    }

    int32_t delta = reading - lastEncoderPosition;  // obliczenie różnicy położenia enkodera
    if (delta > 131071) {  // jeżeli różnica jest większa niż połowa zakresu enkodera to znaczy że enkoder przeszedł przez 0
      delta -= 262144;
    }
    if (delta < -131071) {  // jeżeli różnica jest mniejsza niż -połowa zakresu enkodera to znaczy że enkoder przeszedł przez 0
        delta += 262144;
    }
    lastEncoderPosition = reading;
    positionDelta[positionDeltaPointer] = delta;  // zapisanie różnicy położenia do tablicy
    positionDeltaPointer++;                       // inkrementacja wskaźnika tablicy
    if (positionDeltaPointer >= window) {         // jeżeli wskaźnik przekroczy wielkość tablicy to zresetuj go
      positionDeltaPointer = 0;
    }

    sum = 0;
    for (int i = 0; i < window; i++) {  // obliczenie średniej z tablicy różnic położenia
      sum += positionDelta[i];
    }
    sum = sum * 100.0 / (double)window / 262144.0 * 60.0;  // przeliczenie na prędkość w RPM (suma różnic położenia * 100 [ilość odczytów na sekundę] / wielkość tablicy / 262144 [rozdzielczość enkodera] * 60[60 sekund w minucie])
    actualSpeed = sum;
  }
  if (reading != -1 && (workingMode == 1 || workingMode == 4)) {  // jeżeli odczytano poprawnie i jesteśmy w trybie pozycji lub utrzymywania pozycji
    trace = reading - settedPosition;                             // obliczenie różnicy położenia enkodera i pozycji docelowej
    if (trace > 131071) {  // jeżeli różnica jest większa niż połowa zakresu enkodera to znaczy że enkoder przeszedł przez 0
      trace -= 262144;
    }
    if (trace < -131071) {  // jeżeli różnica jest mniejsza niż -połowa zakresu enkodera to znaczy że enkoder przeszedł przez 0
      trace += 262144;
    }

    if (trace < -64 && total_error < 0) total_error = 0;  // margines błędu pozycjonowania
    if (trace > 64 && total_error > 0) total_error = 0;   // margines błędu pozycjonowania
    if (trace < 64 && trace > -64) {                       // margines błędu pozycjonowania
      trace = 0;
    }

    MOTOR_PID_Tick(trace, PID_POSITION);             // obliczenie sterowania PID
  } else if (reading != -1 && workingMode == 2) {    // jeżeli odczytano poprawnie enkoder i jesteśmy w trybie prędkości
    if (settedSpeed == 0.0 && actualSpeed == 0.0) {  // jeżeli prędkość docelowa i aktualna są równe 0 to nie rób nic
      total_error = 0;
    }
    MOTOR_PID_Tick(-(settedSpeed * 100) + (sum * 100), PID_SPEED);  // obliczenie sterowania PID
  }

  if (reading == -1) {   // jeżeli odczyt jest błędny
  } else {               // jeżeli odczyt jest poprawny
    if (counter == 0) {  // tramisja danych przez CAN
      CAN_Transmit_Encoder_Data(reading);
    }
    counter++;
    if (counter >= settings.canFrequency)  // reset licznika zależny od nastawy częstotliwości CAN
      counter = 0;
  }
}

void TIM_PWM_Set(double fill) {      // ustawienie wypełnienia PWM
  int timerSet = fill * TIM17->ARR;  // przeliczenie wypełnienia na wartość timera
  if (timerSet < 0)                  // zabezpieczenie przed ujemnym wypełnieniem
    timerSet = 0;
  if (timerSet > TIM17->ARR)  // zabezpieczenie przed wypełnieniem większym niż maksymalne
    timerSet = TIM17->ARR;
  TIM17->CCR1 = timerSet;  // ustawienie wypełnienia PWM
}
/* USER CODE END 1 */
