/*******************************************************************************
 * Copyright (C) 2022 Amlogic, Inc. All rights reserved.
 ******************************************************************************/

/*****************************************************************************/
/**
 *
 * @file adlak_bitmap.c
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

/***************************** Include Files *********************************/
#include "adlak_bitmap.h"

#include "adlak_log.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/

unsigned long find_next_zero_bit_le(const unsigned long *map, unsigned long size,
                                    unsigned long start) {
    unsigned long tmp;
    int           i;
    if (map == NULL) {
        return size;
    }
    // AML_LOG_DEBUG("find_next_zero_bit: start = 0x%lX, size = 0x%lX, map_base =0x%lX\n",
    //               (uintptr_t)start, (uintptr_t)size, (uintptr_t)map);
    while (1) {
        if (start >= size) {
            return start;
        }
        tmp = map[start / BITS_PER_LONG];
        if (tmp == (unsigned long)ULLONG_MAX) {
            // AML_LOG_DEBUG(
            //     "find_next_zero_bit: tmp == ULLONG_MAX,start = 0x%lX, tmp = 0x%lX, ULLONG_MAX "
            //     "=0x%lX\n",
            //     (uintptr_t)start, (uintptr_t)tmp, (uintptr_t)ULLONG_MAX);
            start += BITS_PER_LONG;
        } else {
            // AML_LOG_DEBUG(
            //     "find_next_zero_bit: tmp != ULLONG_MAX,start = 0x%lX, tmp = 0x%lX, ULLONG_MAX "
            //     "=0x%lX\n",
            //     (uintptr_t)start, (uintptr_t)tmp, (uintptr_t)ULLONG_MAX);
            break;
        }
    }
    for (i = 0; i < BITS_PER_LONG; i++) {
        if (!(tmp & 1)) {
            break;
        }
        tmp = (tmp >> 1);
    }
    // AML_LOG_DEBUG("find_next_zero_bit: return start = 0x%lX\n", (uintptr_t)start + i);
    return start + i;
}
unsigned long find_next_bit_le(unsigned long *map, unsigned long end, unsigned long start) {
    unsigned long tmp;
    int           i;
    if (map == NULL) {
        return end;
    }
    // AML_LOG_DEBUG("find_next_bit: start = 0x%lX, end = 0x%lX, map_base =0x%lX\n",
    // (uintptr_t)start,
    //               (uintptr_t)end, (uintptr_t)map);
    while (1) {
        if (start >= end) {
            return start;
        }
        tmp = map[start / BITS_PER_LONG];
        // AML_LOG_DEBUG("find_next_bit: tmp_ori =0x%lX\n", (uintptr_t)tmp);
        tmp = tmp & BITMAP_FIRST_WORD_MASK(start % BITS_PER_LONG);
        // AML_LOG_DEBUG("find_next_bit: tmp =0x%lX\n", (uintptr_t)tmp);
        if (start + BITS_PER_LONG > end) {
            tmp = (~BITMAP_FIRST_WORD_MASK(end % BITS_PER_LONG)) & tmp;
        }

        // AML_LOG_DEBUG("find_next_bit: start = 0x%lX, end = 0x%lX, tmp =0x%lX\n",
        // (uintptr_t)start,
        //               (uintptr_t)end, (uintptr_t)tmp);

        if (tmp != 0) {
            return start;
        } else {
            start += BITS_PER_LONG;
        }
    }
}

#define find_next_zero_bit find_next_zero_bit_le
#define find_next_bit find_next_bit_le
/**
 * bitmap_find_next_zero_area_off - find a contiguous aligned zero area
 * @map: The address to base the search on
 * @size: The bitmap size in bits
 * @start: The bitnumber to start searching at
 * @nr: The number of zeroed bits we're looking for
 * @align_mask: Alignment mask for zero area
 * @align_offset: Alignment offset for zero area.
 *
 * The @align_mask should be one less than a power of 2; the effect is that
 * the bit offset of all zero areas this function finds plus @align_offset
 * is multiple of that power of 2.
 */
unsigned long bitmap_find_next_zero_area_off(unsigned long *map, unsigned long size,
                                             unsigned long start, unsigned int nr,
                                             unsigned long align_mask, unsigned long align_offset) {
    unsigned long index, end, i;
again:
    index = find_next_zero_bit(map, size, start);

    /* Align allocation */
    index = __ALIGN_MASK(index + align_offset, align_mask) - align_offset;

    end = index + nr;
    if (end > size) {
        return end;
    }
    i = find_next_bit(map, end, index);
    if (i < end) {
        start = i + 1;
        goto again;
    }
    return index;
}
// #if !defined(bitmap_set)
void __bitmap_set(unsigned long *map, unsigned int start, int len) {
    unsigned long *    p           = map + BIT_WORD(start);
    const unsigned int size        = start + len;
    int                bits_to_set = BITS_PER_LONG - (start % BITS_PER_LONG);
    unsigned long      mask_to_set = BITMAP_FIRST_WORD_MASK(start);

    while (len - bits_to_set >= 0) {
        *p |= mask_to_set;
        len -= bits_to_set;
        bits_to_set = BITS_PER_LONG;
        mask_to_set = ~0UL;
        p++;
    }
    if (len) {
        mask_to_set &= BITMAP_LAST_WORD_MASK(size);
        *p |= mask_to_set;
    }
}
// #endif

// #if !defined(bitmap_clear)
void __bitmap_clear(unsigned long *map, unsigned int start, int len) {
    unsigned long *    p             = map + BIT_WORD(start);
    const unsigned int size          = start + len;
    int                bits_to_clear = BITS_PER_LONG - (start % BITS_PER_LONG);
    unsigned long      mask_to_clear = BITMAP_FIRST_WORD_MASK(start);

    while (len - bits_to_clear >= 0) {
        *p &= ~mask_to_clear;
        len -= bits_to_clear;
        bits_to_clear = BITS_PER_LONG;
        mask_to_clear = ~0UL;
        p++;
    }
    if (len) {
        mask_to_clear &= BITMAP_LAST_WORD_MASK(size);
        *p &= ~mask_to_clear;
    }
}
// #endif