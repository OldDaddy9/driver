/*******************************************************************************
 * Copyright (C) 2022 Amlogic, Inc. All rights reserved.
 ******************************************************************************/

/*****************************************************************************/
/**
 *
 * @file adlak_os.c
 * @brief
 *
 * <pre>
 * MODIFICATION HISTORY:
 *
 * Ver   	Who				Date				Changes
 * ----------------------------------------------------------------------------
 * 1.00a shiwei.sun@amlogic.com	2022/04/08	Initial release
 * </pre>
 *
 ******************************************************************************/

/***************************** Include Files *********************************/

#include "adlak_os.h"

#include <stdarg.h>

#include "adlak_common.h"
#include "adlak_error.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/
// #define PRINT_FUNC_NAME
#define PRINT_FUNC_NAME AML_LOG_DEBUG("%s \n", __func__)
/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/
#if ADLAK_DEBUG
static int32_t dbg_mem_alloc_count_kmd = 0;
#endif
/************************** Function Prototypes ******************************/
void *adlak_os_malloc(size_t size, uint32_t flag) {
#if ADLAK_DEBUG
    void *addr = NULL;
    dbg_mem_alloc_count_kmd += 1;
    addr = pvPortMalloc(size);
    AML_LOG_DEBUG("alloc: dbg_mem_alloc_count_kmd = %d, addr = %p\n", dbg_mem_alloc_count_kmd,
                  addr);
    return addr;
#else
    return pvPortMalloc(size);
#endif
}
void *adlak_os_zalloc(size_t size, uint32_t flag) {
    void *addr = NULL;
    addr       = adlak_os_malloc(size, flag);
    if (addr) {
        memset(addr, 0, size);
    }
    return addr;
}

void *adlak_os_calloc(size_t num, size_t size, uint32_t flag) {
    return adlak_os_zalloc(size * num, flag);
}

void adlak_os_free(void *ptr) {
#if ADLAK_DEBUG
    dbg_mem_alloc_count_kmd -= 1;
    AML_LOG_DEBUG("free: dbg_mem_alloc_count_kmd = %d, addr = %p\n", dbg_mem_alloc_count_kmd, ptr);
#endif
    vPortFree(ptr);
}

void *adlak_os_vmalloc(size_t size, uint32_t flag) { return adlak_os_malloc(size, flag); }

void *adlak_os_vzalloc(size_t size, uint32_t flag) { return adlak_os_zalloc(size, flag); }

void adlak_os_vfree(void *ptr) { adlak_os_free(ptr); }

void *adlak_os_memset(void *dest, int ch, size_t count) { return memset(dest, ch, count); }

void *adlak_os_memcpy(void *dest, const void *src, size_t count) {
    return memcpy(dest, src, count);
}

int adlak_os_memcmp(const void *lhs, const void *rhs, size_t count) {
    return memcmp(lhs, rhs, count);
}

int adlak_os_printf(const char *format, ...) {
    va_list args;
    int     r;
    va_start(args, format);
    r = vprintf(format, args);
    va_end(args);
    return r;
}

int adlak_os_snprintf(char *buffer, ssize_t size, const char *format, ...) {
    int     cnt;
    va_list arg;
    if (size <= 0) {
        return 0;
    }
    va_start(arg, format);
    cnt = vsnprintf(buffer, size, format, arg);
    va_end(arg);
    return cnt;
}

void      adlak_os_msleep(unsigned int ms) { vTaskDelay(pdMS_TO_TICKS(ms)); }
uintptr_t adlak_os_msecs_to_jiffies(uintptr_t ms) { return pdMS_TO_TICKS(ms); }

void adlak_os_udelay(unsigned int usecs) {
    unsigned int ticks = (usecs * configTICK_RATE_HZ + 999999) / 1000000;
    vTaskDelay(ticks);
}

// Mutex
typedef struct adlak_os_mutex_inner {
    SemaphoreHandle_t mutex_hd;
} adlak_os_mutex_inner_t;

