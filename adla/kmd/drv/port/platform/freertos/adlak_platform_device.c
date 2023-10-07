/*******************************************************************************
 * Copyright (C) 2022 Amlogic, Inc. All rights reserved.
 ******************************************************************************/

/*****************************************************************************/
/**
 *
 * @file adlak_platform_device.c
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

#include "adlak_common.h"
#include "adlak_device.h"
#include "adlak_dpm.h"
#include "adlak_hw.h"
#include "adlak_profile.h"
#include "adlak_submit.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/

struct adlak_device *g_adlak_dev = NULL;
int                  g_adlak_log_level;
#if ADLAK_DEBUG
int g_adlak_log_level_pre;
#endif

#include "adlak_fops.c"

static int adlak_platform_remove(struct adlak_device *padlak) {
    int ret = 0;
    AML_LOG_DEBUG("%s", __func__);
    if (NULL == padlak) {
        return ERR(NONE);
    }
    adlak_device_deinit(padlak);
    adlak_os_mutex_lock(&padlak->dev_mutex);
    adlak_platform_free_resource(padlak);
    adlak_os_mutex_unlock(&padlak->dev_mutex);
    adlak_os_spinlock_destroy(&padlak->spinlock);
    adlak_os_mutex_destroy(&padlak->dev_mutex);
    adlak_os_free(padlak);
    /* success */
    AML_LOG_INFO("platform remove done");
    return 0;
}

/**
 * @brief probe operation registered to platform_driver struct
 *        This function will be called while the module is loading.
 *
 * @param pdev: platform device struct pointer
 * @return 0 if successful; others if failed.
 */
static int adlak_platform_probe(struct adlak_platform_config *pdev) {
    int ret = 0;

    struct adlak_device *padlak = NULL;
    AML_LOG_DEBUG("%s", __func__);

    padlak = adlak_os_zalloc(sizeof(struct adlak_device), ADLAK_GFP_KERNEL);
    if (!padlak) {
        ret = ERR(ENOMEM);
        goto err_alloc_data;
    }
    g_adlak_dev = padlak;

    padlak->net_count = 0;
    ret               = adlak_platform_get_resource(padlak, pdev);
    if (ret) {
        goto err_get_res;
    }
    ret = adlak_platform_request_resource(padlak);
    if (ret) {
        goto err_req_res;
    }
    ret = adlak_device_init(padlak);
    if (ret) {
        goto err_dev_init;
    }

    AML_LOG_INFO("ADLAK probe done");

    return 0;

err_dev_init:
    adlak_platform_free_resource(padlak);
err_req_res:
err_get_res:
    adlak_os_free(padlak);
err_alloc_data:
    return ret;
}
#if defined(CONFIG_ADLA_FREERTOS) && CONFIG_ADLAK_EMU_EN
static void *_mem_pool = NULL;
#endif
int adlak_platform_set_info(struct adlak_platform_config *      plat_cfg,
                            const struct adlau_platform_config *uplat_config) {
    int ret = 0;

#if defined(CONFIG_ADLA_FREERTOS) && CONFIG_ADLAK_EMU_EN
    int memory_pool_size;
    plat_cfg->registerMemSize    = 0x1000;   // 4K
    plat_cfg->contiguousSramSize = 0x40000;  // 256KB

    memory_pool_size = 0x140000 + plat_cfg->registerMemSize + plat_cfg->contiguousSramSize + 0x1000;
    _mem_pool        = adlak_os_zalloc(memory_pool_size, 0);
    plat_cfg->contiguousMemBase = ADLAK_ALIGN((uintptr_t)_mem_pool, 0x1000);
    plat_cfg->contiguousMemSize =
        memory_pool_size - plat_cfg->registerMemSize - plat_cfg->contiguousSramSize;
    plat_cfg->contiguousSramBase = plat_cfg->contiguousMemBase + plat_cfg->contiguousMemSize;
    plat_cfg->registerMemBase    = plat_cfg->contiguousSramBase + plat_cfg->contiguousMemSize;

#else
    plat_cfg->registerMemBase    = ADLAK_REG_PHY_ADDR;
    plat_cfg->registerMemSize    = ADLAK_REG_SIZE;
    plat_cfg->contiguousMemBase  = ADLAK_MEM_PHY_ADDR;
    plat_cfg->contiguousMemSize  = ADLAK_MEM_SIZE;
    plat_cfg->contiguousSramBase = ADLAK_SRAM_PHY_ADDR;
    plat_cfg->contiguousSramSize = ADLAK_SRAM_SIZE;
#endif
    plat_cfg->irqline         = ADLAK_IRQ_LINE;
    plat_cfg->has_smmu        = ADLAK_HAS_SMMU;
    plat_cfg->dependency_mode = ADLAK_DEPENDENCY_MODE_MODULE_LAYER;
    plat_cfg->axi_freq        = ADLAK_AXI_FREQ;
    plat_cfg->core_freq       = ADLAK_CORE_FREQ;
    plat_cfg->dpm_period      = ADLAK_DPM_PERIOD;
    plat_cfg->sch_timeout_ms  = ADLAK_TIMEOUT_MAX_MS;
    plat_cfg->cmd_queue_size  = ADLAK_CMD_QUEUE_SIZE;
    plat_cfg->use_smmu        = plat_cfg->has_smmu;
    if (uplat_config) {
        if (-1 != uplat_config->cmd_queue_size) {  // in Bytes,align with 4096
            plat_cfg->cmd_queue_size = uplat_config->cmd_queue_size;
            plat_cfg->cmd_queue_size = ADLAK_ALIGN(plat_cfg->cmd_queue_size, 4096);
        }
        if (-1 != uplat_config->axi_freq) {  // frequence maximum in Hz,
            plat_cfg->axi_freq = uplat_config->axi_freq;
            if (plat_cfg->axi_freq > ADLAK_AXI_FREQ) {
                plat_cfg->axi_freq = ADLAK_AXI_FREQ;
            }
        }
        if (-1 != uplat_config->core_freq) {  // frequence maximum in Hz
            plat_cfg->core_freq = uplat_config->core_freq;
            if (plat_cfg->core_freq > ADLAK_CORE_FREQ) {
                plat_cfg->core_freq = ADLAK_CORE_FREQ;
            }
        }
        if (-1 != uplat_config
                      ->use_smmu) {  // If the device supports smmu, you can decide whether to use.
            if (plat_cfg->use_smmu && uplat_config->use_smmu) {
                plat_cfg->use_smmu = true;
            } else {
                plat_cfg->use_smmu = false;
            }
        }
#if ADLAK_DEBUG
        if (-1 != uplat_config->dependency_mode) {  // debug value
            plat_cfg->dependency_mode = uplat_config->dependency_mode;
        }
#endif
        if (-1 != uplat_config->sch_timeout_ms) {  // Recommended value range[10 - 30]
            plat_cfg->sch_timeout_ms = uplat_config->sch_timeout_ms;
        }
        if (-1 != uplat_config->dpm_enable) {  // Whether the dynamic power mannegement is enabled
            if (uplat_config->dpm_enable) {
                plat_cfg->dpm_enable = true;
            } else {
                plat_cfg->dpm_enable = false;
            }
        }
        if (-1 != uplat_config->dpm_period) {  // the period of dynamic power mannegement check.
            plat_cfg->dpm_period = uplat_config->dpm_period;
        }
    }

    return ret;
}

