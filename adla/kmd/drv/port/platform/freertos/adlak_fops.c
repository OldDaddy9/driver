/*****************************************************************************/
/**
 *
 * @file adlak_fops.c
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
#include "adlak_api.h"
#include "adlak_common.h"
#include "adlak_context.h"
#include "adlak_device.h"
#include "adlak_mm.h"
#include "adlak_queue.h"
#include "adlak_submit.h"
/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/
extern struct adlak_device *g_adlak_dev;
/************************** Function Prototypes ******************************/

int adlak_dev_open(uintptr_t *usr_context) {
    int                   ret     = 0;
    struct adlak_device * padlak  = (struct adlak_device *)g_adlak_dev;
    struct adlak_context *context = NULL;

#if ADLAK_DEBUG
    g_adlak_log_level = g_adlak_log_level_pre;
#endif
    AML_LOG_DEBUG("%s", __func__);
    padlak = (struct adlak_device *)g_adlak_dev;

    adlak_os_mutex_lock(&padlak->dev_mutex);
    ret = adlak_create_context(padlak, &context);
    adlak_os_mutex_unlock(&padlak->dev_mutex);
    if (ret) {
        return ret;
    } else {
        *usr_context = (uintptr_t)context;
    }

    /* success */
    return ERR(NONE);
}

int adlak_dev_close(uintptr_t *usr_context) {
    int                   ret     = 0;
    struct adlak_device * padlak  = (struct adlak_device *)g_adlak_dev;
    struct adlak_context *context = (struct adlak_context *)*usr_context;
    ASSERT(usr_context);

    AML_LOG_DEBUG("%s", __func__);

    ret = adlak_destroy_context(padlak, context);
    if (ERR(NONE) != ret) {
        goto err_handle;
    }
    *usr_context = (uintptr_t)NULL;
err_handle:
    return ret;
}

int adlak_dev_ioctl_querycap(const uintptr_t *usr_context, void *udata) {
    int                  ret    = 0;
    struct adlak_device *padlak = (struct adlak_device *)g_adlak_dev;
    ASSERT(padlak);
    ASSERT(usr_context);
    AML_LOG_DEBUG("%s", __func__);
    if (!udata) {
        ret = padlak->dev_caps.size;
    } else {
        /* copy cap info/errcode to user for reference */
        adlak_os_memcpy(udata, padlak->dev_caps.data, padlak->dev_caps.size);
        ret = ERR(NONE);
    }
    return ret;
}

int adlak_dev_ioctl_reqbuf(const uintptr_t *usr_context, void *udata) {
    int                   ret     = 0;
    struct adlak_device * padlak  = (struct adlak_device *)g_adlak_dev;
    struct adlak_context *context = (struct adlak_context *)*usr_context;
    ASSERT(padlak);
    ASSERT(usr_context);
    ASSERT(udata);
    AML_LOG_DEBUG("%s", __func__);
    ret = adlak_os_mutex_lock(&padlak->dev_mutex);
    if (ret) {
        ret = ERR(EIO);
    } else {
        ret = adlak_mem_alloc_request(context, (struct adlak_buf_req *)udata);
        if (ret) {
            ret = ERR(ENOMEM);
        }
        adlak_os_mutex_unlock(&padlak->dev_mutex);
    }
    return ret;
}

int adlak_dev_ioctl_freebuf(const uintptr_t *usr_context, void *udata) {
    int                   ret     = 0;
    struct adlak_device * padlak  = (struct adlak_device *)g_adlak_dev;
    struct adlak_context *context = (struct adlak_context *)*usr_context;
    ASSERT(padlak);
    ASSERT(usr_context);
    ASSERT(udata);
    AML_LOG_DEBUG("%s", __func__);
    ret = adlak_os_mutex_lock(&padlak->dev_mutex);
    if (ret) {
        ret = ERR(EIO);
    } else {
        ret = adlak_mem_free_request(context, *(uint64_t *)udata);
        if (ret) {
            ret = ERR(ENOMEM);
        }
        adlak_os_mutex_unlock(&padlak->dev_mutex);
    }
    return ret;
}

