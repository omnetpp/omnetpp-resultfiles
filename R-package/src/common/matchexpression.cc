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

#include "matchexpression.h"
#include "patternmatcher.h"
#include "commonutil.h"

USING_NAMESPACE


MatchExpression::Elem::Elem(PatternMatcher *pattern, const char *fieldname)
{
    type = fieldname==NULL ? PATTERN : FIELDPATTERN;
    this->fieldname = fieldname==NULL ? "" : fieldname;
    this->pattern = pattern;
}

MatchExpression::Elem::~Elem()
{
    if (type==PATTERN || type==FIELDPATTERN)
        delete pattern;
}

void MatchExpression::Elem::operator=(const Elem& other)
{
    type = other.type;
    if (type==PATTERN || type==FIELDPATTERN)
    {
        fieldname = other.fieldname;
        pattern = new PatternMatcher(*(other.pattern));
    }
}

MatchExpression::MatchExpression()
{
}

MatchExpression::MatchExpression(const char *pattern, bool dottedpath, bool fullstring, bool casesensitive)
{
    setPattern(pattern, dottedpath, fullstring, casesensitive);
}

void MatchExpression::setPattern(const char *pattern, bool dottedpath, bool fullstring, bool casesensitive)
{
    this->dottedpath = dottedpath;
    this->fullstring = fullstring;
    this->casesensitive = casesensitive;

    elems.clear();

    Assert(pattern);
    if (*pattern)
        parsePattern(elems, pattern, dottedpath, fullstring, casesensitive);
}

bool MatchExpression::matches(const Matchable *object)
{
    if (elems.empty())
        return false;

    const int stksize = 20;
    bool stk[stksize];

    int tos = -1;
    for (int i = 0; i < (int)elems.size(); i++)
    {
        Elem& e = elems[i];
        const char *attr;
        switch (e.type)
        {
          case Elem::PATTERN:
            if (tos>=stksize-1)
                throw opp_runtime_error("MatchExpression: malformed expression: stack overflow");
            attr = object->getDefaultAttribute();
            stk[++tos] = attr==NULL ? false : e.pattern->matches(attr);
            break;
          case Elem::FIELDPATTERN:
            if (tos>=stksize-1)
                throw opp_runtime_error("MatchExpression: malformed expression: stack overflow");
            attr = object->getAttribute(e.fieldname.c_str());
            stk[++tos] = attr==NULL ? false : e.pattern->matches(attr);
            break;
          case Elem::OR:
            if (tos<1)
                throw opp_runtime_error("MatchExpression: malformed expression: stack underflow");
            stk[tos-1] = stk[tos-1] || stk[tos];
            tos--;
            break;
          case Elem::AND:
            if (tos<1)
                throw opp_runtime_error("MatchExpression: malformed expression: stack underflow");
            stk[tos-1] = stk[tos-1] && stk[tos];
            tos--;
            break;
          case Elem::NOT:
            if (tos<0)
                throw opp_runtime_error("MatchExpression: malformed expression: stack underflow");
            stk[tos] = !stk[tos];
            break;
          default:
            throw opp_runtime_error("MatchExpression: malformed expression: unknown element type");
       }
    }
    if (tos!=0)
        throw opp_runtime_error("MatchExpression: malformed expression: %d items left on stack", tos);

    return stk[tos];
}

