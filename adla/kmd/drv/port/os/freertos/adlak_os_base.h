/*******************************************************************************
 * Copyright (C) 2022 Amlogic, Inc. All rights reserved.
 ******************************************************************************/

/*****************************************************************************/
/**
 *
 * @file adlak_os_base.h
 * @brief
 *
 * <pre>
 * MODIFICATION HISTORY:
 *
 * Ver   	Who				Date				Changes
 * ----------------------------------------------------------------------------
 * 1.00a shiwei.sun@amlogic.com	2022/05/05	Initial release
 * </pre>
 *
 ******************************************************************************/

#ifndef __ADLAK_OS_BASE_H__
#define __ADLAK_OS_BASE_H__

/***************************** Include Files *********************************/

#include <FreeRTOS.h>
#include <semphr.h>
#include <timers.h>

/*********This dividing line is for avoid clang-format adjusting the order ***/
#include "adlak_bitmap.h"
#include "adlak_list.h"

#ifdef __cplusplus
extern "C" {
#endif

/************************** Constant Definitions *****************************/

/**************************Global Variable************************************/

/**************************Type Definition and Structure**********************/

/************************** Function Prototypes ******************************/

#ifdef __cplusplus
}
#endif

#endif /* __ADLAK_OS_BASE_H__ end define*/
