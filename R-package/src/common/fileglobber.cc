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

#include <string.h>
#include "platmisc.h"
#include "fileglobber.h"

#ifdef _WIN32

#include <io.h>
#include <direct.h>
#include <stdlib.h> // _MAX_PATH

USING_NAMESPACE


struct GlobPrivateData
{
    bool nowildcard;
    long handle;
    struct _finddata_t fdata;
    char dir[_MAX_PATH];
    char tmpfname[_MAX_PATH];
};

FileGlobber::FileGlobber(const char *filenamepattern)
{
    fnamepattern = filenamepattern;
    data = NULL;
}

FileGlobber::~FileGlobber()
{
    delete data;
}

const char *FileGlobber::getNext()
{
    if (!data)
    {
        // first call
        data = new GlobPrivateData();
        if (!strchr(fnamepattern.c_str(),'*') && !strchr(fnamepattern.c_str(),'?'))
        {
            data->nowildcard = true;
            return fnamepattern.c_str();
        }

        data->nowildcard = false;

        // remember directory in data->dir
        strcpy(data->dir,fnamepattern.c_str());
        char *s = data->dir + strlen(data->dir);
        while (--s>=data->dir)
            if (*s=='/' || *s=='\\')
                break;
        *(s+1)='\0';  // points (s+1) points either after last "/" or at beg of string.

        if (strchr(data->dir,'*') || strchr(data->dir,'?'))
            throw std::runtime_error(std::string(data->dir)+": wildcard characters in directory names are not allowed");

        // get first file
        data->handle = _findfirst(fnamepattern.c_str(), &data->fdata);
        if (data->handle<0)
        {
            _findclose(data->handle);
            return NULL;
        }

        // concat file name on directory
        strcpy(data->tmpfname,data->dir);
        strcat(data->tmpfname,data->fdata.name);
        return data->tmpfname;
    }
    else
    {
        // subsequent calls
        if (data->nowildcard)
            return NULL;
        int done=_findnext(data->handle, &data->fdata);
        if (done)
        {
            _findclose(data->handle);
            return NULL;
        }
        // concat file name on directory
        strcpy(data->tmpfname,data->dir);
        strcat(data->tmpfname,data->fdata.name);
        return data->tmpfname;
    }
}

#else

#include <glob.h>

struct GlobPrivateData
{
    bool nowildcard;
    glob_t globdata;
    int globpos;
};

FileGlobber::FileGlobber(const char *filenamepattern)
{
    fnamepattern = filenamepattern;
    data = NULL;
}

FileGlobber::~FileGlobber()
{
    globfree(&data->globdata);
    delete data;
}

const char *FileGlobber::getNext()
{
    if (!data)
    {
        data = new GlobPrivateData();
        if (!strchr(fnamepattern.c_str(),'*') && !strchr(fnamepattern.c_str(),'?'))
        {
            data->nowildcard = true;
            return fnamepattern.c_str();
        }

        data->nowildcard = false;
        if (glob(fnamepattern.c_str(), 0, NULL, &data->globdata)!=0)
            return NULL;
        data->globpos = 0;
        return data->globdata.gl_pathv[data->globpos++];
    }
    else
    {
        if (data->nowildcard)
            return NULL;
        return data->globdata.gl_pathv[data->globpos++];
    }
}
#endif


std::vector<std::string> FileGlobber::getFilenames()
{
    std::vector<std::string> v;
    const char *filename;
    while ((filename=getNext())!=NULL)
        v.push_back(filename);
    return v;
}