int adlak_dev_ioctl_attach_extern_buf(const uintptr_t *usr_context, void *udata) {
    int                   ret     = 0;
    struct adlak_device * padlak  = (struct adlak_device *)g_adlak_dev;
    struct adlak_context *context = (struct adlak_context *)*usr_context;
    ASSERT(padlak);
    ASSERT(usr_context);
    ASSERT(udata);
    AML_LOG_DEBUG("%s", __func__);
    ret = adlak_os_mutex_lock(&padlak->dev_mutex);
    if (ret) {
        ret = ERR(EIO);
    } else {
        ret = adlak_ext_mem_attach_request(context, (struct adlak_extern_buf_info *)udata);
        if (ret) {
            ret = ERR(ENOMEM);
        }
        adlak_os_mutex_unlock(&padlak->dev_mutex);
    }
    return ret;
}

int adlak_dev_ioctl_dettach_extern_buf(const uintptr_t *usr_context, void *udata) {
    int                   ret     = 0;
    struct adlak_device * padlak  = (struct adlak_device *)g_adlak_dev;
    struct adlak_context *context = (struct adlak_context *)*usr_context;
    ASSERT(padlak);
    ASSERT(usr_context);
    ASSERT(udata);
    AML_LOG_DEBUG("%s", __func__);
    ret = adlak_os_mutex_lock(&padlak->dev_mutex);
    if (ret) {
        ret = ERR(EIO);
    } else {
        ret = adlak_ext_mem_dettach_request(context, *(uint64_t *)udata);
        if (ret) {
            ret = ERR(ENOMEM);
        }
        adlak_os_mutex_unlock(&padlak->dev_mutex);
    }
    return ret;
}

int adlak_dev_ioctl_flush_cache(const uintptr_t *usr_context, void *udata) {
    int                   ret     = 0;
    struct adlak_device * padlak  = (struct adlak_device *)g_adlak_dev;
    struct adlak_context *context = (struct adlak_context *)*usr_context;
    ASSERT(padlak);
    ASSERT(usr_context);
    ASSERT(udata);
    AML_LOG_DEBUG("%s", __func__);
    ret = adlak_os_mutex_lock(&padlak->dev_mutex);
    if (ret) {
        ret = ERR(EIO);
    } else {
        ret = adlak_mem_flush_request(context, (struct adlak_buf_flush *)udata);
        if (ret) {
            ret = ERR(ENOMEM);
        }
        adlak_os_mutex_unlock(&padlak->dev_mutex);
    }
    return ret;
}

int adlak_dev_ioctl_register_network(const uintptr_t *usr_context, void *udata) {
    int                   ret     = 0;
    struct adlak_device * padlak  = (struct adlak_device *)g_adlak_dev;
    struct adlak_context *context = (struct adlak_context *)*usr_context;
    ASSERT(padlak);
    ASSERT(usr_context);
    ASSERT(udata);
    AML_LOG_DEBUG("%s", __func__);
    ret = adlak_os_mutex_lock(&padlak->dev_mutex);
    if (ret) {
        ret = ERR(EIO);
    } else {
        ret = adlak_net_register_request(context, (struct adlak_network_desc *)udata);
        if (ret) {
            ret = ERR(ENOMEM);
        }
        adlak_os_mutex_unlock(&padlak->dev_mutex);
    }
    return ret;
}

int adlak_dev_ioctl_destroy_network(const uintptr_t *usr_context, void *udata) {
    int                   ret     = 0;
    struct adlak_device * padlak  = (struct adlak_device *)g_adlak_dev;
    struct adlak_context *context = (struct adlak_context *)*usr_context;
    ASSERT(padlak);
    ASSERT(usr_context);
    ASSERT(udata);
    AML_LOG_DEBUG("%s", __func__);
    ret = adlak_os_mutex_lock(&padlak->dev_mutex);
    if (ret) {
        ret = ERR(EIO);
    } else {
        ret = adlak_net_unregister_request(context, (struct adlak_network_del_desc *)udata);
        if (ret) {
            ret = ERR(ENOMEM);
        }
        adlak_os_mutex_unlock(&padlak->dev_mutex);
    }
    return ret;
}

