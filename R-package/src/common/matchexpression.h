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

#ifndef __MATCHEXPRESSION_H__
#define __MATCHEXPRESSION_H__

#include <string>
#include <vector>
#include "commondefs.h"
#include "exception.h"

NAMESPACE_BEGIN

class PatternMatcher;


/**
 * Matches various fields of an object. By default, a pattern must match
 * the "default field" of the object, which will usually be its name.
 * Other fields can be matched with the fieldname(pattern) syntax.
 * These elements can be combined with the AND, OR, NOT operators, accepted in
 * both lowercase and uppercase. AND has higher precedence than OR, but
 * parentheses can be used to change the evaluation order.
 *
 * Patterns are those accepted by PatternMatcher, that is, "*", "?",
 * character ranges as "{a-z}", numeric ranges as "{0..999}", or bracketed
 * numeric ranges as "[0..999]" (e.g. "*[90..100] matching "foo[95]")
 * are accepted.
 *
 * Pattern examples:
 *  - "" matches nothing
 *  - "node*"
 *  - "node* or host*"
 *  - "node* or host* or className(StandardHost*)"
 *  - "packet-* and className(PPPFrame)"
 *  - "className(TCPSegment) and not kind({0..2}) and SYN or data-*"
 *  - "className(TCPSegment) or byteLength({4096..})
 */
class COMMON_API MatchExpression
{
  public:
    /**
     * Objects to be matched must implement this interface
     */
    class COMMON_API Matchable
    {
      public:
        /**
         * Return the default string to match. The returned pointer will not
         * be cached by the caller, so it is OK to return a pointer to some
         * internal buffer which gets overwritten by subsequent
         * getDefaultAttribute() / getAttribute() calls.
         */
        virtual const char *getDefaultAttribute() const = 0;

        /**
         * Return the default string to match. Should return NULL if attribute
         * doesn't exist. The returned pointer will not be cached by the caller,
         * so it is OK to return a pointer to some internal buffer which gets
         * overwritten by subsequent getDefaultAttribute() / getAttribute() calls.
         */
        virtual const char *getAttribute(const char *name) const = 0;

        /**
         * Virtual destructor, for good manners...
         */
        virtual ~Matchable() {}
    };

    /**
     * One element in a (reverse Polish) expression
     */
    class COMMON_API Elem
    {
      public:
        friend class MatchExpression;
        enum Type {UNDEF, AND, OR, NOT, PATTERN, FIELDPATTERN};
      private:
        Type type;
        std::string fieldname;
        PatternMatcher *pattern;
      public:
        /** Ctor for AND, OR, NOT */
        Elem(Type type=UNDEF)  {this->type = type;}

        /** The given field of the object must match pattern */
        Elem(PatternMatcher *pattern, const char *fieldname=NULL);

        /** Copy ctor */
        Elem(const Elem& other)  {type=UNDEF; operator=(other);}

        /** Dtor */
        ~Elem();

        /** Assignment */
        void operator=(const Elem& other);
    };

  protected:
    // options
    bool dottedpath;
    bool fullstring;
    bool casesensitive;

    // stores the expression
    std::vector<Elem> elems;

  protected:
    // internal: access to the parser
    static void parsePattern(std::vector<MatchExpression::Elem>& elems, const char *pattern,
                             bool dottedpath, bool fullstring, bool casesensitive);

  public:
    /**
     * Constructor
     */
    MatchExpression();

    /**
     * Constructor, accepts the same args as setPattern().
     */
    MatchExpression(const char *pattern, bool dottedpath, bool fullstring, bool casesensitive);

    /**
     * Sets the pattern to be used by subsequent calls to matches(). See the
     * general class description for the meaning of the rest of the arguments.
     * Throws cException if the pattern is bogus.
     */
    void setPattern(const char *pattern, bool dottedpath, bool fullstring, bool casesensitive);

    /**
     * Returns true if the line matches the pattern with the given settings.
     * See setPattern().
     */
    bool matches(const Matchable *object);
};


/**
 * Wrapper to make a string matchable with MatchExpression.
 */
class COMMON_API MatchableString : public MatchExpression::Matchable
{
  private:
    std::string str;
  public:
    MatchableString(const char *s) {str = s;}
    virtual const char *getDefaultAttribute() const {return str.c_str();}
    virtual const char *getAttribute(const char *name) const {return NULL;}
};

NAMESPACE_END


#endif


