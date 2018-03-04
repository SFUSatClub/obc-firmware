///////////////////////////////////////////////////////////////////////////////
// \author (c) Marco Paland (info@paland.com)
//             2014-2017, PALANDesign Hannover, Germany
//
// \license The MIT License (MIT)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// \brief Tiny printf, sprintf and snprintf implementation, optimized for speed on
//        embedded systems with a very limited resources.
//        Use this instead of bloated standard/newlib printf.
//        These routines are thread safe and reentrant!
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _PRINTF_H_
#define _PRINTF_H_

#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif

// buffer size used for printf (created on stack)
#define PRINTF_BUFFER_SIZE    128U

// ntoa conversion buffer size, this must be big enough to hold one converted numeric number (created on stack)
#define NTOA_BUFFER_SIZE      32U

// ftoa conversion buffer size, this must be big enough to hold one converted float number (created on stack)
#define FTOA_BUFFER_SIZE      32U

// define this to support floating point (%f)
//#define PRINTF_FLOAT_SUPPORT 0

// define this to support long long types (%llu or %p)
//#define PRINTF_LONG_LONG_SUPPORT 0

///////////////////////////////////////////////////////////////////////////////

// internal flag definitions
#define FLAGS_ZEROPAD   (1U << 0U)
#define FLAGS_LEFT      (1U << 1U)
#define FLAGS_PLUS      (1U << 2U)
#define FLAGS_SPACE     (1U << 3U)
#define FLAGS_HASH      (1U << 4U)
#define FLAGS_UPPERCASE (1U << 5U)
#define FLAGS_LONG      (1U << 6U)
#define FLAGS_LONG_LONG (1U << 7U)
#define FLAGS_PRECISION (1U << 8U)
#define FLAGS_WIDTH     (1U << 9U)



/**
 * Output a character to a custom device like UART.
 * This function is declared here only. You have to write your custom implementation somewhere.
 * \param character to output
 */
void sfu_putchar(char character);

#define size_t uint32_t

/**
 * Tiny printf implementation
 * You have to implement _putchar if you use printf()
 * \param format A string that specifies the format of the output
 * \return The number of characters that are written into the array, not counting the terminating null character
 *
 * NOTE:
 * This was modified by SFUSat to send characters directly to the UART (non-RTOS operation) and to the queue (for RTOS mode)
 */
int printf(const char* format, ...);


/**
 * Tiny sprintf implementation
 * Due to security reasons YOU SHOULD CONSIDER USING SNPRINTF INSTEAD!
 * \param buffer A pointer to the buffer where to store the formatted string
 * \param format A string that specifies the format of the output
 * \return The number of characters that are written into the array, not counting the terminating null character
 */
int sprintf(char* buffer, const char* format, ...);


/**
 * Tiny snprintf implementation
 * \param buffer A pointer to the buffer where to store the formatted string
 * \param count The maximum number of characters to store in the buffer, including a terminating null character
 * \param format A string that specifies the format of the output
 * \return The number of characters that are written into the array, not counting the terminating null character
 */
int snprintf(char* buffer, size_t count, const char* format, ...);


#ifdef __cplusplus
}
#endif


#endif  // _PRINTF_H_
