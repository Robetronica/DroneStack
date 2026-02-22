/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ltdc.c
  * @brief   This file provides code for the configuration
  *          of the LTDC instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "ltdc.h"

/* USER CODE BEGIN 0 */
#include "stm32_lcd.h"
#include "stm32n6570_discovery_lcd.h"

static void LCD_init(void);
static uint32_t LCD_GetBppFactor(uint32_t pixel_format);

__attribute__ ((section (".psram_noncacheable")))
__attribute__ ((aligned (32)))
uint8_t lcd_bg_buffer[800 * 480 * 2];

/* USER CODE END 0 */

LTDC_HandleTypeDef hltdc;

/* LTDC init function */
void MX_LTDC_Init(void)
{

  /* USER CODE BEGIN LTDC_Init 0 */

  /* USER CODE END LTDC_Init 0 */

  LTDC_LayerCfgTypeDef pLayerCfg1 = {0};
  LTDC_LayerFlexYUVCoPlanarTypeDef pLayerFlexYUVCoPlanar = {0};

  /* USER CODE BEGIN LTDC_Init 1 */

  /* USER CODE END LTDC_Init 1 */
  hltdc.Instance = LTDC;
  hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
  hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;
  hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;
  hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
  hltdc.Init.HorizontalSync = 4;
  hltdc.Init.VerticalSync = 4;
  hltdc.Init.AccumulatedHBP = 12;
  hltdc.Init.AccumulatedVBP = 12;
  hltdc.Init.AccumulatedActiveW = 812;
  hltdc.Init.AccumulatedActiveH = 492;
  hltdc.Init.TotalWidth = 820;
  hltdc.Init.TotalHeigh = 500;
  hltdc.Init.Backcolor.Blue = 0;
  hltdc.Init.Backcolor.Green = 0;
  hltdc.Init.Backcolor.Red = 0;
  if (HAL_LTDC_Init(&hltdc) != HAL_OK)
  {
    Error_Handler();
  }
  pLayerCfg1.WindowX0 = 0;
  pLayerCfg1.WindowX1 = 800;
  pLayerCfg1.WindowY0 = 0;
  pLayerCfg1.WindowY1 = 480;
  pLayerCfg1.PixelFormat = LTDC_PIXEL_FORMAT_ARGB4444;
  pLayerCfg1.Alpha = 0;
  pLayerCfg1.Alpha0 = 0;
  pLayerCfg1.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
  pLayerCfg1.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
  pLayerCfg1.FBStartAdress = 0;
  pLayerCfg1.ImageWidth = 800;
  pLayerCfg1.ImageHeight = 480;
  pLayerCfg1.Backcolor.Blue = 0;
  pLayerCfg1.Backcolor.Green = 0;
  pLayerCfg1.Backcolor.Red = 0;
  if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg1, 1) != HAL_OK)
  {
    Error_Handler();
  }
  pLayerFlexYUVCoPlanar.Layer.WindowX0 = 0;
  pLayerFlexYUVCoPlanar.Layer.WindowX1 = 800;
  pLayerFlexYUVCoPlanar.Layer.WindowY0 = 0;
  pLayerFlexYUVCoPlanar.Layer.WindowY1 = 480;
  pLayerFlexYUVCoPlanar.Layer.Alpha = 255;
  pLayerFlexYUVCoPlanar.Layer.Alpha0 = 0;
  pLayerFlexYUVCoPlanar.Layer.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
  pLayerFlexYUVCoPlanar.Layer.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
  pLayerFlexYUVCoPlanar.Layer.ImageWidth = 800;
  pLayerFlexYUVCoPlanar.Layer.ImageHeight = 480;
  pLayerFlexYUVCoPlanar.Layer.Backcolor.Blue = 0;
  pLayerFlexYUVCoPlanar.Layer.Backcolor.Green = 0;
  pLayerFlexYUVCoPlanar.Layer.Backcolor.Red = 0;
  pLayerFlexYUVCoPlanar.FlexYUV.YUVOrder = LTDC_YUV_ORDER_LUMINANCE_FIRST;
  pLayerFlexYUVCoPlanar.FlexYUV.LuminanceOrder = LTDC_YUV_LUMINANCE_ORDER_ODD_FIRST;
  pLayerFlexYUVCoPlanar.FlexYUV.ChrominanceOrder = LTDC_YUV_CHROMIANCE_ORDER_U_FIRST;
  pLayerFlexYUVCoPlanar.FlexYUV.LuminanceRescale = LTDC_YUV_LUMINANCE_RESCALE_DISABLE;
  pLayerFlexYUVCoPlanar.YUVAddress = 0;
  pLayerFlexYUVCoPlanar.ColorConverter = LTDC_YUV2RGBCONVERTOR_BT601_REDUCED_RANGE;
  if (HAL_LTDC_ConfigLayerFlexYUVCoPlanar(&hltdc, &pLayerFlexYUVCoPlanar, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LTDC_Init 2 */
  /* Enable LTDC error interrupts (FIFO underrun and transfer error) */
  __HAL_LTDC_ENABLE_IT(&hltdc, LTDC_IT_FU | LTDC_IT_RR);
  /* Optional: enable underrun warning to catch near-underflow conditions */
  __HAL_LTDC_ENABLE_IT(&hltdc, LTDC_IT_FUW);

  /* Ensure LTDC error IRQ lines are enabled in NVIC */
  HAL_NVIC_SetPriority(LTDC_LO_ERR_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(LTDC_LO_ERR_IRQn);
  HAL_NVIC_SetPriority(LTDC_UP_ERR_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(LTDC_UP_ERR_IRQn);

  // Update the layers' addresses to the frame buffer address
  HAL_LTDC_SetAddress(&hltdc, (uint32_t)lcd_bg_buffer, LTDC_LAYER_1);
  HAL_LTDC_Reload(&hltdc, LTDC_RELOAD_IMMEDIATE);
  LCD_init();
  /* USER CODE END LTDC_Init 2 */

}

void HAL_LTDC_MspInit(LTDC_HandleTypeDef* ltdcHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(ltdcHandle->Instance==LTDC)
  {
  /* USER CODE BEGIN LTDC_MspInit 0 */

  /* USER CODE END LTDC_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
    PeriphClkInitStruct.LtdcClockSelection = RCC_LTDCCLKSOURCE_IC16;
    PeriphClkInitStruct.ICSelection[RCC_IC16].ClockSelection = RCC_ICCLKSOURCE_PLL2;
    PeriphClkInitStruct.ICSelection[RCC_IC16].ClockDivider = 128;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* LTDC clock enable */
    __HAL_RCC_LTDC_CLK_ENABLE();

    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**LTDC GPIO Configuration
    PH3     ------> LTDC_B4
    PH6     ------> LTDC_B5
    PD15     ------> LTDC_R2
    PB14     ------> LTDC_HSYNC
    PB13     ------> LTDC_CLK
    PB2     ------> LTDC_B2
    PB15     ------> LTDC_G4
    PE11     ------> LTDC_VSYNC
    PD8     ------> LTDC_R7
    PH4     ------> LTDC_R4
    PD9     ------> LTDC_R1
    PG6     ------> LTDC_B3
    PG5     ------> LTDC_B1
    PA1     ------> LTDC_G2
    PB11     ------> LTDC_G6
    PA15(JTDI)     ------> LTDC_R5
    PG15     ------> LTDC_B0
    PG1     ------> LTDC_G1
    PB12     ------> LTDC_G5
    PG0     ------> LTDC_R0
    PA2     ------> LTDC_B7
    PG12     ------> LTDC_G0
    PB4(NJTRST)     ------> LTDC_R3
    PG8     ------> LTDC_G7
    PA8     ------> LTDC_B6
    PG13     ------> LTDC_DE
    PA0     ------> LTDC_G3
    PG11     ------> LTDC_R6
    */
    GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_6|GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF14_LCD;
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_15|GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF14_LCD;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_14|GPIO_PIN_13|GPIO_PIN_2|GPIO_PIN_15
                          |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF14_LCD;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF14_LCD;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_5|GPIO_PIN_15|GPIO_PIN_1
                          |GPIO_PIN_0|GPIO_PIN_12|GPIO_PIN_8|GPIO_PIN_13
                          |GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF14_LCD;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_15|GPIO_PIN_2|GPIO_PIN_8
                          |GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF14_LCD;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* LTDC interrupt Init */
    HAL_NVIC_SetPriority(LTDC_UP_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(LTDC_UP_IRQn);
    HAL_NVIC_SetPriority(LTDC_UP_ERR_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(LTDC_UP_ERR_IRQn);
  /* USER CODE BEGIN LTDC_MspInit 1 */
    /* Power on the LCD panel and enable backlight */
    HAL_GPIO_WritePin(LCD_ONOFF_GPIO_Port, LCD_ONOFF_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LCD_BL_CTRL_GPIO_Port, LCD_BL_CTRL_Pin, GPIO_PIN_SET);
  /* USER CODE END LTDC_MspInit 1 */
  }
}

void HAL_LTDC_MspDeInit(LTDC_HandleTypeDef* ltdcHandle)
{

  if(ltdcHandle->Instance==LTDC)
  {
  /* USER CODE BEGIN LTDC_MspDeInit 0 */

  /* USER CODE END LTDC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_LTDC_CLK_DISABLE();

    /**LTDC GPIO Configuration
    PH3     ------> LTDC_B4
    PH6     ------> LTDC_B5
    PD15     ------> LTDC_R2
    PB14     ------> LTDC_HSYNC
    PB13     ------> LTDC_CLK
    PB2     ------> LTDC_B2
    PB15     ------> LTDC_G4
    PE11     ------> LTDC_VSYNC
    PD8     ------> LTDC_R7
    PH4     ------> LTDC_R4
    PD9     ------> LTDC_R1
    PG6     ------> LTDC_B3
    PG5     ------> LTDC_B1
    PA1     ------> LTDC_G2
    PB11     ------> LTDC_G6
    PA15(JTDI)     ------> LTDC_R5
    PG15     ------> LTDC_B0
    PG1     ------> LTDC_G1
    PB12     ------> LTDC_G5
    PG0     ------> LTDC_R0
    PA2     ------> LTDC_B7
    PG12     ------> LTDC_G0
    PB4(NJTRST)     ------> LTDC_R3
    PG8     ------> LTDC_G7
    PA8     ------> LTDC_B6
    PG13     ------> LTDC_DE
    PA0     ------> LTDC_G3
    PG11     ------> LTDC_R6
    */
    HAL_GPIO_DeInit(GPIOH, GPIO_PIN_3|GPIO_PIN_6|GPIO_PIN_4);

    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_15|GPIO_PIN_8|GPIO_PIN_9);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_14|GPIO_PIN_13|GPIO_PIN_2|GPIO_PIN_15
                          |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_4);

    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_11);

    HAL_GPIO_DeInit(GPIOG, GPIO_PIN_6|GPIO_PIN_5|GPIO_PIN_15|GPIO_PIN_1
                          |GPIO_PIN_0|GPIO_PIN_12|GPIO_PIN_8|GPIO_PIN_13
                          |GPIO_PIN_11);

    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1|GPIO_PIN_15|GPIO_PIN_2|GPIO_PIN_8
                          |GPIO_PIN_0);

    /* LTDC interrupt Deinit */
    HAL_NVIC_DisableIRQ(LTDC_UP_IRQn);
    HAL_NVIC_DisableIRQ(LTDC_UP_ERR_IRQn);
  /* USER CODE BEGIN LTDC_MspDeInit 1 */

  /* USER CODE END LTDC_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void ltdc2_set_address_no_reload(uint32_t Address)
{
  if (HAL_LTDC_SetAddress_NoReload(&hltdc, Address, LTDC_LAYER_2) != HAL_OK)
  {
    Error_Handler();
  }
}

void ltdc2_reload(void)
{
  if (HAL_LTDC_ReloadLayer(&hltdc, LTDC_RELOAD_VERTICAL_BLANKING, LTDC_LAYER_2) != HAL_OK)
  {
    Error_Handler();
  }
}

static uint32_t LCD_GetBppFactor(uint32_t pixel_format)
{
  switch (pixel_format)
  {
    case LTDC_PIXEL_FORMAT_ARGB8888:
    case LTDC_PIXEL_FORMAT_ABGR8888:
      return 4U;
    case LTDC_PIXEL_FORMAT_RGB888:
      return 3U;
    case LTDC_PIXEL_FORMAT_RGB565:
    case LTDC_PIXEL_FORMAT_ARGB1555:
    case LTDC_PIXEL_FORMAT_ARGB4444:
    case LTDC_PIXEL_FORMAT_AL88:
      return 2U;
    case LTDC_PIXEL_FORMAT_L8:
    case LTDC_PIXEL_FORMAT_AL44:
      return 1U;
    default:
      return 4U;
}
}

static void LCD_init(void)
{
  const uint32_t instance = 0U;
  const LTDC_LayerCfgTypeDef *background = &hltdc.LayerCfg[LTDC_LAYER_1];
  const LTDC_LayerCfgTypeDef *foreground = &hltdc.LayerCfg[LTDC_LAYER_2];
  const LTDC_LayerCfgTypeDef *layer_cfg = background;
  uint32_t active_layer = LTDC_LAYER_1;

  if ((foreground->ImageWidth != 0U) && (foreground->ImageHeight != 0U))
  {
    layer_cfg = foreground;
    active_layer = LTDC_LAYER_2;
  }

  Lcd_Ctx[instance].ReloadEnable = 1U;
  Lcd_Ctx[instance].Brightness = 0U;
  Lcd_Ctx[instance].ActiveLayer = active_layer;
  Lcd_Ctx[instance].XSize = layer_cfg->ImageWidth;
  Lcd_Ctx[instance].YSize = layer_cfg->ImageHeight;
  Lcd_Ctx[instance].PixelFormat = layer_cfg->PixelFormat;
  Lcd_Ctx[instance].BppFactor = LCD_GetBppFactor(layer_cfg->PixelFormat);

  UTIL_LCD_SetFuncDriver(&LCD_Driver);
  UTIL_LCD_SetLayer(LTDC_LAYER_1);
  UTIL_LCD_Clear(0x00000000);
  UTIL_LCD_SetFont(&Font20);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
}
/* USER CODE END 1 */
