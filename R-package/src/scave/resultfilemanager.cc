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

#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>
#include <algorithm>
#include <utility>
#include <functional>
#include "opp_ctype.h"
#include "platmisc.h"
#include "matchexpression.h"
#include "patternmatcher.h"
#include "filereader.h"
#include "linetokenizer.h"
#include "stringtokenizer.h"
#include "filereader.h"
#include "indexfile.h"
#include "scaveutils.h"
#include "scaveexception.h"
#include "resultfilemanager.h"
#include "fileutil.h"
#include "commonutil.h"
#include "stringutil.h"

#ifdef THREADED
#define READER_MUTEX Mutex __reader_mutex_(getReadLock());
#define WRITER_MUTEX Mutex __writer_mutex_(getWriteLock());
#else
#define READER_MUTEX
#define WRITER_MUTEX
#endif

USING_NAMESPACE

ResultItem::Type ResultItem::getType() const
{
    StringMap::const_iterator it = attributes.find("type");
    if (it == attributes.end())
    {
        if (attributes.find("enum") != attributes.end())
            return TYPE_ENUM;
        else
            return TYPE_DOUBLE;
    }
    else
    {
        const std::string &type = it->second;
        if (type == "int")
            return TYPE_INT;
        else if (type == "double")
            return TYPE_DOUBLE;
        else if (type == "enum")
            return TYPE_ENUM;
        else
            throw opp_runtime_error("Unknown type: %s", type.c_str());
    }
}

EnumType* ResultItem::getEnum() const
{
    StringMap::const_iterator it = attributes.find("enum");
    if (it != attributes.end())
    {
        EnumType *enumPtr = new EnumType();
        enumPtr->parseFromString(it->second.c_str());
        return enumPtr;
    }
    else
    {
        return NULL;
    }
}

InterpolationMode VectorResult::getInterpolationMode() const
{
    StringMap::const_iterator it = attributes.find("interpolationmode");
    if (it != attributes.end())
    {
        const std::string &mode = it->second;
        if (mode == "none")
            return NONE;
        else if (mode == "sample-hold")
            return SAMPLE_HOLD;
        else if (mode == "backward-sample-hold")
            return BACKWARD_SAMPLE_HOLD;
        else if (mode == "linear")
            return LINEAR;
        else
            throw opp_runtime_error("Unknown interpolation mode: %s", mode.c_str());
    }
    else
    {
        return UNSPECIFIED;
    }
}

ResultFileManager::ResultFileManager()
{
}

ResultFileManager::~ResultFileManager()
{
    for (int i=0; i<(int)fileRunList.size(); i++)
        delete fileRunList[i];

    for (int i=0; i<(int)runList.size(); i++)
        delete runList[i];

    for (int i=0; i<(int)fileList.size(); i++)
        delete fileList[i];

    fileRunList.clear();
    runList.clear();
    fileList.clear();

    moduleNames.clear();
    names.clear();
    classNames.clear();
}

ResultFileList ResultFileManager::getFiles() const
{
    READER_MUTEX
    ResultFileList out;
    for (int i=0; i<(int)fileList.size(); i++)
        if (fileList[i] && !fileList[i]->computed)
            out.push_back(fileList[i]);
    return out;
}

RunList ResultFileManager::getRunsInFile(ResultFile *file) const
{
    READER_MUTEX
    RunList out;
    for (int i=0; i<(int)fileRunList.size(); i++)
        if (fileRunList[i]->fileRef == file)
            out.push_back(fileRunList[i]->runRef);
    return out;
}

ResultFileList ResultFileManager::getFilesForRun(Run *run) const
{
    READER_MUTEX
    ResultFileList out;
    for (int i=0; i<(int)fileRunList.size(); i++)
        if (fileRunList[i]->runRef == run && !fileRunList[i]->fileRef->computed)
            out.push_back(fileRunList[i]->fileRef);
    return out;
}

const ResultItem& ResultFileManager::getItem(ID id) const
{
    READER_MUTEX
    try
    {
        switch (_type(id))
        {
            case SCALAR: return getFileForID(id)->scalarResults.at(_pos(id));
            case VECTOR: return getFileForID(id)->vectorResults.at(_pos(id));
            case HISTOGRAM: return getFileForID(id)->histogramResults.at(_pos(id));
            default: throw opp_runtime_error("ResultFileManager: invalid ID: wrong type");
        }
    }
    catch (std::out_of_range e)
    {
        throw opp_runtime_error("ResultFileManager::getItem(id): invalid ID");
    }
}

ResultFileList *ResultFileManager::getUniqueFiles(const IDList& ids) const
{
    READER_MUTEX
    // collect unique runs in this dataset
    std::set<ResultFile*> set;
    for (int i=0; i<ids.size(); i++)
        set.insert(getItem(ids.get(i)).fileRunRef->fileRef);

    // convert to list for easier handling at recipient
    return new ResultFileList(set.begin(), set.end());
}

RunList *ResultFileManager::getUniqueRuns(const IDList& ids) const
{
    READER_MUTEX
    // collect unique runs in this dataset
    std::set<Run*> set;
    for (int i=0; i<ids.size(); i++)
        set.insert(getItem(ids.get(i)).fileRunRef->runRef);

    // convert to list for easier handling at recipient
    return new RunList(set.begin(), set.end());
}

FileRunList *ResultFileManager::getUniqueFileRuns(const IDList& ids) const
{
    READER_MUTEX
    // collect unique FileRuns in this dataset
    std::set<FileRun*> set;
    for (int i=0; i<ids.size(); i++)
        set.insert(getItem(ids.get(i)).fileRunRef);

    // convert to list for easier handling at recipient
    return new FileRunList(set.begin(), set.end());
}

StringSet *ResultFileManager::getUniqueModuleNames(const IDList& ids) const
{
    READER_MUTEX
    // collect unique module names in this dataset
    StringSet *set = new StringSet();
    for (int i=0; i<ids.size(); i++)
        set->insert(*getItem(ids.get(i)).moduleNameRef);
    return set;
}

StringSet *ResultFileManager::getUniqueNames(const IDList& ids) const
{
    READER_MUTEX
    // collect unique scalar/vector names in this dataset
    StringSet *set = new StringSet();
    for (int i=0; i<ids.size(); i++)
        set->insert(*getItem(ids.get(i)).nameRef);
    return set;
}