int adlak_os_mutex_init(OUT adlak_os_mutex_t *mutex) {
    adlak_os_mutex_inner_t *pmutex_inner = NULL;
    PRINT_FUNC_NAME;
    pmutex_inner = (adlak_os_mutex_inner_t *)adlak_os_malloc(sizeof(adlak_os_mutex_inner_t), 0);
    if (ADLAK_IS_ERR_OR_NULL(pmutex_inner)) {
        AML_LOG_ERR("invalid params!\n");
        return ERR(ENOMEM);
    }
    pmutex_inner->mutex_hd = xSemaphoreCreateMutex();
    if (pmutex_inner->mutex_hd) {
        *mutex = pmutex_inner;
        AML_LOG_DEBUG("mutex_init success!\n");
        return ERR(NONE);
    } else {
        adlak_os_free(pmutex_inner);
        AML_LOG_ERR("mutex_init fail!\n");
        return ERR(ENXIO);
    }
}

int adlak_os_mutex_destroy(adlak_os_mutex_t *mutex) {
    adlak_os_mutex_inner_t *pmutex_inner = (adlak_os_mutex_inner_t *)*mutex;
    PRINT_FUNC_NAME;
    if (pmutex_inner && pmutex_inner->mutex_hd) {
        vSemaphoreDelete(pmutex_inner->mutex_hd);
        pmutex_inner->mutex_hd = NULL;
        adlak_os_free((void *)pmutex_inner);
        *mutex = NULL;
        AML_LOG_DEBUG("mutex_destroy success!\n");
        return ERR(NONE);
    }

    AML_LOG_ERR("invalid params!\n");
    return ERR(EPERM);
}

int adlak_os_mutex_lock(adlak_os_mutex_t *mutex) {
    int                     ret;
    adlak_os_mutex_inner_t *pmutex_inner = (adlak_os_mutex_inner_t *)*mutex;
    PRINT_FUNC_NAME;
    if (pmutex_inner && pmutex_inner->mutex_hd) {
        do {
            ret = xSemaphoreTake(pmutex_inner->mutex_hd, portMAX_DELAY);
            if (ret != pdPASS) {
                AML_LOG_DEBUG("mutex_lock timeout...!\n");
            }
        } while (ret != pdPASS);
        return ERR(NONE);
    }

    AML_LOG_ERR("invalid params!\n");
    return ERR(EPERM);
}

int adlak_os_mutex_unlock(adlak_os_mutex_t *mutex) {
    adlak_os_mutex_inner_t *pmutex_inner = (adlak_os_mutex_inner_t *)*mutex;
    PRINT_FUNC_NAME;
    if (pmutex_inner && pmutex_inner->mutex_hd) {
        if (xSemaphoreGive(pmutex_inner->mutex_hd) != pdTRUE) {
            return ADLAU_EIO;
        }
        return ERR(NONE);
    }

    AML_LOG_ERR("invalid params!\n");
    return ERR(EPERM);
}

