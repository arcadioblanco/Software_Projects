/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    spi.c
 * @brief   This file provides code for the configuration
 *          of the SPI instances.
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
#include "spi.h"

/* USER CODE BEGIN 0 */
uint8_t sdata[6];
int prev;
uint32_t rawVal = 0;
uint8_t parity;
int32_t convertedOffset = 0;
uint8_t errorCounter = 0;
uint8_t calculated_crc = 0;
/* USER CODE END 0 */

SPI_HandleTypeDef hspi2;

/* SPI2 init function */
void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES_RXONLY;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 7;
  hspi2.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi2.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(spiHandle->Instance==SPI2)
  {
  /* USER CODE BEGIN SPI2_MspInit 0 */

  /* USER CODE END SPI2_MspInit 0 */
    /* SPI2 clock enable */
    __HAL_RCC_SPI2_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**SPI2 GPIO Configuration
    PB13     ------> SPI2_SCK
    PB14     ------> SPI2_MISO
    */
    GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN SPI2_MspInit 1 */

  /* USER CODE END SPI2_MspInit 1 */
  }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{

  if(spiHandle->Instance==SPI2)
  {
  /* USER CODE BEGIN SPI2_MspDeInit 0 */

  /* USER CODE END SPI2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SPI2_CLK_DISABLE();

    /**SPI2 GPIO Configuration
    PB13     ------> SPI2_SCK
    PB14     ------> SPI2_MISO
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_13|GPIO_PIN_14);

  /* USER CODE BEGIN SPI2_MspDeInit 1 */

  /* USER CODE END SPI2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
static const uint8_t ab_CRC8_LUT[256] = {
  0x00, 0x97, 0xB9, 0x2E, 0xE5, 0x72, 0x5C, 0xCB, 0x5D, 0xCA, 0xE4, 0x73, 0xB8, 0x2F, 0x01, 0x96,
  0xBA, 0x2D, 0x03, 0x94, 0x5F, 0xC8, 0xE6, 0x71, 0xE7, 0x70, 0x5E, 0xC9, 0x02, 0x95, 0xBB, 0x2C,
  0xE3, 0x74, 0x5A, 0xCD, 0x06, 0x91, 0xBF, 0x28, 0xBE, 0x29, 0x07, 0x90, 0x5B, 0xCC, 0xE2, 0x75,
  0x59, 0xCE, 0xE0, 0x77, 0xBC, 0x2B, 0x05, 0x92, 0x04, 0x93, 0xBD, 0x2A, 0xE1, 0x76, 0x58, 0xCF,
  0x51, 0xC6, 0xE8, 0x7F, 0xB4, 0x23, 0x0D, 0x9A, 0x0C, 0x9B, 0xB5, 0x22, 0xE9, 0x7E, 0x50, 0xC7,
  0xEB, 0x7C, 0x52, 0xC5, 0x0E, 0x99, 0xB7, 0x20, 0xB6, 0x21, 0x0F, 0x98, 0x53, 0xC4, 0xEA, 0x7D,
  0xB2, 0x25, 0x0B, 0x9C, 0x57, 0xC0, 0xEE, 0x79, 0xEF, 0x78, 0x56, 0xC1, 0x0A, 0x9D, 0xB3, 0x24,
  0x08, 0x9F, 0xB1, 0x26, 0xED, 0x7A, 0x54, 0xC3, 0x55, 0xC2, 0xEC, 0x7B, 0xB0, 0x27, 0x09, 0x9E,
  0xA2, 0x35, 0x1B, 0x8C, 0x47, 0xD0, 0xFE, 0x69, 0xFF, 0x68, 0x46, 0xD1, 0x1A, 0x8D, 0xA3, 0x34,
  0x18, 0x8F, 0xA1, 0x36, 0xFD, 0x6A, 0x44, 0xD3, 0x45, 0xD2, 0xFC, 0x6B, 0xA0, 0x37, 0x19, 0x8E,
  0x41, 0xD6, 0xF8, 0x6F, 0xA4, 0x33, 0x1D, 0x8A, 0x1C, 0x8B, 0xA5, 0x32, 0xF9, 0x6E, 0x40, 0xD7,
  0xFB, 0x6C, 0x42, 0xD5, 0x1E, 0x89, 0xA7, 0x30, 0xA6, 0x31, 0x1F, 0x88, 0x43, 0xD4, 0xFA, 0x6D,
  0xF3, 0x64, 0x4A, 0xDD, 0x16, 0x81, 0xAF, 0x38, 0xAE, 0x39, 0x17, 0x80, 0x4B, 0xDC, 0xF2, 0x65,
  0x49, 0xDE, 0xF0, 0x67, 0xAC, 0x3B, 0x15, 0x82, 0x14, 0x83, 0xAD, 0x3A, 0xF1, 0x66, 0x48, 0xDF,
  0x10, 0x87, 0xA9, 0x3E, 0xF5, 0x62, 0x4C, 0xDB, 0x4D, 0xDA, 0xF4, 0x63, 0xA8, 0x3F, 0x11, 0x86,
  0xAA, 0x3D, 0x13, 0x84, 0x4F, 0xD8, 0xF6, 0x61, 0xF7, 0x60, 0x4E, 0xD9, 0x12, 0x85, 0xAB, 0x3C
};
uint8_t CRC_SPI_97_64bit (uint64_t dw_InputData) {
  uint8_t b_Index = 0;
  uint8_t b_CRC = 0;

  b_Index = (uint8_t)((dw_InputData >> 56u) & (uint64_t)0x000000FFu);
  b_CRC = (uint8_t)((dw_InputData >> 48u) & (uint64_t)0x000000FFu);
  b_Index = b_CRC ^ ab_CRC8_LUT[b_Index];
  b_CRC = (uint8_t)((dw_InputData >> 40u) & (uint64_t)0x000000FFu);
  b_Index = b_CRC ^ ab_CRC8_LUT[b_Index];
  b_CRC = (uint8_t)((dw_InputData >> 32u) & (uint64_t)0x000000FFu);
  b_Index = b_CRC ^ ab_CRC8_LUT[b_Index];
  b_CRC = (uint8_t)((dw_InputData >> 24u) & (uint64_t)0x000000FFu);
  b_Index = b_CRC ^ ab_CRC8_LUT[b_Index];
  b_CRC = (uint8_t)((dw_InputData >> 16u) & (uint64_t)0x000000FFu);
  b_Index = b_CRC ^ ab_CRC8_LUT[b_Index];
  b_CRC = (uint8_t)((dw_InputData >> 8u) & (uint64_t)0x000000FFu);
  b_Index = b_CRC ^ ab_CRC8_LUT[b_Index];
  b_CRC = (uint8_t)(dw_InputData & (uint64_t)0x000000FFu);
  b_Index = b_CRC ^ ab_CRC8_LUT[b_Index];
  b_CRC = ab_CRC8_LUT[b_Index];
  return b_CRC;
}

uint32_t SPI_Recive() {
  HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_RESET);
  for (uint16_t i = 0; i < 370; i++) {
	  __NOP();
  }
  // Receive 4 bytes from encoder
  if (HAL_SPI_Receive(&hspi2, sdata, 4, 100) != HAL_OK) {
    HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_SET);
    Error_Handler();
  }

  HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_SET);

  // Extract raw 24-bit data from first 3 bytes
  uint32_t raw24 = ((uint32_t)sdata[0] << 16) | ((uint32_t)sdata[1] << 8) | sdata[2];

  // Calculate CRC
  calculated_crc = ~CRC_SPI_97_64bit(raw24);

  if (calculated_crc != sdata[3]) {
	  encoderConnected = 0;
	  return actualPosition;
  }

  // Extract bits 31–10 (position, 22 bits)
  uint32_t position = raw24 >> 6;

  // Extract status bits from the lower part of sdata[2]
  uint8_t errorBit   = (sdata[2] >> 1) & 0x01;  // bit 9
  uint8_t warningBit = sdata[2] & 0x01;        // bit 8

  if (errorBit == 0) {
    // Invalid position data, return previous valid position
    return actualPosition;
  }

  // Apply offset and wrap in 18-bit space (262144 steps)
  rawVal = (position + convertedOffset) % 262144;

  int32_t delta = (int32_t)(rawVal - actualPosition);
  if (delta > 131072) delta -= 262144;  // wrap negative
  else if (delta < -131072) delta += 262144;  // wrap positive

  if ((errorCounter < 2) && (offsetRequestReceived == 0) && (abs(delta) > 60000)) {
    errorCounter += 1;
    return actualPosition;
  }

  // Update actual position
  actualPosition = rawVal;
  encoderConnected = 1;
  offsetRequestReceived = 0;
  errorCounter = 0;
  return rawVal;
}
// -----------------------------------------------------------------------------------------------------------
/* USER CODE END 1 */
