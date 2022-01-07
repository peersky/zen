/*
    This contains helper functions used across app/bsp/libraries to debug and test aplication
*/

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "stdint.h"

// static inline void assert_failed(uint8_t *file, uint32_t line);
#ifdef USE_DEVLIB_ASSERT
#define DEVLIB_ASSERT_PARAM(expr) ((expr) ? (void)0U : assert_failed((uint8_t *)__FILE__, __LINE__))
    void assert_failed(uint8_t *file, uint32_t line);
#else
#define DEVLIB_ASSERT_PARAM(expr) ((void)0U)
#endif /* USE_FULL_ASSERT */

#ifdef __cplusplus
}
#endif