int adlak_os_mutex_lock_from_isr(adlak_os_mutex_t *mutex) {
    adlak_os_mutex_inner_t *pmutex_inner             = (adlak_os_mutex_inner_t *)*mutex;
    BaseType_t              xHigherPriorityTaskWoken = pdFALSE;
    PRINT_FUNC_NAME;
    if (pmutex_inner && pmutex_inner->mutex_hd) {
        xSemaphoreTakeFromISR(pmutex_inner->mutex_hd, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        return ERR(NONE);
    }
    return ERR(EPERM);
}

int adlak_os_mutex_unlock_from_isr(adlak_os_mutex_t *mutex) {
    adlak_os_mutex_inner_t *pmutex_inner             = (adlak_os_mutex_inner_t *)*mutex;
    BaseType_t              xHigherPriorityTaskWoken = pdFALSE;
    PRINT_FUNC_NAME;
    if (pmutex_inner && pmutex_inner->mutex_hd) {
        xSemaphoreGiveFromISR(pmutex_inner->mutex_hd, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        return ERR(NONE);
    }
    return ERR(EPERM);
}

typedef adlak_os_mutex_t adlak_os_spinlock_t;
int adlak_os_spinlock_init(adlak_os_spinlock_t *spinlock) { return adlak_os_mutex_init(spinlock); }

int adlak_os_spinlock_destroy(adlak_os_spinlock_t *spinlock) {
    return adlak_os_mutex_destroy(spinlock);
}

int adlak_os_spinlock_lock(adlak_os_spinlock_t *spinlock) {
    return adlak_os_mutex_lock_from_isr(spinlock);
}

int adlak_os_spinlock_unlock(adlak_os_spinlock_t *spinlock) {
    return adlak_os_mutex_unlock_from_isr(spinlock);
}
// Semaphore
typedef struct adlak_os_sema_inner {
    SemaphoreHandle_t sema_hd;
} adlak_os_sema_inner_t;

int adlak_os_sema_init(OUT adlak_os_sema_t *sem, unsigned int max_count, unsigned int init_count) {
    adlak_os_sema_inner_t *psema_inner = NULL;
    psema_inner = (adlak_os_sema_inner_t *)adlak_os_malloc(sizeof(adlak_os_sema_inner_t), 0);
    if (ADLAK_IS_ERR_OR_NULL(psema_inner)) {
        return ERR(ENOMEM);
    }
    PRINT_FUNC_NAME;
    if (max_count == 1) {
        psema_inner->sema_hd = xSemaphoreCreateBinary();
    } else {
        psema_inner->sema_hd = xSemaphoreCreateCounting(max_count, init_count);
    }
    if (psema_inner->sema_hd != NULL) {
        *sem = psema_inner;
        AML_LOG_DEBUG("sema_init success!\n");
        return ERR(NONE);
    } else {
        adlak_os_free(psema_inner);
        *sem = NULL;
        AML_LOG_ERR("sema_init fail!\n");
        return ERR(ENXIO);
    }
}

int adlak_os_sema_destroy(adlak_os_sema_t *sem) {
    adlak_os_sema_inner_t *psema_inner = (adlak_os_sema_inner_t *)*sem;
    PRINT_FUNC_NAME;
    if (psema_inner && psema_inner->sema_hd) {
        vSemaphoreDelete(psema_inner->sema_hd);
        psema_inner->sema_hd = NULL;
        adlak_os_free((void *)psema_inner);
        *sem = NULL;
        AML_LOG_DEBUG("sema_destroy success!\n");
        return ERR(NONE);
    }
    AML_LOG_ERR("invalid params!\n");
    return ERR(EPERM);
}

int adlak_os_sema_take(adlak_os_sema_t sem) {
    int                    ret;
    adlak_os_sema_inner_t *psema_inner = (adlak_os_sema_inner_t *)sem;
    PRINT_FUNC_NAME;
    if (psema_inner && psema_inner->sema_hd) {
        do {
            ret = xSemaphoreTake(psema_inner->sema_hd, portMAX_DELAY);
            if (ret != pdPASS) {
                AML_LOG_DEBUG("sema_take timeout...!\n");
            }
        } while (ret != pdTRUE);
        return ERR(NONE);
    }
    AML_LOG_ERR("invalid params!\n");
    return ERR(EPERM);
}
int adlak_os_sema_take_timeout(adlak_os_sema_t sem, unsigned int ms) {
    adlak_os_sema_inner_t *psema_inner = (adlak_os_sema_inner_t *)sem;
    long                   jiffies;
    PRINT_FUNC_NAME;
    if (psema_inner != NULL) {
        jiffies = adlak_os_msecs_to_jiffies(ms);
        if (xSemaphoreTake(psema_inner->sema_hd, jiffies) == pdTRUE) {
            return ERR(NONE);
        } else {
            AML_LOG_DEBUG("Failed to acquire semaphore[%d ms]", ms);
            return ERR(EINTR);
        }
    }
    AML_LOG_ERR("invalid params!\n");
    return ERR(EINVAL);
}
int adlak_os_sema_give(adlak_os_sema_t sem) {
    adlak_os_sema_inner_t *psema_inner = (adlak_os_sema_inner_t *)sem;
    PRINT_FUNC_NAME;
    if (psema_inner && psema_inner->sema_hd) {
        if (xSemaphoreGive(psema_inner->sema_hd) != pdTRUE) {
            return ADLAU_EIO;
        }
        return ERR(NONE);
    }
    AML_LOG_ERR("invalid params!\n");
    return ERR(EPERM);
}

int adlak_os_sema_give_from_isr(adlak_os_sema_t sem) {
    adlak_os_sema_inner_t *psema_inner = (adlak_os_sema_inner_t *)sem;
    PRINT_FUNC_NAME;
    if (psema_inner && psema_inner->sema_hd) {
        if (xSemaphoreGiveFromISR(psema_inner->sema_hd, pdFALSE) != pdTRUE) {
            return ADLAU_EIO;
        }
        return ERR(NONE);
    }
    AML_LOG_ERR("invalid params!\n");
    return ERR(EPERM);
}

#include "./FreeRTOS_POSIX_pthread.c"
typedef struct adlak_os_thread_inner {
    pthread_t pthread;
} adlak_os_thread_inner_t;
int adlak_os_thread_create(adlak_os_thread_t *pthrd, adlak_thread_cb_func_t func, void *arg) {
    int                      ret;
    adlak_os_thread_inner_t *pthread_inner = NULL;
    PRINT_FUNC_NAME;
    pthread_inner = (adlak_os_thread_inner_t *)adlak_os_malloc(sizeof(adlak_os_thread_inner_t), 0);
    if (ADLAK_IS_ERR_OR_NULL(pthread_inner)) {
        AML_LOG_ERR("alloc pthread_inner fail!\n");
        return ERR(ENOMEM);
    }

    pthrd->thrd_should_stop = 0;
    ret                     = pthread_create(&pthread_inner->pthread, 0, func, arg);
    if (!ret) {
        pthrd->handle = (void *)pthread_inner;
        AML_LOG_DEBUG("thread create success!\n");
        return ERR(NONE);
    } else {
        adlak_os_free(pthread_inner);
        pthrd->handle = NULL;
        AML_LOG_ERR("thread create fail!\n");
        return ERR(ENXIO);
    }
}

int adlak_os_thread_join(adlak_os_thread_t *pthrd) {
    int                      ret;
    adlak_os_thread_inner_t *pthread_inner = (adlak_os_thread_inner_t *)pthrd->handle;
    PRINT_FUNC_NAME;
    if (pthread_inner) {
        pthrd->thrd_should_stop = 1;
        ret                     = pthread_join(pthread_inner->pthread, 0);
        if (ret) {
            AML_LOG_ERR("pthread_join fail!\n");
        } else {
            AML_LOG_DEBUG("pthread_join success!\n");
        }
        adlak_os_free(pthread_inner);
        pthrd->handle = NULL;
        return ret;
    }
    return ERR(EPERM);
}

int adlak_os_thread_detach(adlak_os_thread_t *pthrd, void (*thread_finalize)(void *), void *arg) {
    int                      ret;
    adlak_os_thread_inner_t *pthread_inner = (adlak_os_thread_inner_t *)pthrd->handle;
    PRINT_FUNC_NAME;
    if (pthread_inner) {
        pthrd->thrd_should_stop = 1;
        if (thread_finalize) {
            thread_finalize(arg);
        }
        while (pthrd->thrd_should_stop) {
            adlak_os_msleep(3);
        }
        ret = pthread_detach(pthread_inner->pthread);
        if (ret) {
            AML_LOG_ERR("pthread_detach fail!\n");
        } else {
            AML_LOG_DEBUG("pthread_detach success!\n");
        }
        adlak_os_free(pthread_inner);
        pthrd->handle = NULL;
        return ret;
    }
    AML_LOG_ERR("invalid params!\n");
    return ERR(EPERM);
}

void adlak_os_thread_yield() {
    PRINT_FUNC_NAME;
    sched_yield();
}

typedef struct adlak_os_timer_inner {
    // struct timer_list timer;
    // unsigned long flags;

    TimerHandle_t timer;
    /*!< handle of timer*/
    unsigned int ms;
    /*!< Reserve */
    unsigned int autoreload;
    /*!< If uxAutoReload is set to pdTRUE, then the timer will expire repeatedly with a frequency
     * set by the xTimerPeriod parameter*/
    TimerCallbackFunction_t entry;
    /*!< callback*/
    void *param;
    /*!< callback param*/
} adlak_os_timer_inner_t;

static void adlak_os_timer_callback(TimerHandle_t handle) {
    struct adlak_os_timer_inner *ptimer_inner =
        (struct adlak_os_timer_inner *)pvTimerGetTimerID(handle);
    PRINT_FUNC_NAME;
    if (ptimer_inner->entry) {
        ptimer_inner->entry(ptimer_inner->param);
    }
}
int adlak_os_timer_init(adlak_os_timer_t *ptim, adlak_timer_cb_func_t func, void *param) {
    adlak_os_timer_inner_t *ptimer_inner = NULL;
    PRINT_FUNC_NAME;
    ptimer_inner =
        (adlak_os_timer_inner_t *)adlak_os_zalloc(sizeof(adlak_os_timer_inner_t), ADLAK_GFP_KERNEL);
    if (ADLAK_IS_ERR_OR_NULL(ptimer_inner)) {
        return ERR(ENOMEM);
    }
    ptimer_inner->entry      = (TimerCallbackFunction_t)func;
    ptimer_inner->param      = param;
    ptimer_inner->autoreload = pdFALSE;                     /* This is a one-shot timer. */
    ptimer_inner->timer      = xTimerCreate(NULL,           // TimerName
                                       portMAX_DELAY,  // xTimerPeriodInTicks
                                       ptimer_inner->autoreload,  // uxAutoReload
                                       ptimer_inner,              // timer unique id
                                       (TimerCallbackFunction_t)adlak_os_timer_callback);
    if (NULL == ptimer_inner->timer) {
        /* The timer was not created. */
        adlak_os_free((void *)ptimer_inner);
        *ptim = NULL;
        AML_LOG_ERR("The timer was not created!\n");

        return ERR(EINVAL);
    } else {
        *ptim = ptimer_inner;
        AML_LOG_DEBUG("timer_init success!\n");
        return ERR(NONE);
    }
    // timer_setup(&ptimer_inner->timer, (void (*)(struct timer_list *))func, 0);
}

int adlak_os_timer_destroy(adlak_os_timer_t *ptim) {
    adlak_os_timer_inner_t *ptimer_inner = (adlak_os_timer_inner_t *)*ptim;
    PRINT_FUNC_NAME;
    if (ptimer_inner != NULL) {
        if (ERR(NONE) != adlak_os_timer_del(ptim)) {
            return ERR(EINVAL);
        }
        if (xTimerIsTimerActive(ptimer_inner->timer) != pdFALSE) {
            /* xTimer is already active - delete it. */
            xTimerDelete(ptimer_inner->timer, 0);
        }
        adlak_os_free((void *)ptimer_inner);
        *ptim = NULL;
        AML_LOG_DEBUG("timer_destroy success!\n");
        return ERR(NONE);
    }
    return ERR(EINVAL);
}

int adlak_os_timer_del(adlak_os_timer_t *ptim) {
    adlak_os_timer_inner_t *ptimer_inner = (adlak_os_timer_inner_t *)*ptim;
    PRINT_FUNC_NAME;
    if (ptimer_inner != NULL) {
        if (pdPASS == xTimerStop(ptimer_inner->timer, 0)) {
            AML_LOG_DEBUG("timer_del success!\n");
            return ERR(NONE);
        } else {
            return ERR(EINVAL);
        }
    }
    return ERR(EINVAL);
}

int adlak_os_timer_add(adlak_os_timer_t *ptim, unsigned int timeout_ms) {
    adlak_os_timer_inner_t *ptimer_inner = (adlak_os_timer_inner_t *)*ptim;
    // unsigned long           expire;
    PRINT_FUNC_NAME;

    if (ptimer_inner != NULL) {
        if (pdPASS == xTimerChangePeriod(ptimer_inner->timer, pdMS_TO_TICKS(timeout_ms), 0)) {
            return ERR(NONE);
        } else {
            return ERR(EINVAL);
        }
    }
    return ERR(EINVAL);
}

int adlak_os_timer_modify(adlak_os_timer_t *ptim, unsigned int timeout_ms) {
    return adlak_os_timer_add(ptim, timeout_ms);
}

typedef struct {
    uint64_t        rsv;
    adlak_os_sema_t sema;
} wait_queue_head_t;

/**
 * @brief task wait handle init
 *
 * @param hd
 * @return int
 */
int adlak_to_umd_sinal_init(uintptr_t *hd) {
    wait_queue_head_t *wait_inner = NULL;
    PRINT_FUNC_NAME;
    wait_inner = (wait_queue_head_t *)adlak_os_malloc(sizeof(wait_queue_head_t), ADLAK_GFP_KERNEL);
    if (ADLAK_IS_ERR_OR_NULL(wait_inner)) {
        return ERR(ENOMEM);
    }

    adlak_os_sema_init(&wait_inner->sema, 1, 0);
    // init_waitqueue_head(wait_inner);
    *hd = (uintptr_t)wait_inner;
    AML_LOG_DEBUG("adlak_to_umd_sinal_init success!\n");
    return ERR(NONE);
}

/**
 * @brief task wait handle destroy
 *
 * @param hd
 * @return int
 */
int adlak_to_umd_sinal_deinit(uintptr_t *hd) {
    wait_queue_head_t *wait_inner = (wait_queue_head_t *)*hd;
    PRINT_FUNC_NAME;
    if (wait_inner != NULL) {
        adlak_os_sema_destroy(&wait_inner->sema);
        adlak_os_free((void *)wait_inner);
        *hd = (uintptr_t)NULL;
        AML_LOG_DEBUG("adlak_to_umd_sinal_deinit success!\n");
        return ERR(NONE);
    }
    return ERR(EINVAL);
}

int adlak_to_umd_sinal_wait(uintptr_t hd, unsigned int timeout_ms) {
    wait_queue_head_t *wait_inner = (wait_queue_head_t *)hd;
    PRINT_FUNC_NAME;
    if (wait_inner != NULL) {
        if (ERR(EINTR) == adlak_os_sema_take_timeout(wait_inner->sema, timeout_ms)) {
            AML_LOG_WARN("umd_sinal_wait timeout!\n");
            return ERR(EINTR);
        } else {
            return ERR(NONE);
        }
    }
    return ERR(EINVAL);
}

int adlak_to_umd_sinal_give(uintptr_t hd) {
    wait_queue_head_t *wait_inner = (wait_queue_head_t *)hd;
    PRINT_FUNC_NAME;
    if (wait_inner != NULL) {
        adlak_os_sema_give(wait_inner->sema);
        return ERR(NONE);
    }
    return ERR(EINVAL);
}

adlak_os_ktime_t       adlak_os_ktime_get(void) { return (adlak_os_ktime_t)xTaskGetTickCount(); }
static inline intptr_t ktime_divns(const adlak_os_ktime_t kt, intptr_t div) {
    /*
     * 32-bit implementation cannot handle negative divisors,
     * so catch them on 64bit as well.
     */
    WARN_ON(div < 0);
    return kt / div;
}
/* Subtract two ktime_t variables. rem = lhs -rhs: */
#define ktime_sub(lhs, rhs) ((lhs) - (rhs))

/* Add two ktime_t variables. res = lhs + rhs: */
#define ktime_add(lhs, rhs) ((lhs) + (rhs))
#define NSEC_PER_USEC 1000L
#define NSEC_PER_MSEC 1000000L
static inline intptr_t ktime_to_us(const adlak_os_ktime_t kt) {
    return ktime_divns(kt, NSEC_PER_USEC);
}

static inline intptr_t ktime_to_ms(const adlak_os_ktime_t kt) {
    return ktime_divns(kt, NSEC_PER_MSEC);
}

uintptr_t adlak_os_ktime_us_delta(const adlak_os_ktime_t later, const adlak_os_ktime_t earlier) {
    return ktime_to_us(ktime_sub(later, earlier));
}

uintptr_t adlak_os_ktime_ms_delta(const adlak_os_ktime_t later, const adlak_os_ktime_t earlier) {
    return ktime_to_ms(ktime_sub(later, earlier));
}
