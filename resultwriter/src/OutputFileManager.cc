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


#include <time.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include "OutputFileManager.h"

using namespace std;

OutputFileManager::OutputFileManager()
{
}

OutputFileManager::~OutputFileManager()
{
}

string OutputFileManager::generateRunID(const string&baseString)
{
    char s[100];
    time_t temp = time(NULL);
    strftime(s, sizeof(s), "%Y%m%d-%H:%M:%S", localtime(&temp));
    return baseString + s;
}

void OutputFileManager::writeRunHeader(ofstream *out, const string&runID, const StringMap& runAttributes)
{
    *out << "run " << q(runID) << endl;
    writeAttributes(out, &runAttributes);
    *out << endl;
}

void OutputFileManager::writeAttributes(std::ofstream *out, const StringMap *attributes)  ///FIXME StringMap passing...
{
    const StringMap&::iterator iter;
    if (attributes)
    {
        for (iter = attributes->begin(); iter != attributes->end(); ++iter)
        {
            *out << "attr " << q(iter->first) << " " << q(iter->second) << endl;
        }
    }
}

void OutputFileManager::writeAttributes(std::ostringstream *out, StringMap *attributes)
{
    if (attributes)
    {
        StringMap::iterator iter;
        for (iter = attributes->begin(); iter != attributes->end(); iter++)
        {
            *out << "attr " << q(iter->first) << " " << q(iter->second) << endl;
        }
    }
}

/**
 * Quotes the given string if needed.
 */
static std::string opp_quotestr(const char *txt)
{
    char *buf = new char[4 * strlen(txt) + 3]; // a conservative guess
    char *d = buf;
    *d++ = '"';
    const char *s = txt;
    while (*s)
    {
        switch (*s)
        {
        case '\b':
            *d++ = '\\';
            *d++ = 'b';
            s++;
            break;
        case '\f':
            *d++ = '\\';
            *d++ = 'f';
            s++;
            break;
        case '\n':
            *d++ = '\\';
            *d++ = 'n';
            s++;
            break;
        case '\r':
            *d++ = '\\';
            *d++ = 'r';
            s++;
            break;
        case '\t':
            *d++ = '\\';
            *d++ = 't';
            s++;
            break;
        case '"':
            *d++ = '\\';
            *d++ = '"';
            s++;
            break;
        case '\\':
            *d++ = '\\';
            *d++ = '\\';
            s++;
            break;
        default:
            if (iscntrl(*s))
            {
                *d++ = '\\';
                *d++ = 'x';
                sprintf(d, "%2.2X", *s++);
                d += 2;
            }
            else
            {
                *d++ = *s++;
            }
        }
    }
    *d++ = '"';
    *d = '\0';

    std::string ret = buf;
    delete [] buf;
    return ret;
}

static bool opp_needsquotes(const char *txt)
{
    if (!txt[0])
        return true;
    for (const char *s = txt; *s; s++)
        if (isspace(*s) || *s == '\\' || *s == '"' || iscntrl(*s))
            return true;
    return false;
}

string OutputFileManager::q(const string& s)
{
    if (!&s || s.length() == 0)
        return "\"\"";

    bool needsQuotes = opp_needsquotes(s.c_str());
    if (needsQuotes)
        return opp_quotestr(s.c_str());
    else
        return s;
}
