/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Heart_Beat_Pin GPIO_PIN_13
#define Heart_Beat_GPIO_Port GPIOC
#define PANELS_OUTPUT_Pin GPIO_PIN_0
#define PANELS_OUTPUT_GPIO_Port GPIOA
#define HC06_TX_Pin GPIO_PIN_2
#define HC06_TX_GPIO_Port GPIOA
#define HC06_RX_Pin GPIO_PIN_3
#define HC06_RX_GPIO_Port GPIOA
#define Flash_SCK_Pin GPIO_PIN_5
#define Flash_SCK_GPIO_Port GPIOA
#define Flash_MISO_Pin GPIO_PIN_6
#define Flash_MISO_GPIO_Port GPIOA
#define Flash_MOSI_Pin GPIO_PIN_7
#define Flash_MOSI_GPIO_Port GPIOA
#define Flash_CS_Pin GPIO_PIN_0
#define Flash_CS_GPIO_Port GPIOB
#define USB_TX_Pin GPIO_PIN_9
#define USB_TX_GPIO_Port GPIOA
#define USB_RX_Pin GPIO_PIN_10
#define USB_RX_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
