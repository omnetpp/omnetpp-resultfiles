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

#include <iostream>
#include <map>

#include <R.h>
#include <Rdefines.h>
#include <Rinternals.h>

#include "xyarray.h"
#include "resultfilemanager.h"
#include "nodetype.h"
#include "nodetyperegistry.h"
#include "dataflowmanager.h"
#include "vectorfilereader.h"
#include "arraybuilder.h"
#include "util.h"
#include "dataflownetworkbuilder.h"
#include "loadVectors.h"

struct IDAndArray {
    ID id;
    XYArray *array;
};

typedef std::vector<IDAndArray> Vectors;

// referred by computed vectors
class RComputation : public Computation
{
    int id;
    std::string name;
    StringMap params;
public:
    RComputation(int id, const char *name, const StringMap &params) : id(id), name(name), params(params) {}
    virtual ~RComputation() {};
    virtual ComputationID getID() { return id; }
    virtual std::string getOperationName() { return name; }
    virtual StringMap getOperationParams() { return params; };
};

static ProcessingOperationList parseProcessingOperations(SEXP commands)
{
    ProcessingOperationList operations;
    int numOfCommands = GET_LENGTH(commands);
    for (int i = 0; i < numOfCommands; ++i)
    {
        SEXP command = VECTOR_ELT(commands, i);

        ProcessingOperationType type = Apply;
        const char *select = NULL;
        const char *functionName = NULL;
        StringMap attrs;

        const char *typeStr = CHAR(STRING_ELT(VECTOR_ELT(command, 0), 0));
        if (strcmp("apply", typeStr) == 0)
            type = Apply;
        else if (strcmp("compute", typeStr) == 0)
            type = Compute;
        else
            throw opp_runtime_error("Processing operation must be 'apply' or 'compute'. Received: '%s'", typeStr);

        SEXP selectSexp = getElementByName(command, "select");
        select = selectSexp == R_NilValue ? "" : CHAR(STRING_ELT(selectSexp, 0));

        SEXP functionCall = getElementByName(command, "fun");
        functionName = CHAR(STRING_ELT(VECTOR_ELT(functionCall, 0), 0));

        SEXP names = GET_NAMES(functionCall);
        for(int j = 1; j < GET_LENGTH(functionCall); ++j)
        {
            const char *argName = CHAR(STRING_ELT(names, j));
            SEXP argValue = VECTOR_ELT(functionCall, j);
            const char *valueAsString = NULL;
            switch (TYPEOF(argValue))
            {
            case NILSXP: break;
            case STRSXP: valueAsString = CHAR(STRING_ELT(argValue, 0)); break;
            case INTSXP:
            case REALSXP: valueAsString = CHAR(STRING_ELT(AS_CHARACTER(argValue), 0)); break;
            case LGLSXP: valueAsString = LOGICAL(argValue)[0] == 0 ? "false" : "true"; break;
            default: throw opp_runtime_error("Wrong type for argument '%s' in call for '%s'", argName, functionName);
            }

            if (valueAsString)
            {
                attrs[argName] = valueAsString;
            }
        }

        Computation *computation = new RComputation(i, functionName, attrs);
        operations.push_back(ProcessingOperation(type, select, computation));
    }

    return operations;
}

static Vectors loadVectors(SEXP vectors, SEXP commands, ResultFileManager &manager)
{
    Vectors vs;
    SEXP files = getElementByName(vectors, "file");
    if (!IS_CHARACTER(files))
    {
        error("vectors$file is not a character vector: %d", (int)TYPEOF(files));
        return vs;
    }

    SEXP vectorids = getElementByName(vectors, "vectorid");
    if (!IS_INTEGER(vectorids))
    {
        error("vectors$vectorid is not an integer vector");
        return vs;
    }

    int vectorCount = GET_LENGTH(vectorids);
    if (GET_LENGTH(files) != vectorCount)
    {
        error("vectors$file and vectors$vectorid have different lengths");
        return vs;
    }

    // load files
    IDList idlist;
    for (int i = 0; i < vectorCount; ++i)
    {
        const char *ospath = CHAR(STRING_ELT(files, i));
        int vectorid = INTEGER(vectorids)[i];
        ResultFile *resultFile = manager.loadFile(ospath);
        ID id = manager.getVectorById(resultFile, vectorid);
        if (id == 0)
        {
            error("Vector not found.");
            return vs;
        }
        idlist.add(id);
    }

    // build dataflow network
    DataflowNetworkBuilder builder(manager);
    ProcessingOperationList processing = parseProcessingOperations(commands);
    builder.build(idlist, processing);
    IDList outputIDs = builder.getOutputIDs();
    ArrayBuilderNodes arrayBuilders = builder.getArrayBuilderNodes();
    //builder.getDataflowManager()->dump();

    // run!
    builder.getDataflowManager()->execute();

    // copy data from nodes
    int count = outputIDs.size();
    for (int i = 0; i < count; i++)
    {
        IDAndArray vectordata;
        vectordata.id = outputIDs.get(i);
        vectordata.array = arrayBuilders[i]->getArray();
        vs.push_back(vectordata);
    }

    return vs;
}

static const char* datasetColumnNames[] = {"vectors", "vectordata", "attrs"};
static const int datasetColumnsLength = sizeof(datasetColumnNames) / sizeof(const char*);

