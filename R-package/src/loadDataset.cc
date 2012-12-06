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

// TODO IDE should use 'runid' as field name, instead of 'run'

#include <iostream>
#include <map>

#include "resultfilemanager.h"

#include <R.h>
#include <Rdefines.h>
#include <Rinternals.h>

#include "util.h"
#include "loadDataset.h"

static int checkType(SEXP type)
{
    if (type == R_NilValue)
        return (ResultFileManager::SCALAR | ResultFileManager::VECTOR | ResultFileManager::HISTOGRAM);
    if (!IS_CHARACTER(type))
        throw opp_runtime_error("type is not a string");
    if (GET_LENGTH(type) == 0)
        throw opp_runtime_error("type is an empty character vector");

    const char *typeStr = CHAR(STRING_ELT(type, 0));
    if (strcmp(typeStr, "scalar") == 0)
        return ResultFileManager::SCALAR;
    else if (strcmp(typeStr, "vector") == 0)
        return ResultFileManager::VECTOR;
    else if (strcmp(typeStr, "statistic") == 0)
        return ResultFileManager::HISTOGRAM;
    else
        throw opp_runtime_error("unknown type: %s", typeStr);
}

static ResultFileList loadFiles(SEXP files, ResultFileManager &manager)
{
    if (!IS_CHARACTER(files))
        throw opp_runtime_error("files is not a character vector");

    int numOfFiles = GET_LENGTH(files);
    ResultFileList fileList;
    for (int j = 0; j < numOfFiles; ++j)
    {
        const char *f = CHAR(STRING_ELT(files, j));
        fileList.push_back(manager.loadFile(f));
    }
    return fileList;
}

static IDList selectIDs(int type, const char *pattern, const ResultFileManager &manager)
{
    IDList source;
    if ((type & ResultFileManager::SCALAR) != 0)
    {
        IDList scalars = manager.getAllScalars(false, false);
        source.merge(scalars);
    }
    if ((type & ResultFileManager::VECTOR) != 0)
    {
        IDList vectors = manager.getAllVectors();
        source.merge(vectors);
    }
    if ((type & ResultFileManager::HISTOGRAM) != 0)
    {
        IDList statistics = manager.getAllHistograms();
        source.merge(statistics);
    }
    IDList idlist = manager.filterIDList(source, pattern);

    return idlist;
}

static void executeCommands(SEXP files, SEXP commands, ResultFileManager &manager, IDList &out)
{
    ResultFileList fileList = loadFiles(files, manager);

    if (!IS_VECTOR(commands))
    {
        throw opp_runtime_error("commands is not a list");
    }

    int numOfCommands = GET_LENGTH(commands);
    if (numOfCommands == 0)
    {
        IDList scalars = manager.getAllScalars(false, false);
        out.merge(scalars);
        IDList vectors = manager.getAllVectors();
        out.merge(vectors);
        IDList statistics = manager.getAllHistograms();
        out.merge(statistics);
    }
    else
    {
        for (int i = 0; i < numOfCommands; ++i)
        {
            SEXP command = VECTOR_ELT(commands, i);

            if (!IS_LIST(command))
                throw opp_runtime_error("command is not a list");
            if (GET_LENGTH(command) == 0)
                throw opp_runtime_error("command is empty list");

            SEXP op = VECTOR_ELT(command, 0);
            if (!isSymbol(op))
                throw opp_runtime_error("operator is not a symbol: %d", (int)TYPEOF(op));
            const char *opname = CHAR(PRINTNAME(op));

            int type = checkType(getElementByName(command, "type"));

            SEXP select = getElementByName(command, "select");
            const char *selectStr = NULL;
            if (IS_CHARACTER(select) && GET_LENGTH(select) > 0)
                selectStr = CHAR(STRING_ELT(select, 0));
            else if (select != R_NilValue)
                throw opp_runtime_error("select is not a string");

            if (strcmp(opname, "add") == 0)
            {
                IDList selectedIDs = selectIDs(type, selectStr, manager);
                out.merge(selectedIDs);
            }
            else if (strcmp(opname, "discard") == 0)
            {
                IDList selectedIDs = selectIDs(type, selectStr, manager);
                out.substract(selectedIDs);
            }
            else
            {
                throw opp_runtime_error("unknown command: %s", opname);
            }
        }
    }
}

