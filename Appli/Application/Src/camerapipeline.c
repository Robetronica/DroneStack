 /**
 ******************************************************************************
 * @file    app_camerapipeline.c
 * @author  GPM Application Team
 *
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include <assert.h>
#include "camerapipeline.h"
#include "cmw_camera.h"
#include "app_freertos.h"

#define GAMMA_CONVERSION 0

__attribute__ ((section (".psram_noncacheable")))
__attribute__ ((aligned (32)))
uint8_t bufferStream[SCREEN_WIDTH * SCREEN_HEIGHT * 2];

/* Leave the driver use the default resolution */
#define CAMERA_WIDTH 0
#define CAMERA_HEIGHT 0
#define CAMERA_FPS 30

#define COLOR_BGR (0)
#define COLOR_RGB (1)
#define COLOR_MODE    COLOR_RGB

/*Defines: CMW_MIRRORFLIP_NONE; CMW_MIRRORFLIP_FLIP; CMW_MIRRORFLIP_MIRROR; CMW_MIRRORFLIP_FLIP_MIRROR;*/
#define CAMERA_FLIP CMW_MIRRORFLIP_NONE

extern int32_t cameraFrameReceived;

static void DCMIPP_PipeInitDisplay(void)
{
  CMW_DCMIPP_Conf_t dcmipp_conf = {0};
  int ret;

  dcmipp_conf.output_width = SCREEN_WIDTH;
  dcmipp_conf.output_height = SCREEN_HEIGHT;
  dcmipp_conf.output_format = DCMIPP_PIXEL_PACKER_FORMAT_YUV422_1;
  dcmipp_conf.output_bpp = 2;
  dcmipp_conf.mode = CMW_Aspect_ratio_crop;
  dcmipp_conf.enable_gamma_conversion = GAMMA_CONVERSION;
  dcmipp_conf.enable_swap = 1;
  dcmipp_conf.yuv_conversion = 1;
  uint32_t pitch;
  ret = CMW_CAMERA_SetPipeConfig(DCMIPP_PIPE1, &dcmipp_conf, &pitch);
  assert(ret == HAL_OK);
  assert(dcmipp_conf.output_width * dcmipp_conf.output_bpp == pitch);
}

// static void DCMIPP_PipeInitNn(uint32_t *pitch)
// {
//   CMW_DCMIPP_Conf_t dcmipp_conf;
//   int ret;

//   dcmipp_conf.output_width = SCREEN_WIDTH;
//   dcmipp_conf.output_height = SCREEN_HEIGHT;
//   dcmipp_conf.output_format = DCMIPP_PIXEL_PACKER_FORMAT_RGB888_YUV444_1;
//   dcmipp_conf.output_bpp = NN_BPP;
//   dcmipp_conf.mode = CMW_Aspect_ratio_crop;
//   dcmipp_conf.enable_swap = COLOR_MODE;
//   dcmipp_conf.enable_gamma_conversion = GAMMA_CONVERSION;
//   dcmipp_conf.yuv_conversion = 0;
//   ret = CMW_CAMERA_SetPipeConfig(DCMIPP_PIPE2, &dcmipp_conf, pitch);
//   assert(ret == HAL_OK);
// }

/**
* @brief Init the camera and the 2 DCMIPP pipes
*/
void CameraPipeline_Init(void)
{
  int ret;
  CMW_CameraInit_t cam_conf;

  cam_conf.width = CAMERA_WIDTH;
  cam_conf.height = CAMERA_HEIGHT;
  cam_conf.fps = CAMERA_FPS;
  cam_conf.pixel_format = 0; /* Default; Not implemented yet */
  cam_conf.anti_flicker = 0;
  cam_conf.mirror_flip = CAMERA_FLIP;

  ret = CMW_CAMERA_Init(&cam_conf);
  assert(ret == CMW_ERROR_NONE);
  DCMIPP_PipeInitDisplay();
  //DCMIPP_PipeInitNn(&pitch_nn);
}

void CameraPipeline_DeInit(void)
{
  int ret;
  ret = CMW_CAMERA_DeInit();
  assert(ret == CMW_ERROR_NONE);
}

void CameraPipeline_DisplayPipe_Start(void)
{
  int ret;
  ret = CMW_CAMERA_Start(DCMIPP_PIPE1, lcd_bg_buffer, CMW_MODE_CONTINUOUS);
  assert(ret == CMW_ERROR_NONE);
}

void CameraPipeline_DisplayPipe_DoubleBufferStart(void)
{
  int ret;
  ret = CMW_CAMERA_DoubleBufferStart(DCMIPP_PIPE1, lcd_bg_buffer, bufferStream, CMW_MODE_CONTINUOUS);
  assert(ret == CMW_ERROR_NONE);
}

void CameraPipeline_SnapshotPipe_Start(uint8_t *pipe_dst)
{
  int ret;

  ret = CMW_CAMERA_Start(DCMIPP_PIPE2, pipe_dst, CMW_MODE_SNAPSHOT);
  assert(ret == CMW_ERROR_NONE);
}

void CameraPipeline_DisplayPipe_Stop()
{
  int ret;
  ret = CMW_CAMERA_Suspend(DCMIPP_PIPE1);
  assert(ret == CMW_ERROR_NONE);
}

void CameraPipeline_IspUpdate(void)
{
  int ret = CMW_ERROR_NONE;
  ret = CMW_CAMERA_Run();
  assert(ret == CMW_ERROR_NONE);
}

// /**
//   * @brief  Frame event callback
//   * @param  hdcmipp pointer to the DCMIPP handle
//   * @retval None
//   */
// int CMW_CAMERA_PIPE_FrameEventCallback(uint32_t pipe)
// {
//   switch (pipe)
//   {
//     case DCMIPP_PIPE2 :
//       if (captureFrameReadyQueueHandle != NULL)
//       {
//         bool frameReady = true;
//         (void)osMessageQueuePut(captureFrameReadyQueueHandle,  &frameReady, 0U, 0U);
//       }
//       break;
//     case DCMIPP_PIPE1 :
//       if (frameCountSemHandle != NULL)
//       {
//         /* increment count */
//         osSemaphoreRelease(frameCountSemHandle);
//       }
//       break;
//   }
//   return 0;
// }
