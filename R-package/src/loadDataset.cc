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

#include "resultfilemanager.h"
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
    else if (strcmp(typeStr, "histogram") == 0)
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

static IDList selectIDs(const ResultFileList *files, int type, const char *pattern, const ResultFileManager &manager)
{
    FileRunList fileRuns = manager.getFileRuns(files, NULL);

    IDList source;
    for (FileRunList::iterator it = fileRuns.begin(); it != fileRuns.end(); ++it)
    {
        FileRun *fileRun = *it;
        if ((type & ResultFileManager::SCALAR) != 0)
        {
            IDList scalars = manager.getScalarsInFileRun(fileRun);
            source.merge(scalars);
        }
        if ((type & ResultFileManager::VECTOR) != 0)
        {
            IDList vectors = manager.getVectorsInFileRun(fileRun);
            source.merge(vectors);
        }
        if ((type & ResultFileManager::HISTOGRAM) != 0)
        {
            IDList histograms = manager.getHistogramsInFileRun(fileRun);
            source.merge(histograms);
        }
    }
    IDList idlist = manager.filterIDList(source, pattern);
    return idlist;
}

static void executeCommands(SEXP commands, ResultFileManager &manager, IDList &out)
{
    if (!IS_VECTOR(commands))
    {
        throw opp_runtime_error("commands is not a list");
    }

    int numOfCommands = GET_LENGTH(commands);
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
            SEXP files = getElementByName(command, "files");
            ResultFileList fileList = loadFiles(files, manager);
            IDList selectedIDs = selectIDs(&fileList, type, selectStr, manager);
            out.merge(selectedIDs);
        }
        else if (strcmp(opname, "discard") == 0)
        {
            IDList selectedIDs = selectIDs(NULL, type, selectStr, manager);
            out.substract(selectedIDs);
        }
        else
        {
            throw opp_runtime_error("unknown command: %s", opname);
        }
    }
}

static void addRunAttributes(SEXP attributes, int &currentIndex, const RunList &runs)
{
    SEXP types=VECTOR_ELT(attributes, 0), keys=VECTOR_ELT(attributes, 1), names=VECTOR_ELT(attributes, 2), values=VECTOR_ELT(attributes, 3);
    SEXP typeSEXP = mkChar("run"); // immutable, so can be shared
    for (int i = 0; i < runs.size(); ++i)
    {
        Run* run = runs[i];
        for (StringMap::const_iterator it=run->attributes.begin(); it != run->attributes.end(); ++it)
        {
            const char *nameStr = it->first.c_str();
            const char *valueStr = it->second.c_str();
            SET_STRING_ELT(types, currentIndex, typeSEXP);
            INTEGER(keys)[currentIndex] = i;
            SET_STRING_ELT(names, currentIndex, mkChar(nameStr));
            SET_STRING_ELT(values, currentIndex, mkChar(valueStr));
            currentIndex++;
        }
    }
}

static void addResultItemAttributes(SEXP attributes, int &currentIndex, const char *type, const IDList &ids, const ResultFileManager &manager)
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
            INTEGER(keys)[currentIndex] = i;
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

static const char* datasetColumnNames[] = {"runs", "files", "scalars", "vectors", "histograms", "fields", "bins", "params", "attributes"};
static const int datasetColumnsLength = sizeof(datasetColumnNames) / sizeof(const char*);

const char* runColumnNames[] = {"run_key", "runid", "experiment", "measurement", "replication"};
const SEXPTYPE runColumnTypes[] = {INTSXP, STRSXP, STRSXP, STRSXP, STRSXP};
const int runColumnsLength = sizeof(runColumnNames) / sizeof(const char*);

const char* fileColumnNames[] = {"file_key", "directory", "filename", "ospath"};
const SEXPTYPE fileColumnTypes[] = {INTSXP, STRSXP, STRSXP, STRSXP};
const int fileColumnsLength = sizeof(fileColumnNames) / sizeof(const char*);

const char* scalarColumnNames[] = {"scalar_key", "run_key", "file_key", "module", "name", "value"};
const SEXPTYPE scalarColumnTypes[] = {INTSXP, INTSXP, INTSXP, STRSXP, STRSXP, REALSXP};
const int scalarColumnsLength = sizeof(scalarColumnNames) / sizeof(const char*);

