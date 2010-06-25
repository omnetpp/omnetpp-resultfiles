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

#ifndef _NODETYPE_H_
#define _NODETYPE_H_

#include <map>
#include <string>
#include <vector>
#include "dataflowmanager.h"

NAMESPACE_BEGIN

class Node;
class Port;

typedef std::map<std::string,std::string> StringMap;
typedef std::vector<std::string> StringVector;


/**
 * Describes properties of a node class.
 *
 * @see Node, NodeTypeRegistry
 */
class SCAVE_API NodeType
{
    protected:
        void checkAttrNames(const StringMap& attr) const;

    public:
        NodeType() {}
        virtual ~NodeType() {}

        /**
         * Returns type name as displayed on the UI (e.g. "mean" or "winavg")
         */
        virtual const char *getName() const = 0;

        /**
         * Returns the category of the getNode(source, sink, filter, etc.)
         */
        virtual const char *getCategory() const = 0;

        /**
         * Returns the description of the getNode(this can be displayed on the UI.)
         */
        virtual const char *getDescription() const = 0;

        /**
         * Returns true if this node should not appear in the UI.
         */
        virtual bool isHidden() const = 0;

        /**
         * Fills the string map with the names of all accepted attributes
         * and their descriptions.
         */
        virtual void getAttributes(StringMap& attrs) const = 0;

        /**
         * Fills the string map with attribute names and their default values.
         * (Doesn't necessarily supply default value for every attribute).
         */
        virtual void getAttrDefaults(StringMap& attrs) const {}

        /**
         * Throws exception if something is not OK. The message in the
         * exception is shown to the user.
         */
        virtual void validateAttrValues(const StringMap& attrs) const {}

        /**
         * Create a node instance of this type. Also adds the node to the
         * data-flow network, so you don't need to call mgr->addNode() afterwards.
         */
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const = 0;

        /**
         * Get the named port of the give node instance.
         */
        virtual Port *getPort(Node *node, const char *portname) const = 0;

        /**
         * Maps input vector attributes to output vector attributes.
         * This method is called for filter nodes only.
         */
        virtual void mapVectorAttributes(/*inout*/StringMap &attrs, /*out*/StringVector &warnings) const;
};


NAMESPACE_END


#endif