StringSet *ResultFileManager::getUniqueAttributeNames(const IDList &ids) const
{
    READER_MUTEX
    StringSet *set = new StringSet;
    for (int i=0; i<ids.size(); i++)
    {
        const StringMap &attributes = getItem(ids.get(i)).attributes;
        for(StringMap::const_iterator it = attributes.begin(); it != attributes.end(); ++it)
            set->insert(it->first);
    }
    return set;
}

StringSet *ResultFileManager::getUniqueRunAttributeNames(const RunList *runList) const
{
    READER_MUTEX
    StringSet *set = new StringSet;
    for (RunList::const_iterator runRef = runList->begin(); runRef != runList->end(); ++runRef)
    {
        const StringMap &attributes = (*runRef)->attributes;
        for(StringMap::const_iterator it = attributes.begin(); it != attributes.end(); ++it)
            set->insert(it->first);
    }
    return set;
}

StringSet *ResultFileManager::getUniqueModuleParamNames(const RunList *runList) const
{
    READER_MUTEX
    StringSet *set = new StringSet;
    for (RunList::const_iterator runRef = runList->begin(); runRef != runList->end(); ++runRef)
    {
        const StringMap &params = (*runRef)->moduleParams;
        for(StringMap::const_iterator it = params.begin(); it != params.end(); ++it)
            set->insert(it->first);
    }
    return set;
}

StringSet *ResultFileManager::getUniqueAttributeValues(const IDList &ids, const char *attrName) const
{
    READER_MUTEX
    StringSet *values = new StringSet;
    for (int i = 0; i < ids.size(); ++i)
    {
        const char *value = getItem(ids.get(i)).getAttribute(attrName);
        if (value != NULL)
            values->insert(value);
    }
    return values;
}

StringSet *ResultFileManager::getUniqueRunAttributeValues(const RunList& runList, const char *attrName) const
{
    READER_MUTEX
    StringSet *values = new StringSet;
    for (RunList::const_iterator runRef = runList.begin(); runRef != runList.end(); ++runRef)
    {
        const char *value = (*runRef)->getAttribute(attrName);
        if (value!=NULL)
            values->insert(value);
    }

    return values;
}

StringSet *ResultFileManager::getUniqueModuleParamValues(const RunList& runList, const char *paramName) const
{
    READER_MUTEX
    StringSet *values = new StringSet;
    for (RunList::const_iterator runRef = runList.begin(); runRef != runList.end(); ++runRef)
    {
        const char *value = (*runRef)->getModuleParam(paramName);
        if (value != NULL)
            values->insert(value);
    }

    return values;
}

const ScalarResult& ResultFileManager::getScalar(ID id) const
{
    READER_MUTEX
    if (_type(id)!=SCALAR)
        throw opp_runtime_error("ResultFileManager::getScalar(id): this item is not a scalar");
    return getFileForID(id)->scalarResults.at(_pos(id));
}

const VectorResult& ResultFileManager::getVector(ID id) const
{
    READER_MUTEX
    if (_type(id)!=VECTOR)
        throw opp_runtime_error("ResultFileManager::getVector(id): this item is not a vector");
    return getFileForID(id)->vectorResults.at(_pos(id));
}

const HistogramResult& ResultFileManager::getHistogram(ID id) const
{
    READER_MUTEX
    if (_type(id)!=HISTOGRAM)
        throw opp_runtime_error("ResultFileManager::getHistogram(id): this item is not a histogram");
    return getFileForID(id)->histogramResults.at(_pos(id));
}

template <class T>
void ResultFileManager::collectIDs(IDList &out, std::vector<T> ResultFile::* vec, int type, bool includeComputed, bool includeFields) const
{
    for (int k=0; k<(int)fileList.size(); k++)
    {
        if (fileList[k]!=NULL)
        {
            std::vector<T>& v = fileList[k]->*vec;
            for (int i=0; i<(int)v.size(); i++) {
                bool isComputed = v[i].isComputed();
                bool isField = type == SCALAR ? ((ScalarResult&)v[i]).isField : false;

                if ((!isField || includeFields) && (!isComputed || includeComputed))
                    out.uncheckedAdd(_mkID(false, isField, type, k, i));
            }
        }
    }
}

IDList ResultFileManager::getAllItems(bool includeComputed, bool includeFields) const
{
    READER_MUTEX
    IDList out;
    collectIDs(out, &ResultFile::scalarResults, SCALAR, includeComputed, includeFields);
    collectIDs(out, &ResultFile::vectorResults, VECTOR, includeComputed, includeFields);
    collectIDs(out, &ResultFile::histogramResults, HISTOGRAM, includeComputed, includeFields);
    return out;
}

IDList ResultFileManager::getAllScalars(bool includeComputed, bool includeFields) const
{
    READER_MUTEX
    IDList out;
    collectIDs(out, &ResultFile::scalarResults, SCALAR, includeComputed, includeFields);
    return out;
}

IDList ResultFileManager::getAllVectors(bool includeComputed) const
{
    READER_MUTEX
    IDList out;
    collectIDs(out, &ResultFile::vectorResults, VECTOR, includeComputed);
    return out;
}

IDList ResultFileManager::getAllHistograms(bool includeComputed) const
{
    READER_MUTEX
    IDList out;
    collectIDs(out, &ResultFile::histogramResults, HISTOGRAM, includeComputed);
    return out;
}

IDList ResultFileManager::getScalarsInFileRun(FileRun *fileRun) const
{
    READER_MUTEX
    IDList out;
    int fileId = fileRun->fileRef->id;
    ScalarResults& v = fileRun->fileRef->scalarResults;
    for (int i=0; i<(int)v.size(); i++)
        if (v[i].fileRunRef==fileRun && !v[i].isComputed())
            out.uncheckedAdd(_mkID(false,v[i].isField,SCALAR,fileId,i));
    return out;
}

IDList ResultFileManager::getVectorsInFileRun(FileRun *fileRun) const
{
    READER_MUTEX
    IDList out;
    int fileId = fileRun->fileRef->id;
    VectorResults& v = fileRun->fileRef->vectorResults;
    for (int i=0; i<(int)v.size(); i++)
        if (v[i].fileRunRef==fileRun && !v[i].isComputed())
            out.uncheckedAdd(_mkID(false,false,VECTOR,fileId,i));
    return out;
}

IDList ResultFileManager::getHistogramsInFileRun(FileRun *fileRun) const
{
    READER_MUTEX
    IDList out;
    int fileId = fileRun->fileRef->id;
    HistogramResults& v = fileRun->fileRef->histogramResults;
    for (int i=0; i<(int)v.size(); i++)
        if (v[i].fileRunRef==fileRun && !v[i].isComputed())
            out.uncheckedAdd(_mkID(false,false,HISTOGRAM,fileId,i));
    return out;
}

