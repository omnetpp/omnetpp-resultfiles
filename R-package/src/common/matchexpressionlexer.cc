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

#include "stringutil.h"
#include "matchexpression.tab.hh"  // token values
#include "matchexpressionlexer.h"
#include "platmisc.h"   // strncasecmp

USING_NAMESPACE

MatchExpressionLexer::MatchExpressionLexer(const char *input)
    : input(input), ptr(input)
{
}

int MatchExpressionLexer::getNextToken(char **valuep) {
    const char *start;

    *valuep = NULL;
    while (true) {
        start = ptr;
        switch (*ptr++) {
        case '\0': return -1;
        case '(': return '(';
        case ')': return ')';
        case '"':
        {
            std::string str = opp_parsequotedstr(start,ptr);
            *valuep = opp_strdup(str.c_str());
            return STRINGLITERAL;
        }
        case ' ': case '\n': case '\t': continue;
        default:
        {
            for (;;++ptr) {
                switch (*ptr) {
                case '\0':
                case ' ': case '\t': case '\n': case '(': case ')':
                    goto END;
                }
            };
            END:
            int len = ptr - start;
            if (len == 2 && strncasecmp(start, "or", 2) == 0)
                return OR_;
            if (len == 3 && strncasecmp(start, "and", 3) == 0)
                return AND_;
            if (len == 3 && strncasecmp(start, "not", 3) == 0)
                return NOT_;
            std::string str(start, len);
            *valuep = opp_strdup(str.c_str());
            return STRINGLITERAL;
        }
        }
    }
}

