/*******************************************************************************
 * Copyright (C) 2022 Amlogic, Inc. All rights reserved.
 ******************************************************************************/

/*****************************************************************************/
/**
 *
 * @file adlak_interrupt.c
 * @brief
 *
 * <pre>
 * MODIFICATION HISTORY:
 *
 * Ver   	Who				Date				Changes
 * ----------------------------------------------------------------------------
 * 1.00a shiwei.sun@amlogic.com	2022/05/04	Initial release
 * </pre>
 *
 ******************************************************************************/

/***************************** Include Files *********************************/

#include "adlak_interrupt.h"

#include "adlak_api.h"
#include "adlak_common.h"
#include "adlak_context.h"
#include "adlak_device.h"
#include "adlak_hw.h"
#include "adlak_platform_config.h"
#include "adlak_queue.h"
#include "adlak_submit.h"
/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/

/**
 * @brief top-half IRQ handling
 *
 * @param irq
 * @param dev
 * @return int
 */
static int adlak_irq_top_half(const int irq, void *dev) {
    struct adlak_device *const padlak = dev;
    int                        rc;

    AML_LOG_DEBUG("%s", __func__);
    // if (!padlak) {
    //     return IRQ_NONE;
    // }

    // TODO(shiwei.sun)  You need to implement the function according to the platform interface.

    // adlak_os_spinlock_lock(&padlak->spinlock);
    // rc = adlak_irq_proc(padlak);
    // adlak_os_spinlock_unlock(&padlak->spinlock);
    // if (rc) {
    //     return IRQ_NONE;
    // }

    // return IRQ_HANDLED;
}

/**
 * @brief register irq line and register intterrupt callback function;
 *
 * @param data
 * @return int
 */
int adlak_irq_init(void *data) {
    int                  ret    = 0;
    struct adlak_device *padlak = (struct adlak_device *)data;
    AML_LOG_DEBUG("%s", __func__);

#if !(CONFIG_ADLAK_EMU_EN)
// TODO(shiwei.sun)  You need to implement the function according to the platform interface.
// ret = request_irq(padlak->hw_res.irqline, &adlak_irq_top_half, IRQF_SHARED |
// IRQF_TRIGGER_HIGH,
//                   DEVICE_NAME, padlak);
// if (ret) {
//     AML_LOG_ERR("Failed to request IRQ %d\n", padlak->hw_res.irqline);
//     ret = ERR(EINVAL);
// }
#endif
    return ret;
}

/**
 * @brief unregister irq line and unregister intterrupt callback function
 *
 * @param data
 */
void adlak_irq_deinit(void *data) {
    struct adlak_device *padlak = (struct adlak_device *)data;
    AML_LOG_DEBUG("%s", __func__);

#if !(CONFIG_ADLAK_EMU_EN)
// TODO(shiwei.sun)  You need to implement the function according to the platform interface.
// free_irq(padlak->hw_res.irqline, padlak);
#endif
}