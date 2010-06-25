/*
 * Copyright (c) 2010, Andras Varga and Opensim Ltd.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Opensim Ltd. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Andras Varga or Opensim Ltd. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _COMMONUTIL_H_
#define _COMMONUTIL_H_

#include <assert.h>
#include "commondefs.h"
#include "exception.h"
#include "intxtypes.h"


#ifdef NDEBUG
#  define Assert(x)
#  define DBG(x)
#else
#  define Assert(expr)  ((void) ((expr) ? 0 : (throw opp_runtime_error("ASSERT: condition %s false, %s line %d", \
                        #expr, __FILE__, __LINE__), 0)))
//#  define DBG(x)  printf x
#  define DBG(x)
#endif


COMMON_API extern const double NaN;
COMMON_API extern const double POSITIVE_INFINITY;
COMMON_API extern const double NEGATIVE_INFINITY;

inline bool isNaN(double d) { return d != d;}
inline bool isPositiveInfinity(double d) { return d==POSITIVE_INFINITY; }
inline bool isNegativeInfinity(double d) { return d==NEGATIVE_INFINITY; }


#ifdef _MSC_VER
#ifndef vsnprintf
#define vsnprintf _vsnprintf
#endif
#endif


#define VSNPRINTF(buffer, buflen, formatarg) \
    VSNPRINTF2(buffer, buflen, formatarg, formatarg)

#define VSNPRINTF2(buffer, buflen, lastarg, format) \
    va_list va; \
    va_start(va, lastarg); \
    vsnprintf(buffer, buflen, format, va); \
    buffer[buflen-1] = '\0'; \
    va_end(va);

/**
 * Sets locale to Posix ("C"). Needed because we want to read/write real numbers
 * with "." as decimal separator always (and not "," used by some locales).
 * This affects sprintf(), strtod(), etc.
 */
COMMON_API void setPosixLocale();

/**
 * Debugging aid: prints a message on entering/leaving methods; message
 * gets indented according to call depth. See TRACE macro.
 */
class COMMON_API DebugCall
{
  private:
    static int depth;
    std::string funcname;
  public:
    DebugCall(const char *fmt,...);
    ~DebugCall();
};

#define TRACE  DebugCall __x

/**
 * Performs the RDTSC (read time stamp counter) x86 instruction, and returns
 * the resulting high-resolution 64-bit timer value. This can be used for
 * ad-hoc performance measurements on Windows (this function returns 0 on
 * other platforms).
 *
 * See http://en.wikipedia.org/wiki/RDTSC
 */
uint64 readCPUTimeStampCounter();


/**
 * Not all our bison/flex based parsers are reentrant. This macro is meant
 * to catch and report concurrent invocations, e.g. from background threads
 * in the GUI code.
 */
#define NONREENTRANT_PARSER() \
    static bool active = false; \
    struct Guard { \
      Guard() {if (active) throw opp_runtime_error("non-reentrant parser invoked again while parsing"); active=true;} \
      ~Guard() {active=false;} \
    } __guard;


#endif