const char* vectorColumnNames[] = {"vector_key", "run_key", "file_key", "vectorid", "module", "name"};
const SEXPTYPE vectorColumnTypes[] = {INTSXP, INTSXP, INTSXP, INTSXP, STRSXP, STRSXP};
const int vectorColumnsLength = sizeof(vectorColumnNames) / sizeof(const char*);

const char* histogramColumnNames[] = {"histogram_key", "run_key", "file_key", "module", "name"};
const SEXPTYPE histogramColumnTypes[] = {INTSXP, INTSXP, INTSXP, STRSXP, STRSXP};
const int histogramColumnsLength = sizeof(histogramColumnNames) / sizeof(const char*);

const char* fieldColumnNames[] = {"histogram_key", "name", "value"};
const SEXPTYPE fieldColumnTypes[] = {INTSXP, STRSXP, REALSXP};
const int fieldColumnsLength = sizeof(fieldColumnNames) / sizeof(const char*);

const char* binColumnNames[] = {"histogram_key", "lowerbound", "upperbound", "count"};
const SEXPTYPE binColumnTypes[] = {INTSXP, REALSXP, REALSXP, REALSXP};
const int binColumnsLength = sizeof(binColumnNames) / sizeof(const char*);

const char* paramColumnNames[] = {"run_key", "name", "value"};
const SEXPTYPE paramColumnTypes[] = {INTSXP, STRSXP, STRSXP};
const int paramColumnsLength = sizeof(paramColumnNames) / sizeof(const char*);

const char* attributeColumnNames[] = {"type", "key", "name", "value"};
const SEXPTYPE attributeColumnTypes[] = {STRSXP, INTSXP, STRSXP, STRSXP};
const int attributeColumnsLength = sizeof(attributeColumnNames) / sizeof(const char*);

