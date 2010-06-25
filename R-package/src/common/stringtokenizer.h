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

#ifndef __STRINGTOKENIZER_H
#define __STRINGTOKENIZER_H

#include <string>
#include <vector>
#include "commondefs.h"

NAMESPACE_BEGIN

/**
 * String tokenizer class, modelled after strtok(). It considers the input
 * string to consist of tokens, separated by one or more delimiter characters.
 * Repeated calls to nextToken() will enumerate the tokens in the string,
 * returning NULL after the last token. The function hasMoreTokens() can be
 * used to find out whether there are more tokens without consuming one.
 *
 * Limitations: this class does not honor quotes, apostrophes or backslash
 * quoting; nor does it return empty tokens if it encounters multiple
 * delimiter characters in a row (so setting the delimiter to "," does not
 * produce the desired results). This behaviour is consistent with strtok().
 *
 * Example 1:
 *
 * <pre>
 * const char *str = "one two three four";
 * StringTokenizer tokenizer(str);
 * while (tokenizer.hasMoreTokens())
 *     ev << " [" << tokenizer.nextToken() << "]";
 * </pre>
 *
 * Example 2:
 *
 * <pre>
 * const char *str = "42 13 46 72 41";
 * std::vector<int> array = cStringTokenizer(str).asIntVector();
 * </pre>
 *
 * @ingroup SimSupport
 */
class COMMON_API StringTokenizer
{
  private:
    char *str; // copy of full string
    char *rest; // rest of string (to be tokenized)
    std::string delimiter;

  public:
    /**
     * Constructor. The class will make its own copy of the input string
     * and of the delimiters string. The delimiters default to all whitespace
     * characters (space, tab, CR, LF, FF).
     */
    StringTokenizer(const char *str, const char *delimiters=NULL);

    /**
     * Destructor.
     */
    ~StringTokenizer();

    /**
     * Change delimiters. This allows for switching delimiters during
     * tokenization.
     */
    void setDelimiter(const char *s);

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

