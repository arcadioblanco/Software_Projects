/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    can.c
 * @brief   This file provides code for the configuration
 *          of the CAN instances.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "can.h"
#include "gpio.h"
/* USER CODE BEGIN 0 */
CAN_TxHeaderTypeDef TxHeader;
CAN_RxHeaderTypeDef RxHeader;

uint32_t TxMailbox[4];

uint8_t TxData[8];
uint8_t RxData[8];

uint8_t expectedFrameSize[256] = {0};  // table with exoected frame sizes, in frame size is command number and data

void CAN_sendError(uint8_t errorCode, uint8_t recivedCommand) {   // wysłanie ramki z błędem zdefiniowanym na wiki oraz w pliku global.h
  TxHeader.DLC = 3;                                               // length of data
  TxData[0] = 0x02;                                               // error frame number
  TxData[1] = errorCode;                                          // numer błędu wedle wiki
  TxData[2] = recivedCommand;                                     // numer komendy, która spowodowała błąd
  HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox[0]);  // dodanie ramki do kolejki wysyłania
  return;
}

void CAN_sendHeartbeat() {                                        // wysyłanie cykliczne ramki z informacją że płytka nadal żyje
  TxHeader.DLC = 1;                                               // length of data
  TxData[0] = 0x00;                                               // error frame number
  HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox[0]);  // dodanie ramki do kolejki wysyłania
  return;
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan) {  // funkcja wywoływana w momencie odebrania ramki
  HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData);     // pobranie ramki z bufora
  int frameSize = RxHeader.DLC;                                    // długość ramki
  if (frameSize == 0) {                                            // jeżeli długość ramki jest równa 0 to wysyłamy błąd (nie akceptujemy takich ramek)
    CAN_sendError(STATUS_NEED_MORE_ARGUMENTS, RxData[0]);
    return;
  }
  if (frameSize != expectedFrameSize[RxData[0]]) {  // jeżeli ramka ma inny rozmiar niż oczekiwany (zdefiniowany niżej) to wysyłamy błąd (nie akceptujemy takich ramek
    if (expectedFrameSize[RxData[0]] == 0) {        // jeżeli nie ma zdefiniowanego rozmiaru ramki to znaczy że nie akceptujemy takiej ramki
      CAN_sendError(STATUS_WRONG_FRAME, RxData[0]);
    } else {
      CAN_sendError(STATUS_NEED_MORE_ARGUMENTS, RxData[0]);
    }
    return;
  }
  if (RxData[0] == 0x10) {  // sprawdzenie czy ramka ustawiająca moc ma poprawną prędkość (-100 do 100)
    int8_t speed = RxData[1];
    if (speed > 100 || speed < -100) {
      CAN_sendError(STATUS_DATA_ERROR, RxData[0]);
      return;
    }
  }
  if (RxData[0] ==0x31)
  {
	  if (RxData[1] != 1 && RxData[1] != 0 )
	  {
		  CAN_sendError(STATUS_DATA_ERROR, RxData[0]);
		  return;
	  }
  }

  if (RxData[0] == 0x10) {  // jeżeli ramka odpowiada za sterowanie zresetuj timeout wyłączenia silnika
    CANTimeoutTimer = 50;
  }

  int16_t response = -1;

  switch (RxData[0]) {  // wybór funkcji do wykonania na podstawie numeru ramki
    case 0x10:          // ustawienie mocy
      settedEffort = (int8_t)RxData[1];
      if (settedEffort != 0) {  // jeżeli moc jest różna od 0 to ustaw tryb pracy na 0
        workingMode = 0;
      }
      if (settedEffort == 1 || settedEffort == -1) {  // jeżeli moc jest równa 1 lub -1 to wyłącz silnik
        MOTOR_Stop();
      } else {
        if (settedEffort > 0) {  // jeżeli moc jest większa od 0 to ustaw silnik na jazdę do przodu
          MOTOR_Start_Forward(settedEffort / 100.0);
        } else if (settedEffort == 0) {  // jeżeli moc jest równa 0 to ustaw silnik na utrzymanie pozycji
            workingMode = 0;
            MOTOR_Stop();

        } else if (settedEffort < 0) {  // jeżeli moc jest mniejsza od 0 to ustaw silnik na jazdę do tyłu
          MOTOR_Start_Backward(-(settedEffort / 100.0));
        }
      }
      break;

    case 0x31:
        if (RxData[1] == 1) {
          NPK_EN = 1;
          HAL_GPIO_WritePin(ENABLE_NPK_GPIO_Port, ENABLE_NPK_Pin, NPK_EN);
        }
        else
        {
          NPK_EN = 0;
          HAL_GPIO_WritePin(ENABLE_NPK_GPIO_Port, ENABLE_NPK_Pin, NPK_EN);
        }
        break;
    case 0xFF:                  // reset
      if (RxData[1] == 0x00) {  // jeżeli drugi bajt ramki jest równy 0 to zresetuj urządzenie
        HAL_NVIC_SystemReset();
      }
      break;
  }
  if (RxData[0] == 0x01) {  // jeżeli ramka jest zapytaniem to odpowiedz na nią
    switch (RxData[1]) {    // wybór funkcji do wykonania na podstawie numeru żądanej ramki
      case 0x00:            // wyślij heartbeat
        response = -1;
        TxHeader.DLC = 1;
        TxData[0] = 0x00;
        break;
      case 0x10:  // wyślij ustawioną moc
        response = -1;
        TxHeader.DLC = 2;
        TxData[0] = 0x10;
        TxData[1] = settedEffort;
        break;
      case 0x31:            // wyślij heartbeat
		 response = -1;
		 TxHeader.DLC = 2;
		 TxData[0] = 0x31;
		 TxData[1] = NPK_EN;

		 break;
      default:
        CAN_sendError(STATUS_WRONG_FRAME, RxData[0]);
        break;
    }
  }
  if (response >= 0) {  // jeżeli wystąpił błąd to wyślij błąd
    CAN_sendError(response, RxData[0]);
  } else {                    // jeżeli nie wystąpił błąd to wyślij odpowiedź
    if (RxData[0] != 0x01) {  // jeżeli ramka nie jest zapytaniem to odpowiedz identyczną ramką
      TxHeader.DLC = RxHeader.DLC;
      for (int i = 0; i < RxHeader.DLC; i++) {  // przepisz dane z ramki otrzymanej do ramki wysyłanej
        TxData[i] = RxData[i];
      }
    }
    HAL_CAN_AddTxMessage(hcan, &TxHeader, TxData, &TxMailbox[0]);  // dodanie ramki do kolejki wysyłania
  }
}

