/*******************************************************************************
 * Copyright (C) 2021 Amlogic, Inc. All rights reserved.
 ******************************************************************************/

/*****************************************************************************/
/**
 *
 * @file adlak_platform_config.c
 * @brief
 *
 * <pre>
 * MODIFICATION HISTORY:
 *
 * Ver   	Who				Date				Changes
 * ----------------------------------------------------------------------------
 * 1.00a shiwei.sun@amlogic.com	2021/06/05	Initial release
 * </pre>
 *
 ******************************************************************************/

/***************************** Include Files *********************************/
#include "adlak_platform_config.h"

#include "adlak_common.h"
#include "adlak_device.h"
#include "adlak_dpm.h"
#include "adlak_hw.h"
#include "adlak_interrupt.h"
#include "adlak_submit.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/

/*****************************************************************************/

int adlak_platform_get_resource(void *data, void *pdev) {
    int                           ret      = 0;
    struct adlak_device *         padlak   = (struct adlak_device *)data;
    struct adlak_platform_config *plat_cfg = (struct adlak_platform_config *)pdev;

    AML_LOG_DEBUG("%s", __func__);

    padlak->smmu_en = plat_cfg->use_smmu;

    if (padlak->smmu_en) {
        AML_LOG_INFO("smmu available.\n");
    } else {
        AML_LOG_INFO("smmu not available.\n");
    }

    /* get ADLAK IO */

    AML_LOG_DEBUG("get ADLAK IO region: [0x%lX, 0x%lX]", (uintptr_t)plat_cfg->registerMemBase,
                  (uintptr_t)(plat_cfg->registerMemBase + plat_cfg->registerMemSize - 1));

    padlak->hw_res.adlak_reg_pa   = plat_cfg->registerMemBase;
    padlak->hw_res.adlak_reg_size = plat_cfg->registerMemSize;

    if (!plat_cfg->contiguousSramSize) {
        AML_LOG_INFO("get platform sram region failed");
        padlak->hw_res.adlak_sram_pa   = 0;
        padlak->hw_res.adlak_sram_size = 0;
    } else {
        AML_LOG_DEBUG("get ADLAK SRAM region: [0x%lX, 0x%lX]",
                      (uintptr_t)plat_cfg->contiguousSramBase,
                      (uintptr_t)(plat_cfg->contiguousSramBase + plat_cfg->contiguousSramSize - 1));
        padlak->hw_res.adlak_sram_pa   = plat_cfg->contiguousSramBase;
        padlak->hw_res.adlak_sram_size = plat_cfg->contiguousSramSize;
    }
    padlak->hw_res.sram_wrap = 1;  // this configure must sync with adla-compiler,the default is
                                   // wrap enable in adla-compiler.

    /* get reserve-memory */

    if (!plat_cfg->contiguousMemSize) {
        AML_LOG_INFO("get platform reserved_memory region failed");
        padlak->hw_res.adlak_resmem_pa   = 0;
        padlak->hw_res.adlak_resmem_size = 0;
    } else {
        AML_LOG_DEBUG("get ADLA reserved_memory region: [0x%lX, 0x%lX]",
                      (uintptr_t)plat_cfg->contiguousMemBase,
                      (uintptr_t)(plat_cfg->contiguousMemBase + plat_cfg->contiguousMemSize - 1));
        padlak->hw_res.adlak_resmem_pa   = plat_cfg->contiguousMemBase;
        padlak->hw_res.adlak_resmem_size = plat_cfg->contiguousMemSize;
    }

    /* get interrupt number */
    if (!plat_cfg->irqline) {
        AML_LOG_ERR("get irqnum failed");
        ret = ERR(EINVAL);
        goto err;
    }
    padlak->hw_res.irqline = plat_cfg->irqline;
    AML_LOG_DEBUG("get IRQ number: %d", padlak->hw_res.irqline);

    padlak->hw_timeout_ms = (plat_cfg->sch_timeout_ms);

    AML_LOG_DEBUG("padlak->hw_timeout_ms =  %d ms", plat_cfg->sch_timeout_ms);

    padlak->cmq_buffer_public.size = ADLAK_ALIGN(plat_cfg->cmd_queue_size, 256);
    AML_LOG_DEBUG("cmq_size=%d byte", padlak->cmq_buffer_public.size);

    if (plat_cfg->dependency_mode >= ADLAK_DEPENDENCY_MODE_COUNT) {
        plat_cfg->dependency_mode = ADLAK_DEPENDENCY_MODE_MODULE_H_COUNT;
    }
    padlak->dependency_mode = plat_cfg->dependency_mode;
    AML_LOG_DEBUG("padlak->dependency_mode =  %d", padlak->dependency_mode);

    padlak->clk_axi_freq_set  = plat_cfg->axi_freq;
    padlak->clk_core_freq_set = plat_cfg->core_freq;
    padlak->dpm_period_set    = plat_cfg->dpm_period;
    return 0;
err:
    return ret;
}

