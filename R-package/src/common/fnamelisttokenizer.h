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

#ifndef __FNAMELISTTOKENIZER_H
#define __FNAMELISTTOKENIZER_H

#include <string>
#include <vector>
#include "commondefs.h"
#include "commonutil.h"

NAMESPACE_BEGIN


/**
 * A string tokenizer class which honors quotation marks but not
 * backslashes. It is designed to split up filename lists returned by
 * cInifile::getAsFilenames().
 *
 * It considers the string as consisting of tokens separated by tab or space
 * characters. If a token contains space or tab ("C:\Program Files\X"),
 * it has to be surrounded with quotation marks. A token may NOT contain
 * quotation marks. (Rationale: filenames do not contain quotes but they
 * may well contain backslashes, so interpreting backslashes would not be
 * very practical.)
 */
class COMMON_API FilenamesListTokenizer
{
  private:
    char *str; // copy of full string (will be corrupted during tokenization)
    char *rest; // rest of string (to be tokenized)

  public:
    /**
     * Constructor. The class will make its own copy of the input string.
     */
    FilenamesListTokenizer(const char *str);

    /**
     * Destructor.
     */
    ~FilenamesListTokenizer();

    /**
     * Returns true if there're more tokens (i.e. the next nextToken()
     * call won't return NULL).
     */
    bool hasMoreTokens();

    /**
     * Returns the next token. The returned pointers will stay valid as long
     * as the tokenizer object exists. If there're no more tokens,
     * a NULL pointer will be returned.
     */
    const char *nextToken();

    /**
     * Utility function: tokenizes the full input string at once, and
     * returns the tokens in a string vector.
     */
    std::vector<std::string> asVector();
};

NAMESPACE_END


#endif

