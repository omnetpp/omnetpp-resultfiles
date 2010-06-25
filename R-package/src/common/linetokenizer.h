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

#ifndef __LINETOKENIZER_H_
#define __LINETOKENIZER_H_

#include <vector>
#include <string>
#include "commondefs.h"

NAMESPACE_BEGIN

/**
 * Tokenizes a line. Uses space as default separator, and also honors quoted substrings
 * (that is, `one "two or three" four' is returned as 3 tokens `one',
 * `two or three', `four'). If a quote appears within a token, it needs to
 * be preceded by backslash. A backslash also needs to be quoted (ie.
 * duplicated) if it appears within a token.
 */
class COMMON_API LineTokenizer
{
  private:
    // separators
    char sep1;
    char sep2;

    int lineBufferSize;
    char *lineBuffer;

    // storage of tokens
    char **vec;
    int vecsize;
    int numtokens;

  public:
    /**
     * Constructor.
     */
    LineTokenizer(int lineBufferSize = 1000, int maxTokenNum=-1 /*=lineBufferSize/4*/, char sep1=' ', char sep2='\t');

    /**
     * Destructor.
     */
    ~LineTokenizer();

    /**
     * Tokenizes the line. Returns the number of tokens on the line.
     * Throws exception on error.
     */
    int tokenize(const char *line, int length);

    /**
     * Number of tokens read by the last tokenize() call.
     */
    int numTokens() {return numtokens;}

    /**
     * Array that holds the tokens read by the last tokenize() call;
     * contents will be overwritten with the next call.
     */
    char **tokens() {return vec;}
};

NAMESPACE_END


#endif