static void addResultItemAttributes(SEXP attributes, int &currentIndex, const char *type, const IDList &ids, int keyStart, const ResultFileManager &manager)
{
    SEXP types=VECTOR_ELT(attributes, 0), keys=VECTOR_ELT(attributes, 1), names=VECTOR_ELT(attributes, 2), values=VECTOR_ELT(attributes, 3);
    SEXP typeSEXP = mkChar(type); // immutable, so can be shared
    for (int i = 0; i < ids.size(); ++i)
    {
        const ResultItem &resultItem = manager.getItem(ids.get(i));
        for (StringMap::const_iterator it=resultItem.attributes.begin(); it != resultItem.attributes.end(); ++it)
        {
            const char *nameStr = it->first.c_str();
            const char *valueStr = it->second.c_str();
            SET_STRING_ELT(types, currentIndex, typeSEXP);
            INTEGER(keys)[currentIndex] = keyStart + i;
            SET_STRING_ELT(names, currentIndex, mkChar(nameStr));
            SET_STRING_ELT(values, currentIndex, mkChar(valueStr));
            currentIndex++;
        }
    }
}

static IDList filterIDListByType(const IDList &idlist, int type, const ResultFileManager &manager, bool includeFields = false)
{
    IDList result;
    int size = idlist.size();
    for (int i = 0; i < size; ++i)
    {
        ID id = idlist.get(i);
        if (ResultFileManager::getTypeOf(id) == type)
        {
            if (type != ResultFileManager::SCALAR || includeFields || !manager.getScalar(id).isField)
                result.add(id);
        }
    }
    return result;
}

static const char* datasetColumnNames[] = {"runattrs", "fileruns", "scalars", "vectors", "statistics", "fields", "bins", "params", "attrs"};
static const int datasetColumnsLength = sizeof(datasetColumnNames) / sizeof(const char*);

const char* runColumnNames[] = {"runid", "attrname", "attrvalue"};
const SEXPTYPE runColumnTypes[] = {STRSXP, STRSXP, STRSXP};
const int runColumnsLength = sizeof(runColumnNames) / sizeof(const char*);

const char* filerunColumnNames[] = {"runid", "file"};
const SEXPTYPE filerunColumnTypes[] = {STRSXP, STRSXP};
const int filerunColumnsLength = sizeof(filerunColumnNames) / sizeof(const char*);

const char* scalarColumnNames[] = {"resultkey", "runid", "file", "module", "name", "value"};
const SEXPTYPE scalarColumnTypes[] = {INTSXP, STRSXP, STRSXP, STRSXP, STRSXP, REALSXP};
const int scalarColumnsLength = sizeof(scalarColumnNames) / sizeof(const char*);

const char* vectorColumnNames[] = {"resultkey", "runid", "file", "vectorid", "module", "name"};
const SEXPTYPE vectorColumnTypes[] = {INTSXP, STRSXP, STRSXP, INTSXP, STRSXP, STRSXP};
const int vectorColumnsLength = sizeof(vectorColumnNames) / sizeof(const char*);

const char* statisticColumnNames[] = {"resultkey", "runid", "file", "module", "name"};
const SEXPTYPE statisticColumnTypes[] = {INTSXP, STRSXP, STRSXP, STRSXP, STRSXP};
const int statisticColumnsLength = sizeof(statisticColumnNames) / sizeof(const char*);

const char* fieldColumnNames[] = {"resultkey", "fieldname", "fieldvalue"};
const SEXPTYPE fieldColumnTypes[] = {INTSXP, STRSXP, REALSXP};
const int fieldColumnsLength = sizeof(fieldColumnNames) / sizeof(const char*);

