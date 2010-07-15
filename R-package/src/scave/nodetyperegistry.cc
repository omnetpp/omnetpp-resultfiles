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

#include "commonutil.h"
#include "nodetyperegistry.h"
#include "stringtokenizer.h"
#include "arraybuilder.h"
#include "vectorfilereader.h"
#include "vectorfilewriter.h"
#include "indexedvectorfile.h"
#include "indexedvectorfilereader.h"
#include "indexedvectorfilereader2.h"
#include "filewriter.h"
#include "windowavg.h"
#include "slidingwinavg.h"
#include "filternodes.h"
#include "mergernodes.h"
#include "xyplotnode.h"
#include "diffquot.h"
#include "customfilter.h"
#include "stddev.h"
#include "teenode.h"

USING_NAMESPACE

NodeTypeRegistry *NodeTypeRegistry::inst;

NodeTypeRegistry *NodeTypeRegistry::getInstance()
{
    if (!inst)
        inst = new NodeTypeRegistry();
    return inst;
}

void NodeTypeRegistry::add(NodeType *nodetype)
{
    nodeTypeMap[nodetype->getName()] = nodetype;
}

void NodeTypeRegistry::remove(NodeType *nodetype)
{
    NodeTypeMap::iterator it = nodeTypeMap.find(nodetype->getName());
    if (it!=nodeTypeMap.end())
        nodeTypeMap.erase(it);
}

NodeTypeRegistry::NodeTypeRegistry()
{
    add(new ArrayBuilderNodeType());
    add(new StddevNodeType());
    add(new VectorFileReaderNodeType());
    add(new VectorFileWriterNodeType());
    add(new IndexedVectorFileWriterNodeType());
    add(new IndexedVectorFileReaderNodeType());
    add(new IndexedVectorFileReaderNode2Type());
    add(new FileWriterNodeType());
    add(new MergerNodeType());
    add(new AggregatorNodeType());
    add(new XYPlotNodeType());

    add(new WindowAverageNodeType());
    add(new TimeWindowAverageNodeType());
    add(new SlidingWindowAverageNodeType());
    add(new MovingAverageNodeType());
    add(new DifferenceQuotientNodeType());
    add(new NopNodeType());
    add(new AdderNodeType());
    add(new MultiplierNodeType());
    add(new DividerNodeType());
    add(new ModuloNodeType());
    add(new DifferenceNodeType());
    add(new TimeDiffNodeType());
    add(new SumNodeType());
    add(new TimeShiftNodeType());
    add(new LinearTrendNodeType());
    add(new CropNodeType());
    add(new MeanNodeType());
    add(new RemoveRepeatsNodeType());
    add(new CompareNodeType());
    add(new IntegrateNodeType());
    add(new TimeAverageNodeType());
    add(new DivideByTimeNodeType());
    add(new TimeToSerialNodeType());
    add(new SubstractFirstValueNodeType());

    add(new ExpressionFilterNodeType());

    add(new TeeNodeType());
}

NodeTypeRegistry::~NodeTypeRegistry()
{
    for (NodeTypeMap::iterator it=nodeTypeMap.begin(); it!=nodeTypeMap.end(); it++)
        delete it->second;
}

bool NodeTypeRegistry::exists(const char *name)
{
    return nodeTypeMap.find(name)!=nodeTypeMap.end();
}

NodeType *NodeTypeRegistry::getNodeType(const char *name)
{
    NodeTypeMap::iterator it = nodeTypeMap.find(name);
    if (it==nodeTypeMap.end())
        throw opp_runtime_error("unknown node type `%s'", name);
    return it->second;
}

NodeTypeVector NodeTypeRegistry::getNodeTypes()
{
    NodeTypeVector vect;
    for (NodeTypeMap::iterator it=nodeTypeMap.begin(); it!=nodeTypeMap.end(); it++)
        vect.push_back(it->second);
    return vect;
}

Node *NodeTypeRegistry::createNode(const char *filterSpec, DataflowManager *mgr)
{
    // parse filterSpec
    std::string name;
    std::vector<std::string> args;
    parseFilterSpec(filterSpec, name, args);

    // look up node type
    NodeType *nodeType = getNodeType(name.c_str());

    // check number of args match
    StringMap attrs;
    nodeType->getAttributes(attrs);
    if (attrs.size()!=args.size())
        throw opp_runtime_error("error in filter spec `%s' -- %s expects %d parameters", filterSpec, name.c_str(), attrs.size());

    // fill in args map
    //FIXME this is completely unsafe! it would be better to match them by name, since ordering in Map is undefined...
    StringMap attrValues;
    for (StringMap::iterator it=attrs.begin(); it!=attrs.end(); ++it)
        attrValues[it->first] = ""; // initialize
    nodeType->getAttrDefaults(attrValues);
    int i=0;
    for (StringMap::iterator it=attrValues.begin(); it!=attrValues.end(); ++it, ++i)
        if (!args[i].empty())
            it->second = args[i];

    // create filter
    return nodeType->create(mgr, attrValues);
}

void NodeTypeRegistry::parseFilterSpec(const char *filterSpec, std::string& name, std::vector<std::string>& args)
{
    args.clear();
    const char *paren = strchr(filterSpec, '(');
    if (!paren) {
        // no left paren -- treat the whole string as filter name
        name = filterSpec;
        return;
    }

    // check that string ends in right paren
    if (filterSpec[strlen(filterSpec)-1]!=')')
        throw opp_runtime_error("syntax error in filter spec `%s'", filterSpec);

    // filter name is the part before the left paren
    name.assign(filterSpec, paren-filterSpec);

    // arg list is the part between the parens -- split it up along commas
    std::string arglist(paren+1, strlen(paren)-2);
    StringTokenizer tokenizer(arglist.c_str(), ",");
    const char *token;
    while ((token = tokenizer.nextToken())!=NULL)
        args.push_back(token);
}