bool ResultFileManager::isFileLoaded(const char *fileName) const
{
    return getFile(fileName)!=NULL;
}

ResultFile *ResultFileManager::getFile(const char *fileName) const
{
    if (!fileName)
        return NULL;

    READER_MUTEX
    for (int i=0; i<(int)fileList.size(); i++)
        if (fileList[i]!=NULL && fileList[i]->filePath==fileName)
            return fileList[i];
    return NULL;
}

Run *ResultFileManager::getRunByName(const char *runName) const
{
    if (!runName)
        return NULL;

    READER_MUTEX
    for (int i=0; i<(int)runList.size(); i++)
        if (runList[i]->runName==runName)
            return runList[i];
    return NULL;
}

FileRun *ResultFileManager::getFileRun(ResultFile *file, Run *run) const
{
    READER_MUTEX
    for (int i=0; i<(int)fileRunList.size(); i++)
        if (fileRunList[i]->fileRef==file && fileRunList[i]->runRef==run)
            return fileRunList[i];
    return NULL;
}

// currently unused
ID ResultFileManager::getItemByName(FileRun *fileRunRef, const char *module, const char *name) const
{
    if (!fileRunRef || !module || !name)
        return 0;

    READER_MUTEX

    const std::string *moduleNameRef = moduleNames.find(module);
    if (!moduleNameRef)
        return 0;

    const std::string *nameRef = names.find(name);
    if (!nameRef)
        return 0;

    ScalarResults& scalarResults = fileRunRef->fileRef->scalarResults;
    for (int i=0; i<(int)scalarResults.size(); i++)
    {
        const ResultItem& d = scalarResults[i];
        if (d.moduleNameRef==moduleNameRef && d.nameRef==nameRef && d.fileRunRef==fileRunRef)
            return _mkID(d.isComputed(), scalarResults[i].isField, SCALAR, fileRunRef->fileRef->id, i);
    }

    VectorResults& vectorResults = fileRunRef->fileRef->vectorResults;
    for (int i=0; i<(int)vectorResults.size(); i++)
    {
        const ResultItem& d = vectorResults[i];
        if (d.moduleNameRef==moduleNameRef && d.nameRef==nameRef && d.fileRunRef==fileRunRef)
            return _mkID(d.isComputed(), false, VECTOR, fileRunRef->fileRef->id, i);
    }

    HistogramResults& histogramResults = fileRunRef->fileRef->histogramResults;
    for (int i=0; i<(int)histogramResults.size(); i++)
    {
        const ResultItem& d = histogramResults[i];
        if (d.moduleNameRef==moduleNameRef && d.nameRef==nameRef && d.fileRunRef==fileRunRef)
            return _mkID(d.isComputed(), false, HISTOGRAM, fileRunRef->fileRef->id, i);
    }
    return 0;
}

ID ResultFileManager::getVectorById(const ResultFile *fileRef, int vectorId) const
{
    if (!fileRef)
        return 0;

    READER_MUTEX

    const VectorResults& vectorResults = fileRef->vectorResults;
    for (int i=0; i<(int)vectorResults.size(); i++)
    {
        const VectorResult& vector = vectorResults[i];
        if (vector.vectorId == vectorId)
            return _mkID(vector.isComputed(), false, VECTOR, fileRef->id, i);
    }

    return 0;
}


RunList ResultFileManager::filterRunList(const RunList& runList,
                                         const char *runNameFilter,
                                         const StringMap& attrFilter) const
{
    READER_MUTEX

    RunList out;

    // runName matcher
    PatternMatcher runNamePattern(runNameFilter, false, true, true);

    // copy attributes to vectors for performance (std::map iterator is
    // infamously slow, and we need PatternMatchers as well)
    StringVector attrNames;
    std::vector<PatternMatcher> attrValues;
    for (StringMap::const_iterator it = attrFilter.begin(); it!=attrFilter.end(); ++it)
    {
        attrNames.push_back(it->first);
        attrValues.push_back(PatternMatcher(it->second.c_str(), false, true, true));
    }

    // do it
    for (int i=0; i<(int)runList.size(); i++)
    {
        Run *run = runList[i];
        if (!runNamePattern.matches(run->runName.c_str()))
            continue;
        bool matches = true;
        for (int j=0; j<(int)attrNames.size() && matches; j++)
        {
            const char *attrValue = run->getAttribute(attrNames[j].c_str());
            if (attrValue == NULL)
                attrValue = "";

            if (!attrValues[j].matches(attrValue))
            {
                matches = false;
                break;
            }
        }
        if (matches)
            out.push_back(run);
    }
    return out;
}

ResultFileList ResultFileManager::filterFileList(const ResultFileList& fileList,
                                                 const char *filePathFilter) const
{
    ResultFileList out;

    // filePath matcher
    PatternMatcher filePathPattern(filePathFilter, false, true, true);

    READER_MUTEX
    for (int i=0; i<(int)fileList.size(); i++)
    {
        ResultFile *file = fileList[i];
        if (!filePathPattern.matches(file->filePath.c_str()))
            continue;
        out.push_back(file);
    }
    return out;
}

FileRunList ResultFileManager::getFileRuns(const ResultFileList *fileList, const RunList *runList) const
{
    READER_MUTEX
    FileRunList out;
    for (int i=0; i<(int)fileRunList.size(); i++)
    {
        FileRun *fileRun = fileRunList[i];
        if (fileList && std::find(fileList->begin(), fileList->end(), fileRun->fileRef)==fileList->end())
            continue;
        if (runList && std::find(runList->begin(), runList->end(), fileRun->runRef)==runList->end())
            continue;
        out.push_back(fileRun);
    }
    return out;
}

