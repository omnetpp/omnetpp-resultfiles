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

#ifndef _COMPOUNDFILTER_H_
#define _COMPOUNDFILTER_H_

#include <string>
#include <vector>

#include "nodetype.h"
#include "commonnodes.h"

NAMESPACE_BEGIN


class CompoundFilterType;


/**
 * FilterNode nodes combined into a single filter by cascading them.
 */
class SCAVE_API CompoundFilter : public FilterNode
{
        friend class CompoundFilterType;
    protected:
        FilterNode *first;
        FilterNode *last;
        StringMap _attrvalues;  // _attrs[name] = value
    public:
        CompoundFilter() {}
        virtual ~CompoundFilter() {}
        FilterNode *getFirstNode() {return first;}
        FilterNode *getLastNode() {return last;}

        virtual bool isReady() const  {return false;}
        virtual void process()  {}
        virtual bool isFinished() const {return true;}
};


/**
 * FilterNode nodes combined into a single filter by cascading them.
 */
class SCAVE_API CompoundFilterType : public FilterNodeType
{
    public:
        /** Describes one of the cascaded filters */
        class Subfilter
        {
            private:
                std::string _nodetype;  // refers to NodeType name
                std::string _comment;   // comment
                StringMap _attrassignments;  // _attrs[name] = value
            public:
                const char *getNodeType() const  {return _nodetype.c_str();}
                const char *getComment() const  {return _comment.c_str();}
                void setNodeType(const char *s)  {_nodetype = s;}
                void setComment(const char *s)  {_comment = s;}
                /** Allows update, too */
                StringMap& attrAssignments() {return _attrassignments;}
                bool operator==(const Subfilter& other) const;
        };

    private:
        std::string _name;
        std::string _description;
        bool _hidden;
        StringMap _attrs;  // _attrs[name] = description
        StringMap _defaults;  // _defaults[name] = default value

        typedef std::vector<Subfilter> Subfilters;
        Subfilters _subfilters;  // filters are now connected in cascade

    public:
        CompoundFilterType() {_hidden=false;}
        virtual ~CompoundFilterType() {}
        CompoundFilterType& operator=(const CompoundFilterType& other);
        bool equals(const CompoundFilterType& other);

        /** Name, description etc. */
        //@{
        virtual const char *getName() const;
        virtual const char *getCategory() const  {return "custom filter";}
        virtual const char *getDescription() const;
        virtual bool isHidden() const {return _hidden;}
        virtual void setName(const char *);
        virtual void setDescription(const char *);
        virtual void setHidden(bool hidden) {_hidden=hidden;}
        //@}

        /** Creation */
        //@{
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const;
        virtual Port *getPort(Node *node, const char *portname) const;
        //@}

        /** Attributes, ports */
        //@{
        virtual void getAttributes(StringMap& attrs) const;
        virtual void getAttrDefaults(StringMap& attrs) const;
        virtual void setAttr(const char *name, const char *desc, const char *defaultvalue);
        virtual void removeAttr(const char *name);
        //@}

        /** Subfilters */
        //@{
        virtual int getNumSubfilters() const;
        /**
         * Allows update too, via object reference
         */
        virtual Subfilter& getSubfilter(int pos);
        /**
         * Access subfilter.
         */
        virtual const Subfilter& getSubfilter(int pos) const;
        /**
         * Insert subfilter at given position (old filter at pos and following ones
         * are shifted up).
         */
        virtual void insertSubfilter(int pos, const Subfilter& f);
        /**
         * Remove subfilter at given position (following ones are shifted down).
         */
        virtual void removeSubfilter(int pos);
        //@}

        /** Vector attribute mapping */
        //@{
        /**
         * Maps attributes of the input vector to attributes of the output vector.
         */
        virtual void mapVectorAttributes(/*inout*/StringMap &attrs, /*out*/StringVector &warnings) const;
        //@}
};


NAMESPACE_END


#endif
