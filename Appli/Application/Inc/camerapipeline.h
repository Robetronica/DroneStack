 /**
 ******************************************************************************
 * @file    app_camerapipeline.h
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
#ifndef CAMERAPIPELINE
#define CAMERAPIPELINE

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>


#define SCREEN_HEIGHT (480)
#define SCREEN_WIDTH  (800)

extern uint8_t lcd_bg_buffer[];
extern uint8_t bufferStream[];


void CameraPipeline_Init();
void CameraPipeline_DeInit(void);
void CameraPipeline_DisplayPipe_Start(void);
void CameraPipeline_DisplayPipe_DoubleBufferStart(void);
void CameraPipeline_DisplayPipe_Stop(void);
void CameraPipeline_SnapshotPipe_Start(uint8_t *nn_pipe_dst);
void CameraPipeline_IspUpdate(void);

#ifdef __cplusplus
}
#endif

#endif