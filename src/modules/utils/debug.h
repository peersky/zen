/*
 MIT License

 Copyright (c) 2021 Tim Pechersky

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 In this header you can write any custom lirary related debug functions.
 debugProvider.h must be defined in target files and must provide folliowing:
	inline void setDebugAnalyser(float *ch1, float *ch2, size_t size)
	inline void send_to_analyser(size_t channel, float *buffer)
	inline void append_to_analyser(size_t channel, float sample)
	inline void Error_Handler_(const char *file, int line)

 */

#pragma once

// #include <debugProvider.hpp>

/*
    This contains helper functions used across app/bsp/libraries to debug and test aplication
*/

#define USE_DEVLIB_ASSERT 1U
#include "stdint.h"
#include "stdio.h"
#include "assert.h"
#define USE_DEVLIB_PRINT 1U

inline static void Error_Handler_(const char *file, int line)
{
	printf("Error: file %s on line %d\r\n", file, line);
}

inline char Debug_getchar(void)
{
	char input;
	scanf(" %c", &input);

	return input;
}

inline uint32_t Debug_gethex(void)
{
	int input;
	scanf(" %x", &input);

	return input;
}

#define PRINT_LVL1(fmt, ...)                   \
	do                                         \
	{                                          \
		if (USE_DEVLIB_PRINT && PRINT_LVL1_ON) \
		{                                      \
			printf("LVL1:");                   \
			printf(fmt, __VA_ARGS__);          \
			fflush(stdout);                    \
		}                                      \
	} while (0)

#define PRINT_LVL2(fmt, ...)                   \
	do                                         \
	{                                          \
		if (USE_DEVLIB_PRINT && PRINT_LVL2_ON) \
		{                                      \
			printf("LVL2:");                   \
			printf(fmt, __VA_ARGS__);          \
			fflush(stdout);                    \
		}                                      \
	} while (0)

#define PRINT_LVL3(fmt, ...)                   \
	do                                         \
	{                                          \
		if (USE_DEVLIB_PRINT && PRINT_LVL3_ON) \
		{                                      \
			printf("LVL3:");                   \
			printf(fmt, __VA_ARGS__);          \
			fflush(stdout);                    \
		}                                      \
	} while (0)

#define ERROR_HANDLER()                         \
	do                                          \
	{                                           \
		if (USE_DEVLIB_PRINT)                   \
		{                                       \
			Error_Handler_(__FILE__, __LINE__); \
		}                                       \
	} while (0)

#define _ASSERT 0U
#ifdef _ASSERT

#define assert_param(expr) ((expr) ? (void)0U : assert_failed((uint8_t *)__FILE__, __LINE__))
void assert_failed(uint8_t *file, uint32_t line);
#else
#define assert_param(expr) ((void)0U)
#endif /* USE_FULL_ASSERT */

#define ZEN_ERROR_HANDLER() ERROR_HANDLER()