int adlak_platform_init(uintptr_t *dev_hd, const void *uplat_config) {
    int ret           = 0;
    g_adlak_log_level = ADLAK_LOG_LEVEL;
    struct adlak_platform_config platform_config;
#if ADLAK_DEBUG
    g_adlak_log_level_pre = g_adlak_log_level;
#endif
    AML_LOG_DEBUG("%s", __func__);
    ret = adlak_platform_set_info(&platform_config, (struct adlau_platform_config *)uplat_config);
    if (ERR(NONE) != ret) {
        AML_LOG_ERR("platform set fail.\n");
        goto err_set_plat_fail;
    }
    ret = adlak_platform_probe(&platform_config);
    if (ERR(NONE) != ret) {
        AML_LOG_ERR("platform probe fail.\n");
        goto err_probe_fail;
    }

    AML_LOG_DEBUG("platform probe success.\n");
    *dev_hd = (uintptr_t)g_adlak_dev;

err_probe_fail:
err_set_plat_fail:
    return ret;
}

int adlak_platform_deinit(uintptr_t *dev_hd) {
    int ret = ERR(NONE);
    ASSERT(*dev_hd);

    struct adlak_device *padlak = (struct adlak_device *)(uintptr_t)*dev_hd;
    AML_LOG_DEBUG("%s", __func__);
    adlak_platform_remove(padlak);
#if defined(CONFIG_ADLA_FREERTOS) && CONFIG_ADLAK_EMU_EN
    if (_mem_pool) {
        adlak_os_free(_mem_pool);
        _mem_pool = NULL;
    }
#endif
    *dev_hd = (uintptr_t)NULL;
    return ret;
}