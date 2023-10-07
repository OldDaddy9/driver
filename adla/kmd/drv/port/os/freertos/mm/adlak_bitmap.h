/*******************************************************************************
 * Copyright (C) 2022 Amlogic, Inc. All rights reserved.
 ******************************************************************************/

/*****************************************************************************/
/**
 *
 * @file adlak_bitmap.h
 * @brief
 *
 * <pre>
 * MODIFICATION HISTORY:
 *
 * Ver   	Who				Date				Changes
 * ----------------------------------------------------------------------------
 * 1.00a shiwei.sun@amlogic.com	2022/05/06	Initial release
 * </pre>
 *
 ******************************************************************************/

#ifndef __ADLAK_BITMAP_H__
#define __ADLAK_BITMAP_H__

/***************************** Include Files *********************************/

#include <string.h>

#include "adlak_typedef.h"
#ifdef __cplusplus
extern "C" {
#endif

/**************************Type Definition and Structure**********************/

#if BYTE_ORDER != LITTLE_ENDIAN
#error "sorry, only supports little endian"
#else
#define __LITTLE_ENDIAN
#endif

#if ((LONG_MAX) > 0x7fffffffL)
#define BITS_PER_LONG 64
#else
#define BITS_PER_LONG 32
#endif

#ifndef BITS_PER_LONG_LONG
#define BITS_PER_LONG_LONG 64
#endif

#define BIT_ULL(nr) ((1ULL) << (nr))
#define BIT_MASK(nr) ((1UL) << ((nr) % BITS_PER_LONG))
#define BIT_WORD(nr) ((nr) / BITS_PER_LONG)
#define BIT_ULL_MASK(nr) ((1ULL) << ((nr) % BITS_PER_LONG_LONG))
#define BIT_ULL_WORD(nr) ((nr) / BITS_PER_LONG_LONG)

#ifndef BITS_PER_BYTE
// #define BITS_PER_BYTE sizeof(char)
#define BITS_PER_BYTE 8
#endif
#if BYTE_ORDER == LITTLE_ENDIAN
#define aligned_byte_mask(n) ((1UL << 8 * (n)) - 1)
#else
#define aligned_byte_mask(n) (~0xffUL << (BITS_PER_LONG - 8 - 8 * (n)))
#endif

#if BYTE_ORDER == LITTLE_ENDIAN
#define BITMAP_MEM_ALIGNMENT 8
#else
#define BITMAP_MEM_ALIGNMENT (8 * sizeof(unsigned long))
#endif
#define BITMAP_MEM_MASK (BITMAP_MEM_ALIGNMENT - 1)

#define __KERNEL_DIV_ROUND_UP(n, d) (((n) + (d)-1) / (d))
#define BITS_PER_TYPE(type) (sizeof(type) * BITS_PER_BYTE)
#define BITS_TO_LONGS(nr) __KERNEL_DIV_ROUND_UP(nr, BITS_PER_TYPE(long))

#define BITMAP_FIRST_WORD_MASK(start) (~0UL << ((start) & (BITS_PER_LONG - 1)))
#define BITMAP_LAST_WORD_MASK(nbits) (~0UL >> (-(nbits) & (BITS_PER_LONG - 1)))

/************************** Function Prototypes ******************************/
static inline void bitmap_zero(unsigned long *dst, unsigned int nbits) {
    unsigned int len = BITS_TO_LONGS(nbits) * sizeof(unsigned long);
    memset(dst, 0, len);
}

static inline void bitmap_fill(unsigned long *dst, unsigned int nbits) {
    unsigned int len = BITS_TO_LONGS(nbits) * sizeof(unsigned long);
    memset(dst, 0xff, len);
}

/*
 * These functions are the basis of our bit ops.
 */
static inline void set_bit(unsigned int bit, volatile unsigned long *p) {
    // unsigned long flags;
    unsigned long mask = BIT_MASK(bit);

    p += BIT_WORD(bit);

    // raw_local_irq_save(flags);
    *p |= mask;
    // raw_local_irq_restore(flags);
}

static inline void clear_bit(unsigned int bit, volatile unsigned long *p) {
    // unsigned long flags;
    unsigned long mask = BIT_MASK(bit);

    p += BIT_WORD(bit);

    // raw_local_irq_save(flags);
    *p &= ~mask;
    // raw_local_irq_restore(flags);
}

static inline void change_bit(unsigned int bit, volatile unsigned long *p) {
    // unsigned long flags;
    unsigned long mask = BIT_MASK(bit);

    p += BIT_WORD(bit);

    // raw_local_irq_save(flags);
    *p ^= mask;
    // raw_local_irq_restore(flags);
}

#define __set_bit(nr, addr) set_bit((nr), (addr))
#define __clear_bit(nr, addr) clear_bit((nr), (addr))
#define __change_bit(nr, addr) change_bit((nr), (addr))

void          __bitmap_clear(unsigned long *map, unsigned int start, int len);
void          __bitmap_set(unsigned long *map, unsigned int start, int len);
unsigned long bitmap_find_next_zero_area_off(unsigned long *map, unsigned long size,
                                             unsigned long start, unsigned int nr,
                                             unsigned long align_mask, unsigned long align_offset);

/**
 * bitmap_find_next_zero_area - find a contiguous aligned zero area
 * @map: The address to base the search on
 * @size: The bitmap size in bits
 * @start: The bitnumber to start searching at
 * @nr: The number of zeroed bits we're looking for
 * @align_mask: Alignment mask for zero area
 *
 * The @align_mask should be one less than a power of 2; the effect is that
 * the bit offset of all zero areas this function finds is multiples of that
 * power of 2. A @align_mask of 0 means no alignment is required.
 */
static inline unsigned long bitmap_find_next_zero_area(unsigned long *map, unsigned long size,
                                                       unsigned long start, unsigned int nr,
                                                       unsigned long align_mask) {
    return bitmap_find_next_zero_area_off(map, size, start, nr, align_mask, 0);
}

static __always_inline void bitmap_set(unsigned long *map, unsigned int start, unsigned int nbits) {
    if (__builtin_constant_p(nbits) && nbits == 1)
        __set_bit(start, map);
    else if (__builtin_constant_p(start & BITMAP_MEM_MASK) &&
             IS_ALIGNED(start, BITMAP_MEM_ALIGNMENT) &&
             __builtin_constant_p(nbits & BITMAP_MEM_MASK) &&
             IS_ALIGNED(nbits, BITMAP_MEM_ALIGNMENT))
        memset((char *)map + start / 8, 0xff, nbits / 8);
    else
        __bitmap_set(map, start, nbits);
}

static __always_inline void bitmap_clear(unsigned long *map, unsigned int start,
                                         unsigned int nbits) {
    if (__builtin_constant_p(nbits) && nbits == 1)
        __clear_bit(start, map);
    else if (__builtin_constant_p(start & BITMAP_MEM_MASK) &&
             IS_ALIGNED(start, BITMAP_MEM_ALIGNMENT) &&
             __builtin_constant_p(nbits & BITMAP_MEM_MASK) &&
             IS_ALIGNED(nbits, BITMAP_MEM_ALIGNMENT))
        memset((char *)map + start / 8, 0, nbits / 8);
    else
        __bitmap_clear(map, start, nbits);
}

#ifdef __cplusplus
}
#endif

#endif /* __ADLAK_BITMAP_H__ end define*/