IDList ResultFileManager::filterIDList(const IDList& idlist,
                                       const FileRunList *fileRunFilter,
                                       const char *moduleFilter,
                                       const char *nameFilter) const
{
    READER_MUTEX

    // "*" means no filtering, so ignore it
    if (!strcmp(moduleFilter,"*")) moduleFilter = "";
    if (!strcmp(nameFilter,"*")) nameFilter = "";

    // prepare for wildcard matches
    bool patMatchModule = PatternMatcher::containsWildcards(moduleFilter);
    bool patMatchName = PatternMatcher::containsWildcards(nameFilter);

    PatternMatcher *modulePattern = NULL;
    if (patMatchModule)
    {
        modulePattern = new PatternMatcher(moduleFilter, false, true, true); // case-sensitive full-string match
    }
    PatternMatcher *namePattern = NULL;
    if (patMatchName)
    {
        namePattern = new PatternMatcher(nameFilter, false, true, true); // case-sensitive full-string match
    }

    // TODO: if there's no wildcard, find string pointers in the stringsets
    // in advance, then we don't have to do strcmp().

    // iterate over all values and add matching ones to "out".
    // we can exploit the fact that ResultFileManager contains the data in the order
    // they were read from file, i.e. grouped by runs
    IDList out;
    FileRun *lastFileRunRef = NULL;
    bool lastFileRunMatched = false;
    int sz = idlist.size();
    for (int i=0; i<sz; i++)
    {
        ID id = idlist.get(i);
        const ResultItem& d = getItem(id);

        if (fileRunFilter)
        {
            if (lastFileRunRef!=d.fileRunRef)
            {
                lastFileRunRef = d.fileRunRef;
                lastFileRunMatched = std::find(fileRunFilter->begin(), fileRunFilter->end(), d.fileRunRef) != fileRunFilter->end();
            }
            if (!lastFileRunMatched)
                continue;
        }

        if (moduleFilter && moduleFilter[0] &&
            (patMatchModule ? !modulePattern->matches(d.moduleNameRef->c_str())
                            : strcmp(d.moduleNameRef->c_str(), moduleFilter))
           )
            continue; // no match

        if (nameFilter && nameFilter[0] &&
            (patMatchName ? !namePattern->matches(d.nameRef->c_str())
                          : strcmp(d.nameRef->c_str(), nameFilter))
           )
            continue; // no match

        // everything matched, insert it.
        // (note: uncheckedAdd() is fine: if input IDList didn't contain duplicates,
        // the result won't either)
        out.uncheckedAdd(id);
    }
    return out;
}

class MatchableResultItem : public MatchExpression::Matchable
{
    const ResultItem& item;

    public:
        MatchableResultItem(const ResultItem& item) : item(item) {}
        virtual const char *getDefaultAttribute() const;
        virtual const char *getAttribute(const char *name) const;
    private:
        const char *getName() const { return item.nameRef->c_str(); }
        const char *getModuleName() const { return item.moduleNameRef->c_str(); }
        const char *getFileName() const { return item.fileRunRef->fileRef->filePath.c_str(); }
        const char *getRunName() const { return item.fileRunRef->runRef->runName.c_str(); }
        const char *getResultItemAttribute(const char *attrName) const { return item.getAttribute(attrName); }
        const char *getRunAttribute(const char *attrName) const { return item.fileRunRef->runRef->getAttribute(attrName); }
        const char *getModuleParam(const char *paramName) const { return item.fileRunRef->runRef->getModuleParam(paramName); }
};

const char *MatchableResultItem::getDefaultAttribute() const
{
    return getName();
}

const char *MatchableResultItem::getAttribute(const char *name) const
{
    if (strcasecmp("name", name) == 0)
        return getName();
    else if (strcasecmp("module", name) == 0)
        return getModuleName();
    else if (strcasecmp("file", name) == 0)
        return getFileName();
    else if (strcasecmp("run", name) == 0)
        return getRunName();
    else if (strncasecmp("attr:", name, 5) == 0)
        return getRunAttribute(name+5);
    else if (strncasecmp("param:", name, 6) == 0)
        return getModuleParam(name+6);
    else
        return getResultItemAttribute(name);
}

IDList ResultFileManager::filterIDList(const IDList &idlist, const char *pattern) const
{
    if (pattern == NULL || pattern[0] == '\0') // no filter
        pattern = "*";

    MatchExpression matchExpr(pattern, false /*dottedpath*/, true /*fullstring*/, true /*casesensitive*/);

    READER_MUTEX
    IDList out;
    int sz = idlist.size();
    for (int i=0; i<sz; ++i)
    {
        ID id = idlist.get(i);
        const ResultItem &item = getItem(id);
        MatchableResultItem matchable(item);
        if (matchExpr.matches(&matchable))
            out.uncheckedAdd(id);
    }
    return out;
}

void ResultFileManager::checkPattern(const char *pattern)
{
    if (pattern==NULL || pattern[0] == '\0') // no filter
        return;

    // parse it
    //XXX after successful parsing, we could also check that attribute names in it are valid
    MatchExpression matchExpr(pattern, false /*dottedpath*/, true /*fullstring*/, true /*casesensitive*/);
}

ResultFile *ResultFileManager::addFile(const char *fileName, const char *fileSystemFileName, bool computed)
{
    ResultFile *file = new ResultFile();
    file->id = fileList.size();
    fileList.push_back(file);
    file->resultFileManager = this;
    file->fileSystemFilePath = fileSystemFileName;
    file->filePath = fileNameToSlash(fileName);
    splitFileName(file->filePath.c_str(), file->directory, file->fileName);
    file->computed = computed;
    file->numLines = 0;
    file->numUnrecognizedLines = 0;
    return file;
}

Run *ResultFileManager::addRun()
{
    Run *run = new Run();
    run->resultFileManager = this;
    run->runNumber = 0;
    runList.push_back(run);
    return run;
}

FileRun *ResultFileManager::addFileRun(ResultFile *file, Run *run)
{
    FileRun *fileRun = new FileRun();
    fileRunList.push_back(fileRun);
    fileRun->fileRef = file;
    fileRun->runRef = run;
    return fileRun;
}

int ResultFileManager::addScalar(FileRun *fileRunRef, const char *moduleName,
                                  const char *scalarName, double value, bool isField)
{
    ScalarResult scalar;
    scalar.fileRunRef = fileRunRef;

    scalar.moduleNameRef = moduleNames.insert(moduleName);
    scalar.nameRef = names.insert(scalarName);
    scalar.value = value;
    scalar.isField = isField;

    ScalarResults &scalars = fileRunRef->fileRef->scalarResults;
    scalars.push_back(scalar);
    return scalars.size() - 1;
}

int ResultFileManager::addVector(FileRun *fileRunRef, int vectorId, const char *moduleName, const char *vectorName, const char *columns)
{
    VectorResult vector;
    vector.fileRunRef = fileRunRef;
    vector.vectorId = vectorId;
    vector.moduleNameRef = moduleNames.insert(moduleName);
    vector.nameRef = names.insert(vectorName);
    vector.columns = columns;
    vector.stat = Statistics(-1, NaN, NaN, NaN, NaN);
    VectorResults &vectors = fileRunRef->fileRef->vectorResults;
    vectors.push_back(vector);
    return vectors.size() - 1;
}