static const char* vectorColumnNames[] = {"resultkey", "file", "vectorid", "module", "name"};
static const SEXPTYPE vectorColumnTypes[] = {INTSXP, STRSXP, INTSXP, STRSXP, STRSXP};
static const int vectorColumnsLength = sizeof(vectorColumnNames) / sizeof(const char*);

static const char* vectordataColumnNames[] = {"resultkey", "eventno", "x", "y"};
static const SEXPTYPE vectordataColumnTypes[] = {INTSXP, INTSXP, REALSXP, REALSXP};
static const int vectordataColumnsLength = sizeof(vectordataColumnNames) / sizeof(const char*);

static const char* attributeColumnNames[] = {"resultkey", "name", "value"};
static const SEXPTYPE attributeColumnTypes[] = {INTSXP, STRSXP, STRSXP};
static const int attributeColumnsLength = sizeof(attributeColumnNames) / sizeof(const char*);

static SEXP exportVectors(const ResultFileManager &manager, const Vectors &vecs)
{
    SEXP dataset;
    PROTECT(dataset = NEW_LIST(3));
    setNames(dataset, datasetColumnNames, datasetColumnsLength);

    // vectors
    int vectorCount = vecs.size();
    int vectordataCount = 0, attrCount = 0;
    SEXP vectors = createDataFrame(vectorColumnNames, vectorColumnTypes, vectorColumnsLength, vectorCount);
    SEXP resultKey = VECTOR_ELT(vectors, 0);
    SEXP file = VECTOR_ELT(vectors, 1);
    SEXP vectorid = VECTOR_ELT(vectors, 2);
    SEXP module = VECTOR_ELT(vectors, 3);
    SEXP name = VECTOR_ELT(vectors, 4);
    SET_ELEMENT(dataset, 0, vectors);
    UNPROTECT(1); // vectors
    for (int i = 0; i < vectorCount; ++i)
    {
        const VectorResult &vector = manager.getVector(vecs[i].id);
        if (!vector.isComputed())
            attrCount += vector.attributes.size();
        vectordataCount += vecs[i].array->length();

        INTEGER(resultKey)[i] = i;
        SET_STRING_ELT(file, i, mkChar(vector.fileRunRef->fileRef->fileSystemFilePath.c_str()));
        INTEGER(vectorid)[i] = vector.vectorId;
        SET_STRING_ELT(module, i, mkChar(vector.moduleNameRef->c_str()));
        SET_STRING_ELT(name, i, mkChar(vector.nameRef->c_str()));
    }

    // vectordata
    SEXP vectordata = createDataFrame(vectordataColumnNames, vectordataColumnTypes, vectordataColumnsLength, vectordataCount);
    resultKey = VECTOR_ELT(vectordata, 0);
    SEXP eventno = VECTOR_ELT(vectordata, 1);
    SEXP x = VECTOR_ELT(vectordata, 2);
    SEXP y = VECTOR_ELT(vectordata, 3);
    SET_ELEMENT(dataset, 1, vectordata);
    UNPROTECT(1); // vectordata
    int currentIndex = 0;
    currentIndex = 0;
    for (int i = 0; i < vectorCount; ++i)
    {
        XYArray *array = vecs[i].array;
        int arrayLen = array->length();
        for (int j = 0; j < arrayLen; ++j)
        {
            INTEGER(resultKey)[currentIndex] = i;
            INTEGER(eventno)[currentIndex] = array->getEventNumber(j);
            REAL(x)[currentIndex] = array->getX(j);
            REAL(y)[currentIndex] = array->getY(j);
            currentIndex++;
        }
    }

    // attributes
    SEXP attributes = createDataFrame(attributeColumnNames, attributeColumnTypes, attributeColumnsLength, attrCount);
    SEXP keys=VECTOR_ELT(attributes, 0);
    SEXP names=VECTOR_ELT(attributes, 1);
    SEXP values=VECTOR_ELT(attributes, 2);
    SET_ELEMENT(dataset, 2, attributes);
    UNPROTECT(1); // attributes
    currentIndex = 0;
    for (int i = 0; i < vectorCount; ++i)
    {
        const VectorResult &vector = manager.getVector(vecs[i].id);
        if (!vector.isComputed())
        {
            for (StringMap::const_iterator it=vector.attributes.begin(); it != vector.attributes.end(); ++it)
            {
                const char *nameStr = it->first.c_str();
                const char *valueStr = it->second.c_str();
                INTEGER(keys)[currentIndex] = i;
                SET_STRING_ELT(names, currentIndex, mkChar(nameStr));
                SET_STRING_ELT(values, currentIndex, mkChar(valueStr));
                currentIndex++;
            }
        }
    }

    UNPROTECT(1); // dataset

    return dataset;
}

extern "C" {

SEXP callLoadVectors(SEXP vectors, SEXP commands)
{
    try
    {
        ResultFileManager manager;
        Vectors vecs = loadVectors(vectors, commands, manager);
        SEXP dataset = exportVectors(manager, vecs);
        return dataset;
    }
    catch (opp_runtime_error &e)
    {
        error("Error in callLoadDataset: %s\n", e.what());
        return R_NilValue;
    }
}

} // extern "C"
