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

#include <algorithm>
#include <vector>
#include <utility>
#include "xyarray.h"

USING_NAMESPACE

using namespace std;

template <typename T> static void permute(T *&array, int size, const vector<int> &permutation)
{
    // XXX could it be performed in place?
    T* oldArray = array;
    array = new T[size];
    for (int i = 0; i < size; ++i)
        array[i] = oldArray[permutation[i]];
    delete[] oldArray;
}

void XYArray::sortByX()
{
    if (x)
    {
        vector<pair<double,int> > xCoords;
        for (int i = 0; i < len; ++i)
            xCoords.push_back(make_pair(x[i],i));

        sort(xCoords.begin(), xCoords.end());
        vector<int> permutation;
        for (int i = 0; i < len; ++i)
        {
            x[i] = xCoords[i].first;
            permutation.push_back(xCoords[i].second);
        }

        if (y)
            permute(y, len, permutation);

        if (xp)
            permute(xp, len, permutation);

        if (evec)
            permute(evec, len, permutation);
    }
}
