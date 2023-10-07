/*******************************************************************************
 * Copyright (C) 2021 Amlogic, Inc. All rights reserved.
 ******************************************************************************/
/*****************************************************************************/
/**
 *
 * @file adlak_api.h
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
#ifndef __ADLAK_API_H__
#define __ADLAK_API_H__
/***************************** Include Files *********************************/
#include "adlak_typedef.h"
/*********This dividing line is for avoid clang-format adjusting the order ***/
#include "adlak_api_base.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief user configuration
 * Assuming set to -1, the default value is used
 */
struct adlau_platform_config {
    int cmd_queue_size;   // in Bytes,align with 4096
    int axi_freq;         // frequence maximum in Hz,
    int core_freq;        // frequence maximum in Hz
    int use_smmu;         // If the device supports smmu, you can decide whether to use.
    int dependency_mode;  // debug value
    int sch_timeout_ms;   // Recommended value range[10 - 30]
    int dpm_enable;       // Whether the dynamic power mannegement is enabled
    int dpm_period;       // the period of dynamic power mannegement check.
};

int adlak_platform_init(uintptr_t *hd, const void *dev_init_config);
int adlak_platform_deinit(uintptr_t *hd);
int adlak_dev_open(uintptr_t *context);
int adlak_dev_close(uintptr_t *context);
int adlak_dev_ioctl_querycap(const uintptr_t *context_hd, void *udata);
int adlak_dev_ioctl_reqbuf(const uintptr_t *context_hd, void *udata);
int adlak_dev_ioctl_freebuf(const uintptr_t *context_hd, void *udata);
int adlak_dev_ioctl_attach_extern_buf(const uintptr_t *context_hd, void *udata);
int adlak_dev_ioctl_dettach_extern_buf(const uintptr_t *context_hd, void *udata);
int adlak_dev_ioctl_flush_cache(const uintptr_t *context_hd, void *udata);
int adlak_dev_ioctl_register_network(const uintptr_t *context_hd, void *udata);
int adlak_dev_ioctl_destroy_network(const uintptr_t *context_hd, void *udata);
int adlak_dev_ioctl_invoke(const uintptr_t *context_hd, void *udata);
int adlak_dev_ioctl_invoke_cancel(const uintptr_t *context_hd, void *udata);
int adlak_dev_ioctl_get_stat(const uintptr_t *context_hd, void *udata);
int adlak_dev_ioctl_profile_cfg(const uintptr_t *context_hd, void *udata);
int adlak_dev_wait_poll(const uintptr_t *usr_context, int timeout_ms);
#ifdef __cplusplus
}
#endif
#endif /* __ADLAK_API_H__ end define*/
