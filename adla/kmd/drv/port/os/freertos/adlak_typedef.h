/*******************************************************************************
 * Copyright (C) 2022 Amlogic, Inc. All rights reserved.
 ******************************************************************************/

/*****************************************************************************/
/**
 *
 * @file adlak_typedef.h
 * @brief
 *
 * <pre>
 * MODIFICATION HISTORY:
 *
 * Ver   	Who				Date				Changes
 * ----------------------------------------------------------------------------
 * 1.00a shiwei.sun@amlogic.com	2022/04/29	Initial release
 * </pre>
 *
 ******************************************************************************/

#ifndef __ADLAK_TYPEDEF_H__
#define __ADLAK_TYPEDEF_H__

/***************************** Include Files *********************************/

#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef __cplusplus
extern "C" {
#endif

/************************** Constant Definitions *****************************/

/**************************Global Variable************************************/

/**************************Type Definition and Structure**********************/

typedef size_t    resource_size_t;
typedef uintptr_t phys_addr_t;
typedef uintptr_t dma_addr_t;
typedef int       gfp_t;
typedef intptr_t  ktime_t;

#define scnprintf snprintf
#ifndef BYTE_ORDER
#define LITTLE_ENDIAN 0x4321
#define BIG_ENDIAN 0x1234
#define BYTE_ORDER LITTLE_ENDIAN
#endif  // BYTE_ORDER

#if ((LONG_MAX) > 0x7fffffffL)
#ifndef CONFIG_64BIT
#define CONFIG_64BIT
#endif
#endif

#ifndef ALIGN
#define __ALIGN_MASK(x, mask) (((x) + (mask)) & ~(mask))
#endif

#ifdef _GNU_SOURCE
#define ALIGN(x, a) __ALIGN_MASK(x, (typeof(x))(a)-1)
#else
#define ALIGN(x, a) (((x) + (a)-1) & ~((a)-1))
#endif

#define IS_ALIGNED(x, a) (((x) & ((typeof(x))(a)-1)) == 0)

#define PAGE_SIZE (4096)
#define PAGE_SHIFT (12)
/* to align the pointer to the (next) page boundary */
#define PAGE_ALIGN(addr) ALIGN(addr, PAGE_SIZE)
/* test whether an address (unsigned long or pointer) is aligned to PAGE_SIZE */
#define PAGE_ALIGNED(addr) IS_ALIGNED((unsigned long)(addr), PAGE_SIZE)

#define DIV_ROUND_UP(n, d) (((n) + (d)-1) / (d))

#define GFP_KERNEL 0

#ifndef __packed
#define __packed __attribute__((__packed__))
#endif

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define cant_sleep()
#define might_sleep

#define likely(a) (a)

#define __memory_barrier()  // mb()//TODO(shiwei.sun)  You need to implement the function according
                            // to the platform interface.
#define mb() __memory_barrier()
#define barrier() __memory_barrier()

#define min(x, y)           \
    ({                      \
        typeof(x) _x = (x); \
        typeof(y) _y = (y); \
        (void)(&_x == &_y); \
        _x < _y ? _x : _y;  \
    })

#define max(x, y)           \
    ({                      \
        typeof(x) _x = (x); \
        typeof(y) _y = (y); \
        (void)(&_x == &_y); \
        _x > _y ? _x : _y;  \
    })

enum dma_data_direction {
    DMA_BIDIRECTIONAL = 0,
    DMA_TO_DEVICE     = 1,
    DMA_FROM_DEVICE   = 2,
    DMA_NONE          = 3,
};

#include "port/adla_error.h"

/************************** Function Prototypes ******************************/

#ifdef __cplusplus
}
#endif

#endif /* __ADLAK_TYPEDEF_H__ end define*/
