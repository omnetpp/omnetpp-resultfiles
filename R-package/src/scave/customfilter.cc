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

#include <math.h>
#include "channel.h"
#include "customfilter.h"
#include "scaveutils.h"

USING_NAMESPACE



/**
 * Resolves variables (x, y) and functions (sin, fabs, etc) in
 * ExpressionFilterNode expressions.
 */
class Resolver : public Expression::Resolver
{
  private:
    ExpressionFilterNode *hostnode;
  public:
    Resolver(ExpressionFilterNode *node) {hostnode = node;}
    virtual ~Resolver() {};
    virtual Expression::Functor *resolveVariable(const char *varname);
    virtual Expression::Functor *resolveFunction(const char *funcname, int argcount);
};

Expression::Functor *Resolver::resolveVariable(const char *varname)
{
    if (strcmp(varname, "t")==0 || strcmp(varname, "y")==0 || strcmp(varname, "tprev")==0 || strcmp(varname, "yprev")==0)
        return new ExpressionFilterNode::NodeVar(hostnode, varname);
    else
        throw opp_runtime_error("Unrecognized variable: %s", varname);
}

Expression::Functor *Resolver::resolveFunction(const char *funcname, int argcount)
{
    //FIXME check argcount!
    if (MathFunction::supports(funcname))
        return new MathFunction(funcname);
    else
        throw opp_runtime_error("Unrecognized function: %s()", funcname);
}

ExpressionFilterNode::ExpressionFilterNode(const char *text)
{
    skipFirstDatum = false;
    expr = new Expression();
    Resolver resolver(this);
    expr->parse(text, &resolver);
    prevDatum.x = prevDatum.y = NaN;
}

ExpressionFilterNode::~ExpressionFilterNode()
{
    delete expr;
}

bool ExpressionFilterNode::isReady() const
{
    return in()->length()>0;
}

void ExpressionFilterNode::process()
{
    int n = in()->length();
    if (skipFirstDatum && n > 0) {
        in()->read(&prevDatum,1);
        n--;
        skipFirstDatum = false;
    }
    for (int i=0; i<n; i++)
    {
        in()->read(&currentDatum,1);
        double value = expr->doubleValue();
        prevDatum = currentDatum;
        currentDatum.y = value;
        out()->write(&currentDatum,1);
    }
}

double ExpressionFilterNode::getVariable(const char *varname)
{
    if (varname[0]=='t' && varname[1]==0)
        return currentDatum.x;
    else if (varname[0]=='y' && varname[1]==0)
        return currentDatum.y;
    else if (varname[0]=='t' && strcmp(varname, "tprev")==0)
        return prevDatum.x;
    else if (varname[0]=='y' && strcmp(varname, "yprev")==0)
        return prevDatum.y;
    else
        return 0.0;  // cannot happen, as validation has already taken place in Resolver
}

//--

const char *ExpressionFilterNodeType::getDescription() const
{
    return "Evaluates an arbitrary expression. Use t for time, y for value, and tprev, yprev for the previous values."; //FIXME use "t" and "y" instead?
}

void ExpressionFilterNodeType::getAttributes(StringMap& attrs) const
{
    attrs["expression"] = "The expression to evaluate.";
}

Node *ExpressionFilterNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    Node *node = new ExpressionFilterNode(attrs["expression"].c_str());
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}

void ExpressionFilterNodeType::mapVectorAttributes(/*inout*/StringMap &attrs) const
{
    attrs["type"] = "double";
}