int ResultFileManager::addHistogram(FileRun *fileRunRef, const char *moduleName, const char *histogramName,
        Statistics stat, const StringMap &attrs, const HistogramFields &fields)
{
    HistogramResult histogram;
    histogram.attributes = attrs;
    histogram.fields = fields;
    histogram.fileRunRef = fileRunRef;
    histogram.moduleNameRef = moduleNames.insert(moduleName);
    histogram.nameRef = names.insert(histogramName);
    histogram.stat = stat;
    HistogramResults &histograms = fileRunRef->fileRef->histogramResults;
    histograms.push_back(histogram);
    return histograms.size() - 1;
}


// create a file for each dataset?
ID ResultFileManager::addComputedVector(int vectorId, const char *name, const char *file,
        const StringMap &attributes, ComputationID computationID, ID input, ComputationNode computation)
{
    WRITER_MUTEX

    assert(getTypeOf(input) == VECTOR);

    const VectorResult& vector = getVector(input);

    ResultFile *fileRef = getFile(file);
    if (!fileRef)
        fileRef = addFile(file, file, true); // XXX
    Run *runRef = vector.fileRunRef->runRef;
    FileRun *fileRunRef = getFileRun(fileRef, runRef);
    if (!fileRunRef)
        fileRunRef = addFileRun(fileRef, runRef);

    VectorResult newVector = VectorResult();
    newVector.vectorId = vectorId;
    newVector.computation = computation;
    newVector.columns = vector.columns;
    newVector.moduleNameRef = vector.moduleNameRef;
    newVector.nameRef = names.insert(name);
    newVector.fileRunRef = fileRunRef;
    newVector.attributes = attributes;
    newVector.stat = Statistics(-1, NaN, NaN, NaN, NaN);
    fileRef->vectorResults.push_back(newVector);
    ID id = _mkID(true, false, VECTOR, fileRef->id, fileRef->vectorResults.size()-1);
    std::pair<ComputationID, ID> key = std::make_pair(computationID, input);
    computedIDCache[key] = id;
    return id;
}

ID ResultFileManager::getComputedID(ComputationID computationID, ID input) const
{
    READER_MUTEX
    std::pair<ComputationID, ID> key = std::make_pair(computationID, input);
    ComputedIDCache::const_iterator it = computedIDCache.find(key);
    if (it != computedIDCache.end())
      return it->second;
    else
        return -1;
}

/*
void ResultFileManager::dump(ResultFile *fileRef, std::ostream& out) const
{
    Run *prevRunRef = NULL;
    for (ScalarResults::const_iterator i = scalars.begin(); i!=scalars.end(); i++)
    {
        const ScalarResult& d = *i;
        if (d.runRef->fileRef==fileRef)
        {
            if (d.runRef!=prevRunRef)
            {
                out << "run " << d.runRef->runNumber << " " << d.runRef->networkName
                    << " \"" << d.runRef->date << "\"\n";
                prevRunRef = d.runRef;
            }
            out << "scalar \"" << *d.moduleNameRef << "\"\t\""
                << *d.nameRef << "\"\t" << d.value << "\n";   //FIXME use opp_quotestr() here
        }
    }
}
*/

#ifdef CHECK
#undef CHECK
#endif
#define CHECK(cond,msg) if (!(cond)) throw ResultFileFormatException(msg, ctx.fileName, ctx.lineNo);