SEXP exportDataset(ResultFileManager &manager, const IDList &idlist)
{
    int paramsCount = 0, attrCount = 0;

    SEXP dataset, names;
    PROTECT(dataset = NEW_LIST(9));
    setNames(dataset, datasetColumnNames, datasetColumnsLength);

    // runs
    std::map<Run*,int> runIndex;
    const RunList &runList = manager.getRuns();
    int runCount = runList.size();
    SEXP runs = createDataFrame(runColumnNames, runColumnTypes, runColumnsLength, runCount);
    SEXP runKey=VECTOR_ELT(runs,0), runId=VECTOR_ELT(runs,1), experiment=VECTOR_ELT(runs,2), measurement=VECTOR_ELT(runs,3), replication=VECTOR_ELT(runs,4);
    SET_ELEMENT(dataset, 0, runs);
    UNPROTECT(1); // runs
    for(int i = 0; i < runCount; ++i)
    {
        Run *runPtr = runList[i];
        runIndex[runPtr] = i;
        paramsCount += runPtr->moduleParams.size();
        attrCount += runPtr->attributes.size();

        const char *runIdStr = runPtr->runName.c_str();
        const char *experimentStr = runPtr->getAttribute("experiment");
        const char *measurementStr = runPtr->getAttribute("measurement");
        const char *replicationStr = runPtr->getAttribute("replication");
        INTEGER(runKey)[i] = i;
        SET_STRING_ELT(runId, i, mkChar(runIdStr));
        SET_STRING_ELT(experiment, i, (experimentStr != NULL ? mkChar(experimentStr) : NA_STRING));
        SET_STRING_ELT(measurement, i, (measurementStr != NULL ? mkChar(measurementStr) : NA_STRING));
        SET_STRING_ELT(replication, i, (replicationStr != NULL ? mkChar(replicationStr) : NA_STRING));
    }

    // files
    std::map<ResultFile*,int> fileIndex;
    const ResultFileList &fileList = manager.getFiles();
    int fileCount = fileList.size();
    SEXP files = createDataFrame(fileColumnNames, fileColumnTypes, fileColumnsLength, fileCount);
    SEXP fileKey = VECTOR_ELT(files, 0), directory = VECTOR_ELT(files,1), fileName=VECTOR_ELT(files, 2), ospath=VECTOR_ELT(files, 3);
    SET_ELEMENT(dataset, 1, files);
    UNPROTECT(1); // files
    for (int i = 0; i < fileCount; ++i)
    {
        ResultFile *filePtr = fileList[i];
        fileIndex[filePtr] = i;
        const char *directoryStr = filePtr->directory.c_str();
        const char *filenameStr = filePtr->fileName.c_str();
        const char *ospathStr = filePtr->fileSystemFilePath.c_str();
        INTEGER(fileKey)[i] = i;
        SET_STRING_ELT(directory, i, mkChar(directoryStr));
        SET_STRING_ELT(fileName, i, mkChar(filenameStr));
        SET_STRING_ELT(ospath, i, mkChar(ospathStr));
    }

    // scalars
    IDList scalarIDs = filterIDListByType(idlist, ResultFileManager::SCALAR, manager);
    int scalarCount = scalarIDs.size();
    SEXP scalars = createDataFrame(scalarColumnNames, scalarColumnTypes, scalarColumnsLength, scalarCount);
    SEXP scalarKey = VECTOR_ELT(scalars, 0);
    runKey = VECTOR_ELT(scalars, 1);
    fileKey = VECTOR_ELT(scalars, 2);
    SEXP module = VECTOR_ELT(scalars, 3);
    SEXP name = VECTOR_ELT(scalars, 4);
    SEXP value = VECTOR_ELT(scalars, 5);
    SET_ELEMENT(dataset, 2, scalars);
    UNPROTECT(1); // scalars
    for (int i = 0; i < scalarCount; ++i)
    {
        ID id = scalarIDs.get(i);
        const ScalarResult &scalar = manager.getScalar(id);
        attrCount += scalar.attributes.size();

        INTEGER(scalarKey)[i] = i;
        INTEGER(runKey)[i] = runIndex.find(scalar.fileRunRef->runRef)->second;
        INTEGER(fileKey)[i] = fileIndex.find(scalar.fileRunRef->fileRef)->second;
        SET_STRING_ELT(module, i, mkChar(scalar.moduleNameRef->c_str()));
        SET_STRING_ELT(name, i, mkChar(scalar.nameRef->c_str()));
        REAL(value)[i] = scalar.value;
    }

    // vectors
    IDList vectorIDs = filterIDListByType(idlist, ResultFileManager::VECTOR, manager);
    int vectorCount = vectorIDs.size();
    SEXP vectors = createDataFrame(vectorColumnNames, vectorColumnTypes, vectorColumnsLength, vectorCount);
    SEXP vectorKey = VECTOR_ELT(vectors, 0);
    runKey = VECTOR_ELT(vectors, 1);
    fileKey = VECTOR_ELT(vectors, 2);
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

        INTEGER(vectorKey)[i] = i;
        INTEGER(runKey)[i] = runIndex.find(vector.fileRunRef->runRef)->second;
        INTEGER(fileKey)[i] = fileIndex.find(vector.fileRunRef->fileRef)->second;
        INTEGER(vectorid)[i] = vector.vectorId;
        SET_STRING_ELT(module, i, mkChar(vector.moduleNameRef->c_str()));
        SET_STRING_ELT(name, i, mkChar(vector.nameRef->c_str()));
    }

    // histograms
    IDList histogramIDs = filterIDListByType(idlist, ResultFileManager::HISTOGRAM, manager);
    int histogramCount = histogramIDs.size();
    SEXP histograms = createDataFrame(histogramColumnNames, histogramColumnTypes, histogramColumnsLength, histogramCount);
    SEXP histogramKey = VECTOR_ELT(histograms, 0);
    runKey = VECTOR_ELT(histograms, 1);
    fileKey = VECTOR_ELT(histograms, 2);
    module = VECTOR_ELT(histograms, 3);
    name = VECTOR_ELT(histograms, 4);
    SET_ELEMENT(dataset, 4, histograms);
    UNPROTECT(1); // histograms
    int binCount = 0, fieldCount = 0;
    for (int i = 0; i < histogramCount; ++i)
    {
        ID id = histogramIDs.get(i);
        const HistogramResult &histogram = manager.getHistogram(id);
        binCount += histogram.bins.size();
        fieldCount += histogram.fields.size();
        attrCount += histogram.attributes.size();

        INTEGER(histogramKey)[i] = i;
        INTEGER(runKey)[i] = runIndex.find(histogram.fileRunRef->runRef)->second;
        INTEGER(fileKey)[i] = fileIndex.find(histogram.fileRunRef->fileRef)->second;
        SET_STRING_ELT(module, i, mkChar(histogram.moduleNameRef->c_str()));
        SET_STRING_ELT(name, i, mkChar(histogram.nameRef->c_str()));
    }

    // fields
    SEXP fields = createDataFrame(fieldColumnNames, fieldColumnTypes, fieldColumnsLength, fieldCount);
    histogramKey = VECTOR_ELT(fields, 0);
    name = VECTOR_ELT(fields, 1);
    value = VECTOR_ELT(fields, 2);
    SET_ELEMENT(dataset, 5, fields);
    UNPROTECT(1); // fields
    int index = 0;
    for (int i = 0; i < histogramCount; ++i)
    {
        ID id = histogramIDs.get(i);
        const HistogramResult &histogram = manager.getHistogram(id);
        const HistogramFields &fields = histogram.fields;

        for(HistogramFields::const_iterator it = fields.begin(); it != fields.end(); ++it)
        {
            INTEGER(histogramKey)[index] = i;
            SET_STRING_ELT(name, index, mkChar(it->first.c_str()));
            REAL(value)[index] = it->second;
            index++;
        }
    }

    // bins
    SEXP bins = createDataFrame(binColumnNames, binColumnTypes, binColumnsLength, binCount);
    histogramKey = VECTOR_ELT(bins, 0);
    SEXP lowerbound=VECTOR_ELT(bins, 1), upperbound=VECTOR_ELT(bins, 2), count=VECTOR_ELT(bins, 3);
    SET_ELEMENT(dataset, 6, bins);
    UNPROTECT(1); // bins
    index = 0;
    for (int i = 0; i < histogramCount; ++i)
    {
        ID id = histogramIDs.get(i);
        const HistogramResult &histogram = manager.getHistogram(id);
        int size = histogram.bins.size();
        for (int j = 0; j < size; ++j)
        {
            INTEGER(histogramKey)[index] = i;
            REAL(lowerbound)[index] = histogram.bins[j];
            REAL(upperbound)[index] = j+1 < size ? histogram.bins[j+1] : R_PosInf;
            REAL(count)[index] = histogram.values[j];
            index++;
        }
    }

    // params
    SEXP params = createDataFrame(paramColumnNames, paramColumnTypes, paramColumnsLength, paramsCount);
    runKey = VECTOR_ELT(params, 0);
    name = VECTOR_ELT(params, 1);
    value = VECTOR_ELT(params, 2);
    SET_ELEMENT(dataset, 7, params);
    UNPROTECT(1); // params
    index = 0;
    for (int i = 0; i < runCount; ++i)
    {
        Run* run = runList[i];
        for (StringMap::const_iterator it=run->moduleParams.begin(); it != run->moduleParams.end(); ++it)
        {
            const char *nameStr = it->first.c_str();
            const char *valueStr = it->second.c_str();
            INTEGER(runKey)[index] = i;
            SET_STRING_ELT(name, index, mkChar(nameStr));
            SET_STRING_ELT(value, index, mkChar(valueStr));
            index++;
        }
    }

    // attributes
    SEXP attributes = createDataFrame(attributeColumnNames, attributeColumnTypes, attributeColumnsLength, attrCount);
    SET_ELEMENT(dataset, 8, attributes);
    UNPROTECT(1); // attributes
    index = 0;
    addRunAttributes(attributes, index, runList);
    addResultItemAttributes(attributes, index, "scalar", scalarIDs, manager);
    addResultItemAttributes(attributes, index, "vector", vectorIDs, manager);
    addResultItemAttributes(attributes, index, "histogram", histogramIDs, manager);

    UNPROTECT(1); // dataset

    return dataset;
}

SEXP callLoadDataset(SEXP commands)
{
    try
    {
        ResultFileManager manager;
        IDList idlist;
        SEXP dataset;

        executeCommands(commands, manager, idlist);
        dataset = exportDataset(manager, idlist);
        return dataset;
    }
    catch (opp_runtime_error &e)
    {
        error("Error in callLoadDataset: %s\n", e.what());
        return R_NilValue;
    }
}
