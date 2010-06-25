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

#ifndef _CUSTOMFILTER_H_
#define _CUSTOMFILTER_H_

#include "commonnodes.h"
#include "expression.h"

NAMESPACE_BEGIN


/**
 * Processing node which evaluates an arbitrary expression entered
 * by the user.
 *
 * TODO:
 *  - support parameters: $windowsize or $(windowsize) in the expression
 *    would prompt user to enter a value for windowsize;
 *  - extend the expression syntax into a mini imperative language to allow
 *    for writing more powerful filters: introduce variables and assigment operator
 *    (=, +=, *=, ++, --, etc); allow several expressions separated by semicolon;
 *    implement 'for', 'while', 'if'; filternode to keep a variables table;
 *    "init-expression" attr where user could initialize variables;
 *    possibly implement array variables as well.
 */
class SCAVE_API ExpressionFilterNode : public FilterNode
{
    public:
        /**
         * Implements a variable in the expression. Currently just delegates
         * to ExpressionFilterNode::getVariable().
         */
        class NodeVar : public Expression::Functor
        {
          private:
            ExpressionFilterNode *hostnode;
            std::string varname;
          public:
            NodeVar(ExpressionFilterNode *node, const char *name)
                {hostnode = node; varname = name; hostnode->skipFirstDatum |= (varname=="tprev" || varname=="yprev"); }
            virtual ~NodeVar() {}
            virtual Expression::Functor *dup() const {return new NodeVar(hostnode, varname.c_str());}
            virtual const char *getName() const {return varname.c_str();}
            virtual const char *getArgTypes() const {return "";}
            virtual char getReturnType() const {return Expression::Value::DBL;}
            virtual Expression::Value evaluate(Expression::Value args[], int numargs)
                {return hostnode->getVariable(varname.c_str());}
            virtual std::string str(std::string args[], int numargs) {return getName();}
        };
    private:
        Expression *expr;
        Datum currentDatum, prevDatum;
        bool skipFirstDatum;
    public:
        ExpressionFilterNode(const char *expression);
        virtual ~ExpressionFilterNode();
        virtual bool isReady() const;
        virtual void process();
        double getVariable(const char *varname);
};

class SCAVE_API ExpressionFilterNodeType : public FilterNodeType
{
    public:
        virtual const char *getName() const {return "expression";}
        virtual const char *getDescription() const;
        virtual void getAttributes(StringMap& attrs) const;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const;
        virtual void mapVectorAttributes(/*inout*/StringMap &attrs) const;
};

NAMESPACE_END


#endif