void ResultFileManager::processLine(char **vec, int numTokens, sParseContext &ctx)
{
    ++ctx.lineNo;

    // ignore empty lines
    if (numTokens==0 || vec[0][0]=='#')
        return;

    // process "run" lines
    if (vec[0][0]=='r' && !strcmp(vec[0],"run"))
    {
        CHECK(numTokens >= 2, "invalid result file: run Id missing from `run' line");

        if (atoi(vec[1])>0 && strlen(vec[1])<=10)
        {
            // old-style "run" line, format: run <runNumber> [<networkName> [<dateTime>]]
            // and runs in different files cannot be related, so we must create a new Run entry.
            Run *runRef = addRun();
            ctx.fileRunRef = addFileRun(ctx.fileRef, runRef);

            runRef->runNumber = atoi(vec[1]);
            runRef->attributes["run-number"] = vec[1];
            if (numTokens>=3)
                runRef->attributes["network"] = vec[2];
            if (numTokens>=4)
                runRef->attributes["dateTime"] = vec[3];

            // assemble a probably-unique runName
            std::stringstream os;
            os << ctx.fileRef->fileName << ":" << ctx.lineNo << "-#" << vec[1];
            if (numTokens>=3)
                os << "-" << vec[2];
            if (numTokens>=4)
                os << "-" << vec[3];
            runRef->runName = os.str();
        }
        else
        {
            // new-style "run" line, format: run <runName>
            // find out if we have that run already
            Run *runRef = getRunByName(vec[1]);
            if (!runRef)
            {
                // not yet: add it
                runRef = addRun();
                runRef->runName = vec[1];
            }
            // associate Run with this file
            CHECK(getFileRun(ctx.fileRef, runRef)==NULL, "invalid result file: run Id repeats in the file");
            ctx.fileRunRef = addFileRun(ctx.fileRef, runRef);
        }
        ctx.lastResultItemType = 0;
        return;
    }
    else if (vec[0][0] == 'v' && strcmp(vec[0], "version") == 0)
    {
        int version;
        CHECK(numTokens >= 2, "missing version number");
        CHECK(parseInt(vec[1], version), "version is not a number");
        CHECK(version <= 2, "expects version 2 or lower");
        return;
    }


    // if we haven't seen a "run" line yet (as with old vector files), add a default run
    if (ctx.fileRunRef==NULL)
    {
        // fake a new Run
        Run *runRef = addRun();
        ctx.fileRunRef = addFileRun(ctx.fileRef, runRef);
        runRef->runNumber = 0;

        /*
        // make up a unique runName
        std::stringstream os;
        static int counter=1000;
        os << fileRef->fileName << ":" << lineNum << "-#n/a-" <<++counter;
        runRef->runName = os.str();
        */
    }

    if (vec[0][0]=='s' && !strcmp(vec[0],"scalar"))
    {
        // syntax: "scalar <module> <scalarname> <value>"
        CHECK(numTokens>=4, "invalid scalar file: too few items on `scalar' line");

        double value;
        CHECK(parseDouble(vec[3],value), "invalid scalar file syntax: invalid value column");

        ctx.lastResultItemType = SCALAR;
        addScalar(ctx.fileRunRef, vec[1], vec[2], value, false);
    }
    else if (vec[0][0]=='v' && !strcmp(vec[0],"vector"))
    {
        // syntax: "vector <id> <module> <vectorname> [<columns>]"
        CHECK(numTokens>=4, "invalid vector file syntax: too few items on 'vector' line");
        int vectorId;
        CHECK(parseInt(vec[1], vectorId), "invalid vector file syntax: invalid vector id in vector definition");
        const char *columns = (numTokens < 5 || opp_isdigit(vec[4][0]) ? "TV" : vec[4]);

        ctx.lastResultItemType = VECTOR;
        addVector(ctx.fileRunRef, vectorId, vec[2], vec[3], columns);
    }
    else if (vec[0][0]=='s' && !strcmp(vec[0],"statistic"))
    {
        // syntax: "statistic <module> <statisticname>"
        CHECK(numTokens>=3, "invalid scalar file: too few items on `statistic' line");

        const char *moduleName = vec[1];
        const char *statisticName = vec[2];

        Statistics stat;
        StringMap attrs;
        HistogramFields fields;
        ctx.lastResultItemType = HISTOGRAM;
        addHistogram(ctx.fileRunRef, moduleName, statisticName, stat, attrs, fields);
    }
    else if (vec[0][0]=='f' && !strcmp(vec[0],"field"))
    {
        CHECK(ctx.lastResultItemType == HISTOGRAM, "invalid scalar file: missing statistic declaration")

        // syntax: "field <name> <value>"
        CHECK(numTokens>=3, "invalid scalar file: too few items on `field' line");

        std::string fieldName = vec[1];
        double value;
        CHECK(parseDouble(vec[2], value), "invalid scalar file: invalid field value");

        Assert(!ctx.fileRef->histogramResults.empty());
        HistogramResult &histogram = ctx.fileRef->histogramResults.back();
        histogram.fields[fieldName] = value;

        // set histogram.stat fields
        Statistics &stat = histogram.stat;
        if (fieldName == "count")
            stat._count = (long)value;
        else if (fieldName == "min")
            stat._min = value;
        else if (fieldName == "max")
            stat._max = value;
        else if (fieldName == "sum")
            stat._sum = value;
        else if (fieldName == "sqrsum")
            stat._sumSqr = value;

        // add field as scalar too
        std::string scalarName = (*histogram.nameRef) + ":" + fieldName;
        addScalar(ctx.fileRunRef, histogram.moduleNameRef->c_str(), scalarName.c_str(), value, true);
    }
    else if (vec[0][0]=='b' && !strcmp(vec[0],"bin"))
    {
        CHECK(ctx.lastResultItemType == HISTOGRAM, "invalid scalar file: missing statistic declaration")

        // syntax: "bin <lower_bound> <value>"
        CHECK(numTokens>=3, "");
        double lower_bound, value;
        CHECK(parseDouble(vec[1], lower_bound), "");
        CHECK(parseDouble(vec[2], value), "");

        Assert(!ctx.fileRef->histogramResults.empty());
        HistogramResult &histogram = ctx.fileRef->histogramResults.back();
        histogram.addBin(lower_bound, value);
    }
    else if (vec[0][0]=='i' && !strcmp(vec[0],"itervar")) {

        CHECK(numTokens>=3, "invalid result file: 'itervar <name> <value>' expected");

        std::string varName = vec[1];
        std::string varValue = vec[2];

        StringMap &itervars = ctx.fileRunRef->runRef->itervars;
        StringMap::iterator oldRef = itervars.find(varName);
        CHECK(oldRef == itervars.end() || oldRef->second == varValue,
           "Value of iteration variable conflicts with previously loaded value");
        itervars[varName] = varValue;
    }
    else if (vec[0][0]=='a' && !strcmp(vec[0],"attr"))
    {
        CHECK(numTokens>=3, "invalid result file: 'attr <name> <value>' expected");

        std::string attrName = vec[1];
        std::string attrValue = vec[2];

        if (ctx.lastResultItemType == 0) // run attribute
        {
            // store attribute
            StringMap &attributes = ctx.fileRunRef->runRef->attributes;
            StringMap::iterator oldPairRef = attributes.find(attrName);
            CHECK(oldPairRef == attributes.end() || oldPairRef->second == attrValue,
                  "Value of run attribute conflicts with previously loaded value");
            attributes[attrName] = attrValue;

            // the "runNumber" attribute is also stored separately
            if (attrName == "runNumber")
                CHECK(parseInt(vec[2], ctx.fileRunRef->runRef->runNumber), "invalid result file: int value expected as runNumber");
        }
        else if (ctx.lastResultItemType == SCALAR)
        {
            Assert(!ctx.fileRef->scalarResults.empty());
            ctx.fileRef->scalarResults.back().attributes[attrName] = attrValue;
        }
        else if (ctx.lastResultItemType == VECTOR)
        {
            Assert(!ctx.fileRef->vectorResults.empty());
            ctx.fileRef->vectorResults.back().attributes[attrName] = attrValue;
        }
        else if (ctx.lastResultItemType == HISTOGRAM)
        {
            Assert(!ctx.fileRef->histogramResults.empty());
            ctx.fileRef->histogramResults.back().attributes[attrName] = attrValue;
        }
    }
    else if (vec[0][0]=='p' && !strcmp(vec[0],"param"))
    {
        CHECK(numTokens>=3, "invalid result file: 'param <namePattern> <value>' expected");

        // store module param
        std::string paramName = vec[1];
        std::string paramValue = vec[2];
        StringMap &params = ctx.fileRunRef->runRef->moduleParams;
        StringMap::iterator oldPairRef = params.find(paramName);
        CHECK(oldPairRef == params.end() || oldPairRef->second == paramValue,
              "Value of module parameter conflicts with previously loaded value");
        params[paramName] = paramValue;
    }
    else if (opp_isdigit(vec[0][0]) && numTokens>=3)
    {
        // this looks like a vector data line, skip it this time

        // TODO collect statistics
    }
    else
    {
        // ignore unknown lines and vector data lines
        ctx.fileRef->numUnrecognizedLines++;
    }
}

static bool isFileReadable(const char *fileName)
{
    FILE *f = fopen(fileName, "r");
    if (f!=NULL)
    {
        fclose(f);
        return true;
    }
    else
        return false;
}