int adlak_platform_request_resource(void *data) {
    int                  ret    = 0;
    struct adlak_device *padlak = (struct adlak_device *)data;

    AML_LOG_DEBUG("%s", __func__);
    padlak->hw_res.preg =
        adlak_create_ioregion((uint64_t)padlak->hw_res.adlak_reg_pa, padlak->hw_res.adlak_reg_size);
    if (NULL == padlak->hw_res.preg) {
        AML_LOG_ERR("create ioregion failed");
        ret = ERR(EINVAL);
        goto err;
    }

    padlak->all_task_num     = 0;
    padlak->all_task_num_max = ADLAK_TASK_COUNT_MAX;

    return 0;
err:
    return ret;
}

int adlak_platform_free_resource(void *data) {
    int                  ret    = 0;
    struct adlak_device *padlak = (struct adlak_device *)data;

    if (padlak->hw_res.preg) {
        adlak_destroy_ioregion(padlak->hw_res.preg);
    }
    return ret;
}

void adlak_platform_set_clock(void *data, bool enable, int core_freq, int axi_freq) {
    struct adlak_device *padlak = (struct adlak_device *)data;
    AML_LOG_DEBUG("%s", __func__);
    // TODO(shiwei.sun)  You need to implement the function according to the platform interface.
}

void adlak_platform_set_power(void *data, bool enable) {
#if CONFIG_HAS_PM_DOMAIN
    int                  ret    = 0;
    struct adlak_device *padlak = (struct adlak_device *)data;
#endif
    AML_LOG_DEBUG("%s", __func__);
    // TODO(shiwei.sun)  You need to implement the function according to the platform interface.
    if (false == enable) {
        // #if CONFIG_HAS_PM_DOMAIN
        //         pm_runtime_put_sync(padlak->dev);
        //         if (pm_runtime_enabled(padlak->dev)) {
        //             pm_runtime_disable(padlak->dev);
        //         }
        // #endif

    } else {
        // #if CONFIG_HAS_PM_DOMAIN
        //         pm_runtime_enable(padlak->dev);
        //         if (pm_runtime_enabled(padlak->dev)) {
        //             ret = pm_runtime_get_sync(padlak->dev);
        //             if (ret < 0) {
        //                 AML_LOG_ERR("Getpower failed\n");
        //             }
        //         }
        // #endif
    }
}
int adlak_platform_pm_init(void *data) {
    int                  ret    = 0;
    struct adlak_device *padlak = (struct adlak_device *)data;
    AML_LOG_DEBUG("%s", __func__);
    if (ret < 0) {
        AML_LOG_ERR("Get power failed\n");
        goto end;
    }

    adlak_os_sema_init(&padlak->sem_pm_wakeup, 1, 0);
    padlak->pm_suspend = false;
    // power on
    adlak_platform_set_power(padlak, true);
    // clk enable
    adlak_platform_set_clock(padlak, true, padlak->clk_core_freq_set, padlak->clk_axi_freq_set);
    padlak->is_suspend = false;
end:
    return ret;
}
void adlak_platform_pm_deinit(void *data) {
    struct adlak_device *padlak = (struct adlak_device *)data;
    AML_LOG_DEBUG("%s", __func__);
    // clk disable
    adlak_platform_set_clock(padlak, false, 0, 0);
    // power off
    adlak_platform_set_power(padlak, false);
    padlak->is_suspend = true;
    adlak_os_sema_destroy(&padlak->sem_pm_wakeup);
}

void adlak_platform_resume(void *data) {
#if CONFIG_ADLAK_DPM_EN
    struct adlak_device *padlak = (struct adlak_device *)data;
    AML_LOG_INFO("%s", __func__);
    if (false != padlak->is_suspend) {
        // power on
        adlak_platform_set_power(padlak, true);
        // clk enable
        adlak_platform_set_clock(padlak, true, padlak->clk_core_freq_set, padlak->clk_axi_freq_set);
        padlak->is_suspend = false;
        adlak_hw_dev_resume(padlak);
    }
#endif
}

void adlak_platform_suspend(void *data) {
#if CONFIG_ADLAK_DPM_EN
    struct adlak_device *padlak = (struct adlak_device *)data;
    AML_LOG_INFO("%s", __func__);
    if (false == padlak->is_suspend) {
        adlak_hw_dev_suspend(padlak);
        padlak->is_suspend = true;
        // clk disable
        adlak_platform_set_clock(padlak, false, 0, 0);
        // power off
        adlak_platform_set_power(padlak, false);
    }
#endif
}