int adlak_dev_ioctl_invoke(const uintptr_t *usr_context, void *udata) {
    int                   ret     = 0;
    struct adlak_device * padlak  = (struct adlak_device *)g_adlak_dev;
    struct adlak_context *context = (struct adlak_context *)*usr_context;
    ASSERT(padlak);
    ASSERT(usr_context);
    ASSERT(udata);
    AML_LOG_DEBUG("%s", __func__);
    ret = adlak_os_mutex_lock(&padlak->dev_mutex);
    if (ret) {
        ret = ERR(EIO);
    } else {
        ret = adlak_invoke_request(context, (struct adlak_network_invoke_desc *)udata);
        if (ret) {
            ret = ERR(ENOMEM);
        }
        adlak_os_mutex_unlock(&padlak->dev_mutex);
    }
    return ret;
}

int adlak_dev_ioctl_invoke_cancel(const uintptr_t *usr_context, void *udata) {
    int                   ret     = 0;
    struct adlak_device * padlak  = (struct adlak_device *)g_adlak_dev;
    struct adlak_context *context = (struct adlak_context *)*usr_context;
    ASSERT(padlak);
    ASSERT(usr_context);
    ASSERT(udata);
    AML_LOG_DEBUG("%s", __func__);
    ret = adlak_os_mutex_lock(&padlak->dev_mutex);
    if (ret) {
        ret = ERR(EIO);
    } else {
        ret = adlak_uninvoke_request(context, (struct adlak_network_invoke_del_desc *)udata);
        if (ret) {
            ret = ERR(ENOMEM);
        }
        adlak_os_mutex_unlock(&padlak->dev_mutex);
    }
    return ret;
}

int adlak_dev_ioctl_get_stat(const uintptr_t *usr_context, void *udata) {
    int                   ret     = 0;
    struct adlak_device * padlak  = (struct adlak_device *)g_adlak_dev;
    struct adlak_context *context = (struct adlak_context *)*usr_context;
    ASSERT(padlak);
    ASSERT(usr_context);
    ASSERT(udata);
    AML_LOG_DEBUG("%s", __func__);
    ret = adlak_os_mutex_lock(&padlak->dev_mutex);
    if (ret) {
        ret = ERR(EIO);
    } else {
        ret = adlak_get_status_request(context, (struct adlak_get_stat_desc *)udata);
        if (ret) {
            ret = ERR(ENOMEM);
        }
        adlak_os_mutex_unlock(&padlak->dev_mutex);
    }
    return ret;
}
int adlak_dev_ioctl_profile_cfg(const uintptr_t *usr_context, void *udata) {
    int                   ret     = 0;
    struct adlak_device * padlak  = (struct adlak_device *)g_adlak_dev;
    struct adlak_context *context = (struct adlak_context *)*usr_context;
    ASSERT(padlak);
    ASSERT(usr_context);
    ASSERT(udata);
    AML_LOG_DEBUG("%s", __func__);
    ret = adlak_os_mutex_lock(&padlak->dev_mutex);
    if (ret) {
        ret = ERR(EIO);
    } else {
        ret = adlak_profile_config(context, (struct adlak_profile_cfg_desc *)udata);
        if (ret) {
            ret = ERR(ENOMEM);
        }
        adlak_os_mutex_unlock(&padlak->dev_mutex);
    }
    return ret;
}

int adlak_dev_wait_poll(const uintptr_t *usr_context, int timeout_ms) {
    int                   ret     = 0;
    struct adlak_device * padlak  = (struct adlak_device *)g_adlak_dev;
    struct adlak_context *context = (struct adlak_context *)*usr_context;
    ASSERT(padlak);
    ASSERT(usr_context);
    AML_LOG_DEBUG("%s", __func__);

    ret = adlak_to_umd_sinal_wait(context->wait, timeout_ms);

    if (ERR(NONE) == ret) {
        AML_LOG_DEBUG("%s wait no-error", __func__);
    } else {
        AML_LOG_DEBUG("%s wait error, error=%d", __func__, ret);
    }
    return ret;
}

int adlak_dev_ioctl_wait_until_finished(const uintptr_t *usr_context, void *udata) {
    int                   ret     = 0;
    struct adlak_device * padlak  = (struct adlak_device *)g_adlak_dev;
    struct adlak_context *context = (struct adlak_context *)*usr_context;
    ASSERT(padlak);
    ASSERT(usr_context);
    ASSERT(udata);
    AML_LOG_DEBUG("%s", __func__);
    ret = adlak_wait_until_finished(context, (struct adlak_get_stat_desc *)udata);
    return ret;
}