const char* binColumnNames[] = {"resultkey", "lowerbound", "upperbound", "count"};
const SEXPTYPE binColumnTypes[] = {INTSXP, REALSXP, REALSXP, REALSXP};
const int binColumnsLength = sizeof(binColumnNames) / sizeof(const char*);

const char* paramColumnNames[] = {"runid", "paramname", "paramvalue"};
const SEXPTYPE paramColumnTypes[] = {STRSXP, STRSXP, STRSXP};
const int paramColumnsLength = sizeof(paramColumnNames) / sizeof(const char*);

const char* attributeColumnNames[] = {"attrtype", "resultkey", "attrname", "attrvalue"};
const SEXPTYPE attributeColumnTypes[] = {STRSXP, INTSXP, STRSXP, STRSXP};
const int attributeColumnsLength = sizeof(attributeColumnNames) / sizeof(const char*);

SEXP exportDataset(ResultFileManager &manager, const IDList &idlist)
{
    int paramsCount = 0, attrCount = 0, runAttrCount = 0;

    SEXP dataset, names;
    PROTECT(dataset = NEW_LIST(9));
    setNames(dataset, datasetColumnNames, datasetColumnsLength);

    // runattrs
    RunList *runList = manager.getUniqueRuns(idlist);
    int runCount = runList->size();
    for(int i = 0; i < runCount; ++i)
    {
        Run *runPtr = runList->at(i);
        paramsCount += runPtr->moduleParams.size();
        runAttrCount += runPtr->attributes.size();
    }
    SEXP runattrs = createDataFrame(runColumnNames, runColumnTypes, runColumnsLength, runAttrCount);
    SEXP runid=VECTOR_ELT(runattrs,0), name=VECTOR_ELT(runattrs,1), value=VECTOR_ELT(runattrs,2);
    SET_ELEMENT(dataset, 0, runattrs);
    UNPROTECT(1); // runattrs
    int index = 0;
    for (int i = 0; i < runCount; ++i)
    {
        Run* run = runList->at(i);
        SEXP runidSexp = mkChar(run->runName.c_str());
        for (StringMap::const_iterator it=run->attributes.begin(); it != run->attributes.end(); ++it)
        {
            const char *nameStr = it->first.c_str();
            const char *valueStr = it->second.c_str();
            SET_STRING_ELT(runid, index, runidSexp);
            SET_STRING_ELT(name, index, mkChar(nameStr));
            SET_STRING_ELT(value, index, mkChar(valueStr));
            index++;
        }
    }

    // fileruns
    FileRunList *filerunList = manager.getUniqueFileRuns(idlist);
    int filerunCount = filerunList->size();
    SEXP fileruns = createDataFrame(filerunColumnNames, filerunColumnTypes, filerunColumnsLength, filerunCount);
    runid = VECTOR_ELT(fileruns, 0);
    SEXP file =VECTOR_ELT(fileruns, 1);
    SET_ELEMENT(dataset, 1, fileruns);
    UNPROTECT(1); // fileruns
    for (int i = 0; i < filerunCount; ++i)
    {
        FileRun *filerunPtr = filerunList->at(i);
        Run *runPtr = filerunPtr->runRef;
        const char *fileStr = filerunPtr->fileRef->fileSystemFilePath.c_str();
        SET_STRING_ELT(runid, i, mkChar(runPtr->runName.c_str()));
        SET_STRING_ELT(file, i, mkChar(fileStr));
    }

    // scalars
    IDList scalarIDs = filterIDListByType(idlist, ResultFileManager::SCALAR, manager);
    int scalarCount = scalarIDs.size();
    int scalarKeyStart = 0;
    SEXP scalars = createDataFrame(scalarColumnNames, scalarColumnTypes, scalarColumnsLength, scalarCount);
    SEXP resultKey = VECTOR_ELT(scalars, 0);
    runid = VECTOR_ELT(scalars, 1);
    file = VECTOR_ELT(scalars, 2);
    SEXP module = VECTOR_ELT(scalars, 3);
    name = VECTOR_ELT(scalars, 4);
    value = VECTOR_ELT(scalars, 5);
    SET_ELEMENT(dataset, 2, scalars);
    UNPROTECT(1); // scalars
    for (int i = 0; i < scalarCount; ++i)
    {
        ID id = scalarIDs.get(i);
        const ScalarResult &scalar = manager.getScalar(id);
        attrCount += scalar.attributes.size();

        INTEGER(resultKey)[i] = scalarKeyStart + i;
        SET_STRING_ELT(runid, i, mkChar(scalar.fileRunRef->runRef->runName.c_str()));
        SET_STRING_ELT(file, i, mkChar(scalar.fileRunRef->fileRef->fileSystemFilePath.c_str()));
        SET_STRING_ELT(module, i, mkChar(scalar.moduleNameRef->c_str()));
        SET_STRING_ELT(name, i, mkChar(scalar.nameRef->c_str()));
        REAL(value)[i] = scalar.value;
    }

    // vectors
    IDList vectorIDs = filterIDListByType(idlist, ResultFileManager::VECTOR, manager);
    int vectorCount = vectorIDs.size();
    int vectorKeyStart = scalarKeyStart + scalarCount;
    SEXP vectors = createDataFrame(vectorColumnNames, vectorColumnTypes, vectorColumnsLength, vectorCount);
    resultKey = VECTOR_ELT(vectors, 0);
    runid = VECTOR_ELT(vectors, 1);
    file = VECTOR_ELT(vectors, 2);
    SEXP vectorid = VECTOR_ELT(vectors, 3);
    module = VECTOR_ELT(vectors, 4);
    name = VECTOR_ELT(vectors, 5);
    SET_ELEMENT(dataset, 3, vectors);
    UNPROTECT(1); // vectors
    for (int i = 0; i < vectorCount; ++i)
    {
        ID id = vectorIDs.get(i);
        const VectorResult &vector = manager.getVector(id);
        attrCount += vector.attributes.size();

        INTEGER(resultKey)[i] = vectorKeyStart + i;
        SET_STRING_ELT(runid, i, mkChar(vector.fileRunRef->runRef->runName.c_str()));
        SET_STRING_ELT(file, i, mkChar(vector.fileRunRef->fileRef->fileSystemFilePath.c_str()));
        INTEGER(vectorid)[i] = vector.vectorId;
        SET_STRING_ELT(module, i, mkChar(vector.moduleNameRef->c_str()));
        SET_STRING_ELT(name, i, mkChar(vector.nameRef->c_str()));
    }

    // statistics
    IDList statisticIDs = filterIDListByType(idlist, ResultFileManager::HISTOGRAM, manager);
    int statisticCount = statisticIDs.size();
    int statisticKeyStart = vectorKeyStart + vectorCount;
    SEXP statistics = createDataFrame(statisticColumnNames, statisticColumnTypes, statisticColumnsLength, statisticCount);
    resultKey = VECTOR_ELT(statistics, 0);
    runid = VECTOR_ELT(statistics, 1);
    file = VECTOR_ELT(statistics, 2);
    module = VECTOR_ELT(statistics, 3);
    name = VECTOR_ELT(statistics, 4);
    SET_ELEMENT(dataset, 4, statistics);
    UNPROTECT(1); // statistics
    int binCount = 0, fieldCount = 0;
    for (int i = 0; i < statisticCount; ++i)
    {
        ID id = statisticIDs.get(i);
        const HistogramResult &statistic = manager.getHistogram(id);
        binCount += statistic.bins.size();
        fieldCount += statistic.fields.size();
        attrCount += statistic.attributes.size();

        INTEGER(resultKey)[i] = statisticKeyStart + i;
        SET_STRING_ELT(runid, i, mkChar(statistic.fileRunRef->runRef->runName.c_str()));
        SET_STRING_ELT(file, i, mkChar(statistic.fileRunRef->fileRef->fileSystemFilePath.c_str()));
        SET_STRING_ELT(module, i, mkChar(statistic.moduleNameRef->c_str()));
        SET_STRING_ELT(name, i, mkChar(statistic.nameRef->c_str()));
    }

    // fields
    SEXP fields = createDataFrame(fieldColumnNames, fieldColumnTypes, fieldColumnsLength, fieldCount);
    resultKey = VECTOR_ELT(fields, 0);
    name = VECTOR_ELT(fields, 1);
    value = VECTOR_ELT(fields, 2);
    SET_ELEMENT(dataset, 5, fields);
    UNPROTECT(1); // fields
    index = 0;
    for (int i = 0; i < statisticCount; ++i)
    {
        ID id = statisticIDs.get(i);
        const HistogramResult &statistic = manager.getHistogram(id);
        const HistogramFields &fields = statistic.fields;

        for(HistogramFields::const_iterator it = fields.begin(); it != fields.end(); ++it)
        {
            INTEGER(resultKey)[index] = statisticKeyStart + i;
            SET_STRING_ELT(name, index, mkChar(it->first.c_str()));
            REAL(value)[index] = it->second;
            index++;
        }
    }

    // bins
    SEXP bins = createDataFrame(binColumnNames, binColumnTypes, binColumnsLength, binCount);
    resultKey = VECTOR_ELT(bins, 0);
    SEXP lowerbound=VECTOR_ELT(bins, 1), upperbound=VECTOR_ELT(bins, 2), count=VECTOR_ELT(bins, 3);
    SET_ELEMENT(dataset, 6, bins);
    UNPROTECT(1); // bins
    index = 0;
    for (int i = 0; i < statisticCount; ++i)
    {
        ID id = statisticIDs.get(i);
        const HistogramResult &statistic = manager.getHistogram(id);
        int size = statistic.bins.size();
        for (int j = 0; j < size; ++j)
        {
            INTEGER(resultKey)[index] = statisticKeyStart + i;
            REAL(lowerbound)[index] = statistic.bins[j];
            REAL(upperbound)[index] = j+1 < size ? statistic.bins[j+1] : R_PosInf;
            REAL(count)[index] = statistic.values[j];
            index++;
        }
    }

    // params
    SEXP params = createDataFrame(paramColumnNames, paramColumnTypes, paramColumnsLength, paramsCount);
    runid = VECTOR_ELT(params, 0);
    name = VECTOR_ELT(params, 1);
    value = VECTOR_ELT(params, 2);
    SET_ELEMENT(dataset, 7, params);
    UNPROTECT(1); // params
    index = 0;
    for (int i = 0; i < runCount; ++i)
    {
        Run* run = runList->at(i);
        SEXP runidSexp = mkChar(run->runName.c_str());
        for (StringMap::const_iterator it=run->moduleParams.begin(); it != run->moduleParams.end(); ++it)
        {
            const char *nameStr = it->first.c_str();
            const char *valueStr = it->second.c_str();
            SET_STRING_ELT(runid, index, runidSexp);
            SET_STRING_ELT(name, index, mkChar(nameStr));
            SET_STRING_ELT(value, index, mkChar(valueStr));
            index++;
        }
    }

    // attrs
    SEXP attrs = createDataFrame(attributeColumnNames, attributeColumnTypes, attributeColumnsLength, attrCount);
    SET_ELEMENT(dataset, 8, attrs);
    UNPROTECT(1); // attrs
    index = 0;
    addResultItemAttributes(attrs, index, "scalar", scalarIDs, scalarKeyStart, manager);
    addResultItemAttributes(attrs, index, "vector", vectorIDs, vectorKeyStart, manager);
    addResultItemAttributes(attrs, index, "statistic", statisticIDs, statisticKeyStart, manager);

    UNPROTECT(1); // dataset

    delete runList;
    delete filerunList;

    return dataset;
}

SEXP callLoadDataset(SEXP files, SEXP commands)
{
    try
    {
        ResultFileManager manager;
        IDList idlist;
        SEXP dataset;

        executeCommands(files, commands, manager, idlist);
        dataset = exportDataset(manager, idlist);
        return dataset;
    }
    catch (opp_runtime_error &e)
    {
        error("Error in callLoadDataset: %s\n", e.what());
        return R_NilValue;
    }
}
