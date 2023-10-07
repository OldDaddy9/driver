/*******************************************************************************
 * Copyright (C) 2022 Amlogic, Inc. All rights reserved.
 ******************************************************************************/

/*****************************************************************************/
/**
 *
 * @file adlak_io.h
 * @brief
 *
 * <pre>
 * MODIFICATION HISTORY:
 *
 * Ver   	Who				Date				Changes
 * ----------------------------------------------------------------------------
 * 1.00a shiwei.sun@amlogic.com	2022/04/26	Initial release
 * </pre>
 *
 ******************************************************************************/

#ifndef __ADLAK_IO_H__
#define __ADLAK_IO_H__

/***************************** Include Files *********************************/
#include "adlak_common.h"
#include "adlak_hw.h"
#ifdef __cplusplus
extern "C" {
#endif

/************************** Constant Definitions *****************************/

/**************************Global Variable************************************/

/**************************Type Definition and Structure**********************/

static inline uint32_t adlak_read32(struct io_region *region, __IO offset) {
    // TODO(shiwei.sun)  You need to implement the function according to the platform interface.
    //   return 0;
#if ADLAK_HW_DEBUG_EN
    uint32_t val;
#endif
    ASSERT(region);
    ASSERT(region->va_kernel);
    ASSERT(offset < region->size);
#if ADLAK_HW_DEBUG_EN
    val = readl((void __iomem *)((__IO)(region->va_kernel) + offset));
    // printk(KERN_ERR "[ADLAK] read  reg[0x%lX] = [0x%X].", offset, val);
    return val;
#else
    // return readl((void __iomem *)((__IO)(region->va_kernel) + offset));
    return 0;
#endif
}

static inline void adlak_write32(struct io_region *region, __IO offset, unsigned int data) {
    // TODO(shiwei.sun)  You need to implement the function according to the platform interface.
    //   return;
    ASSERT(region);
    ASSERT(region->va_kernel);
    ASSERT(offset < region->size);
#if ADLAK_HW_DEBUG_EN
    printk(KERN_ERR "[ADLAK DEBUG] write reg[0x%lX] = [0x%X].", offset, data);
    writel((uint32_t)data, (void __iomem *)((__IO)(region->va_kernel) + offset));
#else
    //  writel((uint32_t)data, (void __iomem *)((__IO)(region->va_kernel) + offset));
#endif
}

#ifdef __cplusplus
}
#endif

#endif /* __ADLAK_IO_H__ end define*/
