/*******************************************************************************
 * Copyright (C) 2022 Amlogic, Inc. All rights reserved.
 ******************************************************************************/

/*****************************************************************************/
/**
 *
 * @file adlak_platform_device.h
 * @brief
 *
 * <pre>
 * MODIFICATION HISTORY:
 *
 * Ver   	Who				Date				Changes
 * ----------------------------------------------------------------------------
 * 1.00a shiwei.sun@amlogic.com	2022/05/12	Initial release
 * </pre>
 *
 ******************************************************************************/

#ifndef __ADLAK_PLATFORM_DEVICE_H__
#define __ADLAK_PLATFORM_DEVICE_H__

/***************************** Include Files *********************************/
#include "adlak_common.h"
#ifdef __cplusplus
extern "C" {
#endif

struct adlak_platform_config {
    uintptr_t registerMemBase;
    uintptr_t registerMemSize;
    uintptr_t contiguousMemBase;
    uintptr_t contiguousMemSize;
    uintptr_t contiguousSramBase;
    uintptr_t contiguousSramSize;
    int       irqline;
    int       axi_freq; /* Clock frequency expressed in Hz */
    int       core_freq;
    int       has_smmu;
    int       use_smmu;
    int       cmd_queue_size;
    int       dependency_mode;
    int       sch_timeout_ms; /* hw schedule timeout max ,unit is ms */
    int       dpm_enable;
    int       dpm_period;
};

#ifdef __cplusplus
}
#endif

#endif /* __ADLAK_PLATFORM_DEVICE_H__ end define*/
