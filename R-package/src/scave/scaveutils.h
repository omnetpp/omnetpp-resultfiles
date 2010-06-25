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

#ifndef _SCAVEUTILS_H_
#define _SCAVEUTILS_H_

#include <string>
#include <set>
#include <functional>
#include "intxtypes.h"
#include "scavedefs.h"
#include "timeutil.h"
#include "commonutil.h"

NAMESPACE_BEGIN

SCAVE_API bool parseInt(const char *str, int &dest);
SCAVE_API bool parseLong(const char *str, long &dest);
SCAVE_API bool parseInt64(const char *str, int64 &dest);
SCAVE_API bool parseDouble(const char *str, double &dest);
SCAVE_API bool parseSimtime(const char *str, simultime_t &dest);
SCAVE_API std::string unquoteString(const char *str);

// simple profiling macro
// var is a long variable collecting the execution time of stmt in usec
#define TIME(var,stmt) { timeval start,end; \
                         gettimeofday(&start,NULL); \
                         stmt; \
                         gettimeofday(&end,NULL); \
                         var += timeval_diff_usec(end,start); }

template <class Operation>
class FlipArgs
    : public std::binary_function<typename Operation::second_argument_type,
                                    typename Operation::first_argument_type,
                                    typename Operation::result_type>
{
    public:
        typedef typename Operation::second_argument_type first_argument_type;
        typedef typename Operation::first_argument_type second_argument_type;
        typedef typename Operation::result_type result_type;
        FlipArgs(const Operation & _Func) : op(_Func) {};
        result_type operator()(const first_argument_type& _Left, const second_argument_type& _Right)
        {
            return op(_Right, _Left);
        }
    protected:
        Operation op;
};

template<class Operation>
inline FlipArgs<Operation> flipArgs(const Operation& op)
{
    return FlipArgs<Operation>(op);
}

class StringPool
{
    private:
        std::set<std::string> pool;
        const std::string *lastInsertedPtr;
    public:
        StringPool() : lastInsertedPtr(NULL) {}
        const std::string *insert(const std::string& str);
        const std::string *find(const std::string& str) const;
        void clear() { lastInsertedPtr = NULL; pool.clear(); }
};

NAMESPACE_END


#endif

