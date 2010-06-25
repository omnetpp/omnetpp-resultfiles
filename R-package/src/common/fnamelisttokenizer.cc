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
#include <stdlib.h>
#include <string.h>
#include "fnamelisttokenizer.h"

USING_NAMESPACE


FilenamesListTokenizer::FilenamesListTokenizer(const char *s)
{
    if (!s)
        s = "";
    str = new char[strlen(s)+1];
    strcpy(str,s);
    rest = str;
}

FilenamesListTokenizer::~FilenamesListTokenizer()
{
    delete [] str;
}

bool FilenamesListTokenizer::hasMoreTokens()
{
    while (*rest==' ' || *rest=='\t') rest++;
    return *rest != '\0';
}

const char *FilenamesListTokenizer::nextToken()
{
    // we want to write "s" instead of "rest"
    char *&s = rest;

    // skip spaces before token
    while (*s==' ' || *s=='\t') s++;
    if (!*s)
        return NULL;

    char *token;
    if (*s=='"')
    {
        // parse quoted string
        token = s+1;
        s++;
        // try to find end-quote, or end of the string
        while (*s && *s!='"') s++;
        // terminate quoted string with zero, overwriting close quote (if it was there)
        if (*s)
            *s++ = 0;
    }
    else
    {
        // parse unquoted string
        token = s;
        // try find end of string
        while (*s && *s!=' ' && *s!='\t') s++;
        // terminate string with zero (if we are not already at end of the line)
        if (*s)
            *s++ = 0;
    }
    return token;
}

std::vector<std::string> FilenamesListTokenizer::asVector()
{
    const char *s;
    std::vector<std::string> v;
    while ((s=nextToken())!=NULL)
        v.push_back(std::string(s));
    return v;
}


