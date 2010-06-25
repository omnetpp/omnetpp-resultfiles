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

#ifndef _IDLIST_H_
#define _IDLIST_H_

#include <vector>
#include <exception>
#include <stdexcept>
#include "scavedefs.h" // int64

NAMESPACE_BEGIN

class ResultFileManager;

/**
 * Result ID -- identifies a scalar or a vector in a ResultFileManager
 */
typedef int64 ID;


/**
 * Stores a set of unique IDs. Order is not important, and may occasionally
 * change (after merge(), substract() or intersect()).
 *
 * Beware: Copy ctor implements transfer-of-ownership semantics!
 */
class SCAVE_API IDList
{
    private:
        friend class ResultFileManager;
        typedef std::vector<ID> V;
        V *v;

        void operator=(const IDList&); // undefined, to prevent calling it
        void checkV() const {if (!v) throw std::runtime_error("this is a zombie IDList");}
        void uncheckedAdd(ID id) {v->push_back(id);} // doesn't check if already in there
        void discardDuplicates();
        void checkIntegrity(ResultFileManager *mgr) const;
        void checkIntegrityAllScalars(ResultFileManager *mgr) const;
        void checkIntegrityAllVectors(ResultFileManager *mgr) const;
        void checkIntegrityAllHistograms(ResultFileManager *mgr) const;

        template <class T> void sortBy(ResultFileManager *mgr, bool ascending, T& comparator);
        template <class T> void sortScalarsBy(ResultFileManager *mgr, bool ascending, T& comparator);
        template <class T> void sortVectorsBy(ResultFileManager *mgr, bool ascending, T& comparator);

    public:
        IDList()  {v = new V;}
        IDList(unsigned int sz)  {v = new V(sz);}
        IDList(const IDList& ids); // transfer of ownership semantics!
        ~IDList()  {delete v;}
        int size() const  {checkV(); return (int)v->size();}
        bool isEmpty() const  {checkV(); return v->empty();}
        void clear()  {checkV(); v->clear();}
        void set(const IDList& ids);
        void add(ID x);
        ID get(int i) const {checkV(); return v->at(i);} // at() includes bounds check
        // void set(int i, ID x);
        void erase(int i);
        void substract(ID x); // this -= {x}
        int indexOf(ID x) const;
        void merge(IDList& ids);  // this += ids
        void substract(IDList& ids);  // this -= ids
        void intersect(IDList& ids);  // this = intersection(this,ids)
        IDList getSubsetByIndices(int *array, int n) const;
        IDList dup() const;
        int getItemTypes() const;  // SCALAR, VECTOR or their binary OR
        bool areAllScalars() const;
        bool areAllVectors() const;
        bool areAllHistograms() const;
        // sorting
        void sortByFileAndRun(ResultFileManager *mgr, bool ascending);
        void sortByRunAndFile(ResultFileManager *mgr, bool ascending);
        void sortByDirectory(ResultFileManager *mgr, bool ascending);
        void sortByFileName(ResultFileManager *mgr, bool ascending);
        void sortByRun(ResultFileManager *mgr, bool ascending);
        void sortByModule(ResultFileManager *mgr, bool ascending);
        void sortByName(ResultFileManager *mgr, bool ascending);
        void sortScalarsByValue(ResultFileManager *mgr, bool ascending);
        void sortVectorsByVectorId(ResultFileManager *mgr, bool ascending);
        void sortVectorsByLength(ResultFileManager *mgr, bool ascending);
        void sortVectorsByMean(ResultFileManager *mgr, bool ascending);
        void sortVectorsByStdDev(ResultFileManager *mgr, bool ascending);
        void sortVectorsByMin(ResultFileManager *mgr, bool ascending);
        void sortVectorsByMax(ResultFileManager *mgr, bool ascending);
        void sortVectorsByStartTime(ResultFileManager *mgr, bool ascending);
        void sortVectorsByEndTime(ResultFileManager *mgr, bool ascending);
        void sortByRunAttribute(ResultFileManager *mgr, const char* runAttr, bool ascending);
        void reverse();
        void toByteArray(char *array, int n) const;
        void fromByteArray(char *array, int n);
};

NAMESPACE_END


#endif


