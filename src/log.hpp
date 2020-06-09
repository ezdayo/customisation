/**
 *
 * @file      src/log.hpp
 *
 * @brief     This is the definition of logging functions for debugging
 *
 * @author    Olivier Stoltz-Douchet <ezdayo@gmail.com>
 *
 * @copyright (c) 2019-2020 Olivier Stoltz-Douchet
 * @license   http://opensource.org/licenses/MIT MIT
 * @link      https://github.com/ezdayo/customisation
 *
 **/

#pragma once

#ifndef LOGTAG
#pragma message("Define LOGTAG to match the name of your application!!!")
#define LOGTAG "my-app"
#endif

#ifdef __ANDROID__          /* On Android */
#include <android/log.h>

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOGTAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,  LOGTAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  LOGTAG, __VA_ARGS__)

#else                       /* On Linux */
#include <cstdio>

#define LOGE(...) fprintf(stderr, "[E] " LOGTAG ": " __VA_ARGS__); \
                  fprintf(stderr, "\n")
#define LOGW(...) fprintf(stdout, "[W] " LOGTAG ": " __VA_ARGS__); \
                  fprintf(stdout, "\n")
#define LOGI(...) fprintf(stdout, "[I] " LOGTAG ": " __VA_ARGS__); \
                  fprintf(stdout, "\n")

#endif

#include <cassert>

#ifdef NDEBUG
#define LOGD(...) ((void)0)
#define LOGD_IF_NOT(condition, ...) ((void)0)
#define ASSERT(condition, ...) ((void)0)
#else
#define LOGD(...) LOGW(__VA_ARGS__)
#define LOGD_IF_NOT(condition, ...) \
    if ((!(condition))) { LOGD(__VA_ARGS__); }
#define ASSERT(condition, ...) \
    if ((!(condition))) { LOGE(__VA_ARGS__); }; assert(condition)
#endif
