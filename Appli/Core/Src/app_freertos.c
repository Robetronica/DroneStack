/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : FreeRTOS applicative file
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
#include "app_freertos.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32_core_app.h"
#include "stm32_debug_log.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for OffboardControlTask */
osThreadId_t OffboardControlTaskHandle;
const osThreadAttr_t OffboardControlTask_attributes = {
  .name = "OffboardControlTask",
  .priority = (osPriority_t) osPriorityHigh,
  .stack_size = 4096 * 4
};
/* Definitions for PlannerTask */
osThreadId_t PlannerTaskHandle;
const osThreadAttr_t PlannerTask_attributes = {
  .name = "PlannerTask",
  .priority = (osPriority_t) osPriorityAboveNormal,
  .stack_size = 4096 * 4
};
/* Definitions for DebugOutputTask */
osThreadId_t DebugOutputTaskHandle;
const osThreadAttr_t DebugOutputTask_attributes = {
  .name = "DebugOutputTask",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 2048 * 4
};
/* Definitions for ispServiceTask */
osThreadId_t ispServiceTaskHandle;
const osThreadAttr_t ispServiceTask_attributes = {
  .name = "ispServiceTask",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 512 * 4
};
/* Definitions for i2c1BusMutex */
osMutexId_t i2c1BusMutexHandle;
const osMutexAttr_t i2c1BusMutex_attributes = {
  .name = "i2c1BusMutex"
};
/* Definitions for PlannedPathQueue */
osMessageQueueId_t PlannedPathQueueHandle;
const osMessageQueueAttr_t PlannedPathQueue_attributes = {
  .name = "PlannedPathQueue"
};
/* Definitions for LcdDebugQueue */
osMessageQueueId_t LcdDebugQueueHandle;
const osMessageQueueAttr_t LcdDebugQueue_attributes = {
  .name = "LcdDebugQueue"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

/* USER CODE BEGIN 5 */
void vApplicationMallocFailedHook(void)
{
   /* vApplicationMallocFailedHook() will only be called if
   configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h. It is a hook
   function that will get called if a call to pvPortMalloc() fails.
   pvPortMalloc() is called internally by the kernel whenever a task, queue,
   timer or semaphore is created. It is also called by various parts of the
   demo application. If heap_1.c or heap_2.c are used, then the size of the
   heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
   FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
   to query the size of free heap space that remains (although it does not
   provide information on how the remaining heap might be fragmented). */
}
/* USER CODE END 5 */

/* USER CODE BEGIN 4 */
void vApplicationStackOverflowHook(xTaskHandle xTask, char *pcTaskName)
{
   /* Run time stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
   called if a stack overflow is detected. */
}
/* USER CODE END 4 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* creation of i2c1BusMutex */
  i2c1BusMutexHandle = osMutexNew(&i2c1BusMutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */
  /* creation of PlannedPathQueue */
  PlannedPathQueueHandle = osMessageQueueNew (1, sizeof(uint16_t), &PlannedPathQueue_attributes);
  /* creation of LcdDebugQueue */
  LcdDebugQueueHandle = osMessageQueueNew (24, 128, &LcdDebugQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */

  /* USER CODE END RTOS_QUEUES */
  /* creation of OffboardControlTask */
  OffboardControlTaskHandle = osThreadNew(StartOffboardControlTask, NULL, &OffboardControlTask_attributes);

  /* creation of PlannerTask */
  PlannerTaskHandle = osThreadNew(StartPlannerTask, NULL, &PlannerTask_attributes);

  /* creation of DebugOutputTask */
  DebugOutputTaskHandle = osThreadNew(StartDebugOutputTask, NULL, &DebugOutputTask_attributes);

  /* creation of ispServiceTask */
  ispServiceTaskHandle = osThreadNew(StartIspServiceTask, NULL, &ispServiceTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}
/* USER CODE BEGIN Header_StartOffboardControlTask */
/**
* @brief Function implementing the OffboardControlTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartOffboardControlTask */
void StartOffboardControlTask(void *argument)
{
  /* USER CODE BEGIN OffboardControlTask */
  core_app_init();
  /* Infinite loop */
  for(;;)
  {
    core_app_tick();
    osDelay(10); /* 100 Hz */
  }
  /* USER CODE END OffboardControlTask */
}

/* USER CODE BEGIN Header_StartPlannerTask */
/**
* @brief Function implementing the PlannerTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartPlannerTask */
void StartPlannerTask(void *argument)
{
  /* USER CODE BEGIN PlannerTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END PlannerTask */
}

/* USER CODE BEGIN Header_StartDebugOutputTask */
/**
* @brief Function implementing the DebugOutputTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDebugOutputTask */
void StartDebugOutputTask(void *argument)
{
  /* USER CODE BEGIN DebugOutputTask */

  char msg[128];
  for(;;)
  {
    if (osMessageQueueGet(LcdDebugQueueHandle, msg, NULL, osWaitForever) == osOK)
    {
      debug_log_show(msg);
    }
  }
  /* USER CODE END DebugOutputTask */
}

/* USER CODE BEGIN Header_StartIspServiceTask */
/**
* @brief Function implementing the ispServiceTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartIspServiceTask */
void StartIspServiceTask(void *argument)
{
  /* USER CODE BEGIN ispServiceTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END ispServiceTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

