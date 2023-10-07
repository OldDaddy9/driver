/*******************************************************************************
 * Copyright (C) 2021 Amlogic, Inc. All rights reserved.
 ******************************************************************************/

/*****************************************************************************/
/**
 *
 * @file adlak_log.h
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

#ifndef __ADLAK_LOG_H__
#define __ADLAK_LOG_H__

/***************************** Include Files *********************************/
#include "adlak_config.h"
#include "adlak_error.h"
#include "adlak_typedef.h"
#ifdef __cplusplus
extern "C" {
#endif

extern int g_adlak_log_level;
#if ADLAK_DEBUG
extern int g_adlak_log_level_pre;
#ifndef ADLA_LOG_COLOR
#define ADLA_LOG_COLOR
#endif
#endif
#include "adla_log_common.h"

#define LOGK_TAG_ERR LOG_COLOR_RED "[ADLAK ERROR] " LOG_COLOR_NULL
#define LOGK_TAG_WARN LOG_COLOR_YELLOW "[ADLAK WARN] " LOG_COLOR_NULL
#define LOGK_TAG_INFO LOG_COLOR_GREEN "[ADLAK INFO] " LOG_COLOR_NULL
#define LOGK_TAG_DEBUG "[ADLAK DEBUG] "
#define LOGK_TAG_DEFAUL

#ifndef ASSERT
#if ADLAK_DEBUG
#define ASSERT(x)                                                                            \
    do {                                                                                     \
        if (!(x)) {                                                                          \
            adlak_os_printf(LOGK_TAG_ERR "ASSERTION failed %s:%d: %s\n", __FILE__, __LINE__, \
                            __FUNCTION__);                                                   \
        }                                                                                    \
    } while (0)
#else
#define ASSERT(x) \
    do {          \
    } while (0)
#endif
#endif

#ifndef WARN_ON
#if 0  // ADLAK_DEBUG
#define WARN_ON(cond)                                                                          \
    do {                                                                                       \
        if ((cond)) {                                                                          \
            adlak_os_printf(LOGK_TAG_WARN "Internal warning(%s:%d, %s)\n", __FILE__, __LINE__, \
                            __FUNCTION__);                                                     \
        }                                                                                      \
    } while (0)
#else
#define WARN_ON(cond) \
    do {              \
    } while (0)
#endif
#endif
#if ADLAK_DEBUG
#define AML_KMD_LOG(LogLevel, LOG_TAG, FMT, ARGS...)                                             \
    do {                                                                                         \
        if (LogLevel <= g_adlak_log_level) {                                                     \
            if (LogLevel == LOG_LEVEL_ERR) {                                                     \
                adlak_os_printf(LOG_TAG "%s:%d:%s: " FMT "\n", __FILE__, __LINE__, __FUNCTION__, \
                                ##ARGS);                                                         \
            } else if (LogLevel == LOG_LEVEL_WARN) {                                             \
                adlak_os_printf(LOG_TAG "%s:%d:%s: " FMT "\n", __FILE__, __LINE__, __FUNCTION__, \
                                ##ARGS);                                                         \
            } else if (LogLevel == LOG_LEVEL_INFO) {                                             \
                adlak_os_printf(LOG_TAG FMT "\n", ##ARGS);                                       \
            } else if (LogLevel == LOG_LEVEL_DEBUG) {                                            \
                adlak_os_printf(LOG_TAG FMT "\n", ##ARGS);                                       \
            } else if (LogLevel == LOG_LEVEL_DEFAULT) {                                          \
                adlak_os_printf(FMT, ##ARGS);                                                    \
            }                                                                                    \
        }                                                                                        \
    } while (0)

#define AML_LOG_ERR(FMT, ARGS...) AML_KMD_LOG(LOG_LEVEL_ERR, LOGK_TAG_ERR, FMT, ##ARGS)
#define AML_LOG_WARN(FMT, ARGS...) AML_KMD_LOG(LOG_LEVEL_WARN, LOGK_TAG_WARN, FMT, ##ARGS)
#define AML_LOG_INFO(FMT, ARGS...) AML_KMD_LOG(LOG_LEVEL_INFO, LOGK_TAG_INFO, FMT, ##ARGS)
#define AML_LOG_DEBUG(FMT, ARGS...) AML_KMD_LOG(LOG_LEVEL_DEBUG, LOGK_TAG_DEBUG, FMT, ##ARGS)
#define AML_LOG_DEFAULT(FMT, ARGS...) AML_KMD_LOG(LOG_LEVEL_DEFAULT, LOGK_TAG_DEFAUL, FMT, ##ARGS)
#else

#define AML_KMD_LOG(LogLevel, LOG_TAG, FMT, ARGS...)                            \
    do {                                                                        \
        if (LogLevel <= g_adlak_log_level) {                                    \
            if (LogLevel == LOG_LEVEL_ERR) {                                    \
                adlak_os_printf(LOG_TAG "%s: " FMT "\n", __FUNCTION__, ##ARGS); \
            } else if (LogLevel == LOG_LEVEL_WARN) {                            \
                adlak_os_printf(LOG_TAG "%s: " FMT "\n", __FUNCTION__, ##ARGS); \
            }                                                                   \
        }                                                                       \
    } while (0)

#define AML_LOG_ERR(FMT, ARGS...) AML_KMD_LOG(LOG_LEVEL_ERR, LOGK_TAG_ERR, FMT, ##ARGS)
#define AML_LOG_WARN(FMT, ARGS...) AML_KMD_LOG(LOG_LEVEL_WARN, LOGK_TAG_WARN, FMT, ##ARGS)
#define AML_LOG_INFO(FMT, ARGS...)
#define AML_LOG_DEBUG(FMT, ARGS...)
#define AML_LOG_DEFAULT(FMT, ARGS...)
#endif

/************************** Function Prototypes ******************************/

#ifdef __cplusplus
}
#endif

#endif /* __ADLAK_LOG_H__ end define*/
