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

#include <sstream>
#include <algorithm>
#include "stringtokenizer.h"
#include "scaveutils.h"
#include "enumtype.h"

USING_NAMESPACE


EnumType::EnumType(const EnumType& list)
{
     operator=(list);
}

EnumType::EnumType()
{
}

EnumType::~EnumType()
{
}

EnumType& EnumType::operator=(const EnumType& other)
{
    valueToNameMap = other.valueToNameMap;
    nameToValueMap = other.nameToValueMap;
    return *this;
}

void EnumType::insert(int value, const char *name)
{
    valueToNameMap[value] = name;
    nameToValueMap[name] = value;
}

const char *EnumType::nameOf(int value) const
{
    std::map<int,std::string>::const_iterator it = valueToNameMap.find(value);
    return it==valueToNameMap.end() ? NULL : it->second.c_str();
}

int EnumType::valueOf(const char *name, int fallback) const
{
    std::map<std::string,int>::const_iterator it = nameToValueMap.find(name);
    return it==nameToValueMap.end() ? fallback : it->second;
}

static bool less(const std::pair<int,std::string> &left, const std::pair<int,std::string> &right)
{
    return left.first < right.first;
}

static std::string second(std::pair<int,std::string> pair)
{
    return pair.second;
}

std::vector<std::string> EnumType::getNames() const
{
    std::vector<std::pair<int,std::string> > pairs(valueToNameMap.begin(), valueToNameMap.end());
    std::sort(pairs.begin(), pairs.end(), less);
    std::vector<std::string> names(pairs.size());
    std::transform(pairs.begin(), pairs.end(), names.begin(), second);
    return names;
}

std::string EnumType::str() const
{
    std::stringstream out;
    for (std::map<std::string,int>::const_iterator it=nameToValueMap.begin(); it!=nameToValueMap.end(); ++it)
    {
        if (it!=nameToValueMap.begin())
            out << ", ";
        out << it->first << "=" << it->second;
    }
    return out.str();
}

void EnumType::parseFromString(const char *str)
{
    valueToNameMap.clear();
    nameToValueMap.clear();

    StringTokenizer tokenizer(str, ", ");
    int value = -1;
    while (tokenizer.hasMoreTokens())
    {
        std::string nameValue = tokenizer.nextToken();
        std::string::size_type pos = nameValue.find('=');
        if (pos == std::string::npos)
        {
            insert(++value, nameValue.c_str());
        }
        else
        {
            std::string name = nameValue.substr(0, pos);
            std::string valueStr = nameValue.substr(pos+1);
            if (!parseInt(valueStr.c_str(), value))
                throw opp_runtime_error("Enum value must be an int, found: %s", valueStr.c_str());
            insert(value, name.c_str());
        }
    }
}
