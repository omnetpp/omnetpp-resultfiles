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

#include <assert.h>
#include <sstream>
#include <string.h>
#include "exception.h"
#include "linetokenizer.h"

USING_NAMESPACE


LineTokenizer::LineTokenizer(int bufferSize, int maxTokenNum, char sep1, char sep2)
    : sep1(sep1), sep2(sep2)
{
    if (maxTokenNum < 0)
        maxTokenNum = bufferSize/4;

    vecsize = maxTokenNum;
    vec = new char *[vecsize];

    lineBufferSize = bufferSize;
    lineBuffer = new char[lineBufferSize];
}

LineTokenizer::~LineTokenizer()
{
    delete [] vec;
    delete [] lineBuffer;
}

inline int h2d(char c)
{
    if (c>='0' && c<='9') return c-'0';
    if (c>='A' && c<='F') return c-'A'+10;
    if (c>='a' && c<='f') return c-'a'+10;
    return -1;
}

inline int h2d(char *&s)
{
    int a = h2d(*s);
    if (a<0) return 0;
    s++;
    int b = h2d(*s);
    if (b<0) return a;
    s++;
    return a*16+b;
}

static void interpretBackslashes(char *buffer)
{
    // interpret backslashes in-place. This works because the output
    // is always shorter (or equal) than the input.
    char *s = buffer;
    char *d = buffer;
    for (; *s; s++, d++)
    {
        if (*s=='\\')
        {
            // allow backslash as quote character, also interpret backslash sequences
            // note: this must be kept consistent with opp_quotestr()/opp_parsequotedstr()
            s++;
            switch(*s)
            {
                case 'b': *d = '\b'; break;
                case 'f': *d = '\f'; break;
                case 'n': *d = '\n'; break;
                case 'r': *d = '\r'; break;
                case 't': *d = '\t'; break;
                case 'x': s++; *d = h2d(s); s--; break; // hex code
                case '"': *d = '"'; break;  // quote needs to be backslashed
                case '\\': *d = '\\'; break;  // backslash needs to be backslashed
                case '\n': d--; break; // don't store line continuation (backslash followed by newline)
                case '\0': d--; s--; break; // string ends in stray backslash
                default: *d = *s; // be tolerant with unrecogized backslash sequences
            }
        }
        else
        {
            *d = *s;
        }
    }
    *d = '\0';
}

int LineTokenizer::tokenize(const char *line, int length)
{
    if (length >= lineBufferSize)
        throw opp_runtime_error("Cannot tokenize lines longer than %d", lineBufferSize - 1);

    strncpy(lineBuffer, line, length);
    lineBuffer[length] = '\0'; // guard

    char *s = lineBuffer + length - 1;
    while (s >= lineBuffer && (*s == '\r' || *s == '\n'))
        *s-- = '\0';

    numtokens = 0;
    s = lineBuffer;

    // loop through the tokens on the line
    for (;;)
    {
        // skip separators before token
        while (*s==sep1 || *s==sep2) s++;

        char *token;
        if (!*s)
        {
            // end of line found -- exit loop
            break;
        }
        else if (*s=='"')
        {
            // parse quoted string
            token = s+1;
            s++;
            // try to find end of quoted string
            bool containsBackslash = false;
            while (*s && *s!='"')
                if (*s++=='\\')
                    {s++; containsBackslash = true;}
            // check we found the close quote
            if (*s!='"')
                throw opp_runtime_error("Unmatched quote in file");
            // terminate quoted string with zero, overwriting close quote
            *s++ = 0;
            // if token contained a backslash (rare!), we need post-processing
            // to interpret the escape sequences
            if (containsBackslash)
                interpretBackslashes(token);

        }
        else
        {
            // parse unquoted string
            token = s;
            // try find end of string
            while (*s && *s!=sep1 && *s!=sep2) s++;
            // terminate string with zero (if we are not already at end of the line)
            if (*s) *s++ = 0;
        }

        // add token to the array (if there's room); s points to the rest of the string
        if (numtokens==vecsize)
            throw opp_runtime_error("Too many tokens on a line, max %d allowed", vecsize-1);
        vec[numtokens++] = token;
    }
    return numtokens;
}

/*
Example code:

#include <string.h>
#include <iostream>
using namespace std;

void tok(const char *s)
{
    char *buf = new char[strlen(s)+1];
    strcpy(buf, s);
    cout << buf << " --> ";

    LineTokenizer t;
    bool ok = t.tokenize(buf);
    if (!ok)
        cout << t.errorMsg(1) << endl;

    int numtokens = t.numTokens();
    char **vec = t.tokens();
    for (int i=0; i<numtokens; i++)
        cout << (i==0?"":":") << vec[i];
    cout << "\n";
}

int main(int argc, char **argv)
{
    tok("E 121.1344 e434");
    tok("E \"121.1344 e434\" 222");

    return 0;
}
*/


