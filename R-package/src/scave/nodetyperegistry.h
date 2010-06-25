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

#ifndef _NODETYPEREGISTRY_H_
#define _NODETYPEREGISTRY_H_

#include <map>
#include <vector>
#include <string>
#include "node.h"
#include "nodetype.h"

NAMESPACE_BEGIN

class DataflowManager;

typedef std::vector<NodeType*> NodeTypeVector;


/**
 * Factory for all available node types.
 *
 * @see NodeType, Node
 */
class SCAVE_API NodeTypeRegistry
{
    protected:
        static NodeTypeRegistry *inst;
        typedef std::map<std::string,NodeType*> NodeTypeMap;
        NodeTypeMap nodeTypeMap;

        NodeTypeRegistry();
        ~NodeTypeRegistry();

    public:
        /** Access to singleton instance */
        static NodeTypeRegistry *getInstance();

        /** Returns list of node types */
        NodeTypeVector getNodeTypes();

        /** True if a node type with the given name exists */
        bool exists(const char *name);

        /** Returns a node type by name */
        NodeType *getNodeType(const char *name);

        /** Parse filter spec, and create the corresponding node */
        Node *createNode(const char *filterSpec, DataflowManager *mgr);

        /**
         * Parse the given string in form "name(arg1,arg2,...)" into a
         * filter name and filter args list.
         */
        static void parseFilterSpec(const char *filterSpec, std::string& name, std::vector<std::string>& args);

        /** Add a node type to the factory */
        void add(NodeType *nodetype);

        /** Remove a node type from the factory */
        void remove(NodeType *nodetype);

};

NAMESPACE_END


#endif
