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

#ifndef __ENUMTYPE_H
#define __ENUMTYPE_H

#include <map>
#include <vector>
#include <string>
#include "scavedefs.h"

NAMESPACE_BEGIN

/**
 * Provides string representation for enums. The class basically implements
 * effective integer-to-string and string-to-integer mapping.
 */
class SCAVE_API EnumType
{
  private:
    std::map<int,std::string> valueToNameMap;
    std::map<std::string,int> nameToValueMap;

  public:
    /**
     * Constructor.
     */
    EnumType();

    /**
     * Copy constructor.
     */
    EnumType(const EnumType& list);

    /**
     * Destructor.
     */
    virtual ~EnumType();

    /**
     * Assignment operator. The name member doesn't get copied;
     * see cOwnedObject's operator=() for more details.
     */
    EnumType& operator=(const EnumType& list);

    /**
     * Add an item to the enum. If that numeric code exist, overwrite it.
     */
    void insert(int value, const char *name);

    /**
     * Look up value and return string representation. Return
     * NULL if not found.
     */
    const char *nameOf(int value) const;

    /**
     * Look up string and return numeric code. If not found,
     * return the second argument.
     */
    int valueOf(const char *name, int fallback=-1) const;

    /**
     * Returns the names of the enum members sorted by their values.
     */
    std::vector<std::string> getNames() const;

    /**
     * String representation of the definition of this enum.
     */
    std::string str() const;

    /**
     * Parses the name/value pairs from a string.
     */
    void parseFromString(const char *str);
};

NAMESPACE_END


#endif