ResultFile *ResultFileManager::loadFile(const char *fileName, const char *fileSystemFileName, bool reload)
{
    WRITER_MUTEX
    // tricky part: we store "fileName" which is the Eclipse pathname of the file
    // (or some other "display name" of the file), but we actually load from
    // fileSystemFileName which is the fileName suitable for fopen()

    // check if loaded
    ResultFile *fileRef = getFile(fileName);
    if (fileRef) {
        if (reload) {
            unloadFile(fileRef);
        }
        else
            return fileRef;
    }

    // try if file can be opened, before we add it to our database
    if (fileSystemFileName==NULL)
        fileSystemFileName = fileName;
    if (!isFileReadable(fileSystemFileName))
        throw opp_runtime_error("cannot open `%s' for read", fileSystemFileName);

    // add to fileList
    fileRef = NULL;

    try
    {
        fileRef = addFile(fileName, fileSystemFileName, false);

        // if vector file and has index, load vectors from the index file
        if (IndexFile::isVectorFile(fileSystemFileName) && IndexFile::isIndexFileUpToDate(fileSystemFileName))
        {
            std::string indexFileName = IndexFile::getIndexFileName(fileSystemFileName);
            loadVectorsFromIndex(indexFileName.c_str(), fileRef);
        }
        else
        {
            // process lines in file
            FileReader freader(fileSystemFileName);
            char *line;
            LineTokenizer tokenizer;
            sParseContext ctx(fileRef);
            while ((line=freader.getNextLineBufferPointer())!=NULL)
            {
                int len = freader.getCurrentLineLength();
                int numTokens = tokenizer.tokenize(line, len);
                char **tokens = tokenizer.tokens();
                processLine(tokens, numTokens, ctx);
            }

            fileRef->numLines = ctx.lineNo; // freader.getNumReadLines();
        }
    }
    catch (std::exception&)
    {
        try
        {
            if (fileRef)
                unloadFile(fileRef);
        }
        catch (...) {}

        throw;
    }

    return fileRef;
}

void ResultFileManager::loadVectorsFromIndex(const char *filename, ResultFile *fileRef)
{
    VectorFileIndex *index = IndexFileReader(filename).readAll();
    int numOfVectors = index->getNumberOfVectors();

    if (numOfVectors == 0) {
        delete index;
        return;
    }

    Run *runRef = getRunByName(index->run.runName.c_str());
    if (!runRef)
    {
        runRef = addRun();
        runRef->runName = index->run.runName;
    }
    runRef->runNumber = index->run.runNumber;
    runRef->attributes = index->run.attributes;
    runRef->moduleParams = index->run.moduleParams;
    FileRun *fileRunRef = addFileRun(fileRef, runRef);

    for (int i = 0; i < numOfVectors; ++i)
    {
        const VectorData *vectorRef = index->getVectorAt(i);
        assert(vectorRef);

        VectorResult vectorResult;
        vectorResult.fileRunRef = fileRunRef;
        vectorResult.attributes = StringMap(vectorRef->attributes);
        vectorResult.vectorId = vectorRef->vectorId;
        vectorResult.moduleNameRef = moduleNames.insert(vectorRef->moduleName);
        vectorResult.nameRef = names.insert(vectorRef->name);
        vectorResult.columns = vectorRef->columns;
        vectorResult.startEventNum = vectorRef->startEventNum;
        vectorResult.endEventNum = vectorRef->endEventNum;
        vectorResult.startTime = vectorRef->startTime;
        vectorResult.endTime = vectorRef->endTime;
        vectorResult.stat = vectorRef->stat;
        fileRef->vectorResults.push_back(vectorResult);
    }
    delete index;
}

void ResultFileManager::unloadFile(ResultFile *file)
{
    WRITER_MUTEX

    // remove computed vector IDs
    for (ComputedIDCache::iterator it = computedIDCache.begin(); it != computedIDCache.end();)
    {
        ID id = it->first.second;
        if (_fileid(id) == file->id)
        {
            ComputedIDCache::iterator oldIt = it;
            it++;
            computedIDCache.erase(oldIt);
        }
        else
            ++it;
    }

    // remove FileRun entries
    RunList runsPotentiallyToBeDeleted;
    for (int i=0; i<(int)fileRunList.size(); i++)
    {
        if (fileRunList[i]->fileRef==file)
        {
            // remember Run; if this was the last reference, we need to delete it
            runsPotentiallyToBeDeleted.push_back(fileRunList[i]->runRef);

            // delete fileRun
            delete fileRunList[i];
            fileRunList.erase(fileRunList.begin()+i);
            i--;
        }
    }

    // delete ResultFile entry. Note that the fileList array will have a hole.
    // It is not allowed to move another ResultFile into the hole, because
    // that would change its "id", and invalidate existing IDs (IDLists)
    // that use that file.
    fileList[file->id] = NULL;
    delete file;

    // remove Runs that don't appear in other loaded files
    for (int i=0; i<(int)runsPotentiallyToBeDeleted.size(); i++)
    {
        Run *runRef = runsPotentiallyToBeDeleted[i];
        if (getFilesForRun(runRef).empty())
        {
            // delete it.
            RunList::iterator it = std::find(runList.begin(), runList.end(), runRef);
            assert(it != runList.end());  // runs may occur only once in runsPotentiallyToBeDeleted, because runNames are not allowed to repeat in files
            runList.erase(it);
        }
    }
}

/*--------------------------------------------------------------------------
 *                        compute filter hints
 *--------------------------------------------------------------------------*/

/**
 * Utility class: collects strings that occur at least twice.
 */
class DuplicateStringCollector
{
  private:
    typedef std::map<std::string,int> StringCountMap;
    StringCountMap map;
    StringVector vec;
  public:
    void add(const std::string& str) {
        StringCountMap::iterator mi = map.find(str);
        if (mi==map.end())
            map[str] = 1;  // 1st occurrence
        else if (++(mi->second)==2)  // 2nd occurrence
            vec.push_back(str);
    }
    StringVector& get() {
        return vec;
    }
};

inline bool strdictLess(const std::string &first, const std::string &second)
{
    return strdictcmp(first.c_str(), second.c_str()) < 0;
}

struct StrDictCompare
{
    bool operator()(const std::string &first, const std::string &second)
    {
        return strdictLess(first, second);
    }
};

typedef std::set<std::string, StrDictCompare> SortedStringSet;

static void replaceDigitsWithWildcard(std::string &str)
{
    std::string::iterator start;
    while ((start=std::find_if(str.begin(), str.end(), opp_isdigit))!=str.end())
    {
        std::string::iterator end = std::find_if(start, str.end(), std::not1(std::ptr_fun(opp_isdigit)));
        str.replace(start, end, 1, '*');
    }
}