/* USER CODE END 0 */

CAN_HandleTypeDef hcan;

/* CAN init function */
void MX_CAN_Init(void)
{

  /* USER CODE BEGIN CAN_Init 0 */
  // w tym miejscu definiujemy oczekiwane rozmiary ramek, oczekiwany rozmiar nie może być równy 0 (nie akceptujemy ramek o długości 0 bo zawsze jest minimum 1 bajt - numer komendy)
  // tablica ma 256 elementrów, każdy element odpowiada numerowi komendy, wartość elementu to długość ramki
  // domyślna wartość to 0, co oznacza że nie akceptujemy takiej ramki
  expectedFrameSize[0x01] = 2;
  expectedFrameSize[0x10] = 2;
  expectedFrameSize[0x31] = 2;
  expectedFrameSize[0xFF] = 2;

  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */

  /* USER CODE END CAN_Init 1 */
  hcan.Instance = CAN;
  hcan.Init.Prescaler = 8;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_3TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_5TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = DISABLE;
  hcan.Init.AutoWakeUp = DISABLE;
  hcan.Init.AutoRetransmission = DISABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN_Init 2 */

  CAN_FilterTypeDef canfilterconfig;
  canfilterconfig.FilterActivation = CAN_FILTER_ENABLE;
  canfilterconfig.FilterBank = 0;
  canfilterconfig.FilterFIFOAssignment = CAN_RX_FIFO0;
  canfilterconfig.FilterIdHigh = globalCANID << 5;  // filter check ID
  canfilterconfig.FilterIdLow = 0x0000;
  canfilterconfig.FilterMaskIdHigh = 0b1111111111100000;  // mask indicating the exact ID (all bits must match), exact as 0xFFE0
  //	  canfilterconfig.FilterMaskIdHigh = 0b00000; //mask indicating the exact ID (all bits must match), exact as 0xFFE0
  canfilterconfig.FilterMaskIdLow = 0x0000;
  canfilterconfig.FilterMode = CAN_FILTERMODE_IDMASK;
  canfilterconfig.FilterScale = CAN_FILTERSCALE_32BIT;
  canfilterconfig.SlaveStartFilterBank = 14;

  if (HAL_CAN_ConfigFilter(&hcan, &canfilterconfig) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_CAN_Start(&hcan) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK) {
    Error_Handler();
  }

  TxHeader.DLC = 1;
  TxHeader.ExtId = 0;
  TxHeader.IDE = CAN_ID_STD;
  TxHeader.RTR = CAN_RTR_DATA;
  TxHeader.StdId = globalCANID;
  TxHeader.TransmitGlobalTime = DISABLE;

  /* USER CODE END CAN_Init 2 */

}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN)
  {
  /* USER CODE BEGIN CAN_MspInit 0 */

  /* USER CODE END CAN_MspInit 0 */
    /* CAN clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**CAN GPIO Configuration
    PB8     ------> CAN_RX
    PB9     ------> CAN_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* CAN interrupt Init */
    HAL_NVIC_SetPriority(USB_LP_CAN_RX0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USB_LP_CAN_RX0_IRQn);
  /* USER CODE BEGIN CAN_MspInit 1 */

  /* USER CODE END CAN_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN)
  {
  /* USER CODE BEGIN CAN_MspDeInit 0 */

  /* USER CODE END CAN_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();

    /**CAN GPIO Configuration
    PB8     ------> CAN_RX
    PB9     ------> CAN_TX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_8|GPIO_PIN_9);

    /* CAN interrupt Deinit */
    HAL_NVIC_DisableIRQ(USB_LP_CAN_RX0_IRQn);
  /* USER CODE BEGIN CAN_MspDeInit 1 */

  /* USER CODE END CAN_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void CAN_Transmit_Data(int16_t temperature, int16_t humidity) {                   // wysłanie danych z enkodera

  TxHeader.DLC = 5;                                  // rozmiar danych
  TxData[0] = 0x33;                                  // numer ramki
  TxData[1] = temperature >>8;
  TxData[2] = temperature &0xFF;
  TxData[3] = humidity >>8;
  TxData[4] = humidity & 0xFF;
  HAL_CAN_AddTxMessage(&hcan, &TxHeader, &TxData[0], &TxMailbox[0]);  // dodanie ramki do kolejki wysyłania
}
void CAN_Transmit_NPK_EN() {                   // wysłanie danych z enkodera

  TxHeader.DLC = 2;                                  // rozmiar danych
  TxData[0] = 0x31;
  TxData[1] = NPK_EN;                                  // numer ramki// numer ramki
  HAL_CAN_AddTxMessage(&hcan, &TxHeader, &TxData[0], &TxMailbox[0]);  // dodanie ramki do kolejki wysyłania
}
void HAL_CAN_RxFifo0FullCallback(CAN_HandleTypeDef* hcan) {  // funkcja wywoływana w momencie przepełnienia bufora odbiornika
  CAN_sendError(STATUS_TOO_MANY_FRAMES_IN_CAN_QUEUE, 0x00);  // wysłanie błędu
}
/* USER CODE END 1 */
