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

#ifndef _SCAVEEXCEPTION_H_
#define _SCAVEEXCEPTION_H_

#include <string>
#include <exception>
#include <stdexcept>
#include "platmisc.h"
#include "commondefs.h"
#include "scavedefs.h"
#include "exception.h"

NAMESPACE_BEGIN

/**
 * Exception thrown when a syntax error found while reading a result file.
 */
class SCAVE_API ResultFileFormatException : public opp_runtime_error
{
  private:
      std::string file;
      int64 line;
      file_offset_t offset;

  public:
    /**
     * Constructor with error message and location.
     */
    ResultFileFormatException(const char *msg, const char *file, int64 line, file_offset_t offset = -1);

    /**
     * Destructor with throw clause required by gcc.
     */
    virtual ~ResultFileFormatException() throw() {}

    /**
     * Returns the name of the file in which the error occured.
     */
    const char *getFileName() const { return file.c_str(); }

    /**
     * Returns the line number at which the error occured.
     */
    int64 getLine() const { return line; }

    /**
     * Returns the file offset at which the error occured.
     */
    file_offset_t getFileOffset() const { return offset; }
};

NAMESPACE_END


#endif