StringVector *ResultFileManager::getFileAndRunNumberFilterHints(const IDList& idlist) const
{
    READER_MUTEX
    FileRunList *fileRuns = getUniqueFileRuns(idlist);

    StringVector vec;
    DuplicateStringCollector coll;

    for (int i=0; i<(int)fileRuns->size(); i++)
    {
        FileRun *fileRun = (*fileRuns)[i];
        if (fileRun->runRef->runNumber==0)
        {
            vec.push_back(fileRun->fileRef->filePath);
        }
        else
        {
            char runNumberStr[32];
            sprintf(runNumberStr, "%d", fileRun->runRef->runNumber);
            vec.push_back(fileRun->fileRef->filePath+"#"+runNumberStr);
            coll.add(fileRun->fileRef->filePath+"#*");
        }
    }
    delete fileRuns;

    // sort and concatenate them, and return the result
    StringVector *wildvec = new StringVector(coll.get());
    std::sort(vec.begin(), vec.end(), strdictLess);
    std::sort(wildvec->begin(), wildvec->end(), strdictLess);
    wildvec->insert(wildvec->end(), vec.begin(), vec.end());
    return wildvec;
}

StringVector *ResultFileManager::getFilePathFilterHints(const ResultFileList &fileList) const
{
    READER_MUTEX
    StringVector *filterHints = new StringVector;
    for (ResultFileList::const_iterator it = fileList.begin(); it != fileList.end(); ++it)
        filterHints->push_back((*it)->filePath);
    std::sort(filterHints->begin(), filterHints->end(), strdictLess);
    filterHints->insert(filterHints->begin(), "*");
    return filterHints;
}

StringVector *ResultFileManager::getRunNameFilterHints(const RunList &runList) const
{
    READER_MUTEX
    StringVector *filterHints = new StringVector;
    for (RunList::const_iterator it = runList.begin(); it != runList.end(); ++it)
        if ((*it)->runName.size() > 0)
            filterHints->push_back((*it)->runName);
    std::sort(filterHints->begin(), filterHints->end(), strdictLess);
    filterHints->insert(filterHints->begin(), "*");
    return filterHints;
}

StringVector *ResultFileManager::getModuleFilterHints(const IDList& idlist) const
{
    READER_MUTEX
    StringSet& names = *getUniqueModuleNames(idlist);

    SortedStringSet nameHints;
    DuplicateStringCollector coll;

    for (StringSet::iterator i=names.begin(); i!=names.end(); i++)
    {
        std::string a = (*i);

        // replace embedded numbers with "*"
        if (names.size() > 100)
            replaceDigitsWithWildcard(a);
        nameHints.insert(a);

        // break it up along dots, and...
        StringTokenizer tokenizer(a.c_str(), ".");
        const char *prefix = "";
        const char *suffix = ".*";
        while (tokenizer.hasMoreTokens()) {
            const char *token = tokenizer.nextToken();
            if (!tokenizer.hasMoreTokens()) suffix = "";
            coll.add(std::string(prefix)+token+suffix);
            prefix = "*.";
        }
    }
    delete &names;

    // sort and concatenate them, and return the result
    StringVector *wildvec = new StringVector(coll.get());
    wildvec->push_back(std::string("*"));
    std::sort(wildvec->begin(), wildvec->end(), strdictLess);
    wildvec->insert(wildvec->end(), nameHints.begin(), nameHints.end());
    return wildvec;
}

StringVector *ResultFileManager::getNameFilterHints(const IDList& idlist) const
{
    READER_MUTEX
    StringSet& names = *getUniqueNames(idlist);

    StringVector vec;
    DuplicateStringCollector coll;

    for (StringSet::iterator i=names.begin(); i!=names.end(); i++)
    {
        std::string a = (*i);
        vec.push_back(a);

        // break it up along spaces, and...
        StringTokenizer tokenizer(a.c_str());
        const char *prefix = "";
        const char *suffix = " *";
        while (tokenizer.hasMoreTokens()) {
            const char *token = tokenizer.nextToken();
            if (!tokenizer.hasMoreTokens()) suffix = "";
            coll.add(std::string(prefix)+token+suffix);
            prefix = "* ";
        }
    }
    delete &names;

    // sort and concatenate them, and return the result
    StringVector *wildvec = new StringVector(coll.get());
    wildvec->push_back(std::string("*"));
    std::sort(vec.begin(), vec.end(), strdictLess);
    std::sort(wildvec->begin(), wildvec->end(), strdictLess);
    wildvec->insert(wildvec->end(), vec.begin(), vec.end());
    return wildvec;
}

StringVector *ResultFileManager::getResultItemAttributeFilterHints(const IDList &idlist, const char *attrName) const
{
    READER_MUTEX
    StringSet *attrValues = getUniqueAttributeValues(idlist, attrName);
    StringVector *filterHints = new StringVector(attrValues->begin(), attrValues->end());
    std::sort(filterHints->begin(), filterHints->end(), strdictLess);
    filterHints->insert(filterHints->begin(), "*");
    delete attrValues;
    return filterHints;
}

StringVector *ResultFileManager::getRunAttributeFilterHints(const RunList &runList, const char *attrName) const
{
    READER_MUTEX
    StringSet *attrValues = getUniqueRunAttributeValues(runList, attrName);
    StringVector *filterHints = new StringVector(attrValues->begin(), attrValues->end());
    std::sort(filterHints->begin(), filterHints->end(), strdictLess);
    filterHints->insert(filterHints->begin(), "*");
    delete attrValues;
    return filterHints;
}

StringVector *ResultFileManager::getModuleParamFilterHints(const RunList &runList, const char * paramName) const
{
    READER_MUTEX
    StringSet *paramValues = getUniqueModuleParamValues(runList, paramName);
    StringVector *filterHints = new StringVector(paramValues->begin(), paramValues->end());
    std::sort(filterHints->begin(), filterHints->end(), strdictLess);
    filterHints->insert(filterHints->begin(), "*");
    delete paramValues;
    return filterHints;
}

bool ResultFileManager::hasStaleID(const IDList& ids) const
{
    READER_MUTEX
    for (int i = 0; i < ids.size(); ++i)
    {
        ID id = ids.get(i);
        if (isStaleID(id))
            return true;
    }
    return false;
}

const char *ResultFileManager::getRunAttribute(ID id, const char *attribute) const
{
    return getItem(id).fileRunRef->runRef->getAttribute(attribute);
}
