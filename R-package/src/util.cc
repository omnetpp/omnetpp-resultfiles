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

#include "util.h"

SEXP getElementByName(SEXP list, const char *name)
{
    SEXP elmt = R_NilValue, names = getAttrib(list, R_NamesSymbol);
    int i;

    for (i = 0; i < length(list); i++)
    {
        if(strcmp(CHAR(STRING_ELT(names, i)), name) == 0)
        {
            elmt = VECTOR_ELT(list, i);
            break;
        }
    }
    return elmt;
}


void setNames(SEXP vector, const char** names, int len)
{
    SEXP namesSEXP;
    PROTECT(namesSEXP = NEW_STRING(len));
    for (int i = 0; i < len; ++i)
        SET_STRING_ELT(namesSEXP, i, mkChar(names[i]));
    SET_NAMES(vector, namesSEXP);
    UNPROTECT(1);
}

SEXP createDataFrame(const char** columnNames, const SEXPTYPE* columnTypes, int columnCount, int rowCount)
{
    SEXP result, item;
    PROTECT(result = NEW_LIST(columnCount));
    for (int i = 0; i < columnCount; ++i)
    {
        PROTECT(item = allocVector(columnTypes[i], rowCount));
        SET_ELEMENT(result, i, item);
    }
    UNPROTECT(columnCount);
    setNames(result, columnNames, columnCount);
    return result;
}

