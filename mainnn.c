/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Buton Kontrollü RGB LED ve Siren Uygulaması
  * @target         : STM32F103C8T6 (PinARM / PinoLab-CodeBoard)
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Pin Tanımlamaları (Donanım Eşleşmesi) --------------------------------------*/
#define RED_Pin                 GPIO_PIN_7
#define RED_GPIO_Port           GPIOA
#define BLUE_Pin                GPIO_PIN_0
#define BLUE_GPIO_Port          GPIOB
#define BUZZER_Pin              GPIO_PIN_11
#define BUZZER_GPIO_Port        GPIOA
#define USER_BUTON_Pin          GPIO_PIN_1
#define USER_BUTON_GPIO_Port    GPIOB

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);

/**
  * @brief  Uygulama ana giriş noktası
  */
int main(void)
{
  uint8_t durum = 0;

  /* MCU Konfigürasyonu */
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();

  /* Sonsuz Döngü */
  while (1)
  {
      // Buton Kontrolü (Dahili Pull-up: basılınca 0 / LOW olur)
      if (HAL_GPIO_ReadPin(USER_BUTON_GPIO_Port, USER_BUTON_Pin) == GPIO_PIN_RESET)
      {
          HAL_Delay(50); // Buton arkı (debounce) engelleme
          
          // Butona basılı tutulduğu sürece bekle (bırakılmasını bekle)
          while (HAL_GPIO_ReadPin(USER_BUTON_GPIO_Port, USER_BUTON_Pin) == GPIO_PIN_RESET);
          HAL_Delay(50); // Bırakma arkı engelleme

          // Durumu artır (0 -> 1 -> 2 -> 3 -> 0)
          durum++;
          if (durum > 3)
          {
              durum = 0;
          }
      }

      // Durumlara göre çıkış kontrolleri
      if (durum == 1)
      {
          // 1. Basış: Yalnızca Kırmızı LED
          HAL_GPIO_WritePin(RED_GPIO_Port, RED_Pin, GPIO_PIN_SET);
          HAL_GPIO_WritePin(BLUE_GPIO_Port, BLUE_Pin, GPIO_PIN_RESET);
          HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
      }
      else if (durum == 2)
      {
          // 2. Basış: Kırmızı ve Mavi LED birlikte
          HAL_GPIO_WritePin(RED_GPIO_Port, RED_Pin, GPIO_PIN_SET);
          HAL_GPIO_WritePin(BLUE_GPIO_Port, BLUE_Pin, GPIO_PIN_SET);
          HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
      }
      else if (durum == 3)
      {
          // 3. Basış: Kırmızı + Mavi LED + Siren Efekti
          HAL_GPIO_WritePin(RED_GPIO_Port, RED_Pin, GPIO_PIN_SET);
          HAL_GPIO_WritePin(BLUE_GPIO_Port, BLUE_Pin, GPIO_PIN_SET);

          // Kesikli ses efekti
          HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
          HAL_Delay(100);
          HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
          HAL_Delay(100);
      }
      else
      {
          // Başlangıç (0) veya 4. Basış: Tüm çıkışlar kapalı
          HAL_GPIO_WritePin(RED_GPIO_Port, RED_Pin, GPIO_PIN_RESET);
          HAL_GPIO_WritePin(BLUE_GPIO_Port, BLUE_Pin, GPIO_PIN_RESET);
          HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
      }
  }
}

/**
  * @brief Sistem Saat Ayarları (HSI 8 MHz)
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Pin Yapılandırmaları
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Port Saatlerini Aktif Et */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* Başlangıç Çıkış Seviyeleri (Tümü Kapalı) */
  HAL_GPIO_WritePin(GPIOA, RED_Pin|BUZZER_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(BLUE_GPIO_Port, BLUE_Pin, GPIO_PIN_RESET);

  /* Buton Pini (PB1) - Input ve Dahili Pull-Up */
  GPIO_InitStruct.Pin = USER_BUTON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(USER_BUTON_GPIO_Port, &GPIO_InitStruct);

  /* Kırmızı LED (PA7) ve Buzzer (PA11) - Çıkış */
  GPIO_InitStruct.Pin = RED_Pin|BUZZER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* Mavi LED (PB0) - Çıkış */
  GPIO_InitStruct.Pin = BLUE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BLUE_GPIO_Port, &GPIO_InitStruct);
}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}