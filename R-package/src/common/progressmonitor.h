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

#ifndef _PROGRESSMONITOR_H_
#define _PROGRESSMONITOR_H_

#include <string>
#include "platdefs.h"

NAMESPACE_BEGIN

/**
 * Interface for reporting progress. Compatible with Eclipse's IProgressMonitor class.
 */
class IProgressMonitor
{
    public:

    virtual ~IProgressMonitor() {}

    /* Constant indicating an unknown amount of work. */
    static const int UNKNOWN = -1;

    /**
     * Notifies that the main task is beginning.  This must only be called once
     * on a given progress monitor instance.
     *
     * @param name the name (or description) of the main task
     * @param totalWork the total number of work units into which
     *  the main task is been subdivided. If the value is <code>UNKNOWN</code>
     *  the implementation is free to indicate progress in a way which
     *  doesn't require the total number of work units in advance.
     */
    virtual void beginTask(std::string name, int totalWork) = 0;

    /**
     * Notifies that the work is done; that is, either the main task is completed
     * or the user canceled it. This method may be called more than once
     * (implementations should be prepared to handle this case).
     */
    virtual void done() = 0;

    /**
     * Returns whether cancelation of current operation has been requested.
     * Long-running operations should poll to see if cancelation
     * has been requested.
     *
     * @return <code>true</code> if cancellation has been requested,
     *    and <code>false</code> otherwise
     * @see #setCanceled(boolean)
     */
    virtual bool isCanceled() = 0;

    /**
     * Sets the cancel state to the given value.
     *
     * @param value <code>true</code> indicates that cancelation has
     *     been requested (but not necessarily acknowledged);
     *     <code>false</code> clears this flag
     * @see #isCanceled()
     */
    virtual void setCanceled(bool value) = 0;

    /**
     * Notifies that a subtask of the main task is beginning.
     * Subtasks are optional; the main task might not have subtasks.
     *
     * @param name the name (or description) of the subtask
     */
    virtual void subTask(std::string name) = 0;

    /**
     * Notifies that a given number of work unit of the main task
     * has been completed. Note that this amount represents an
     * installment, as opposed to a cumulative amount of work done
     * to date.
     *
     * @param work a non-negative number of work units just completed
     */
    virtual void worked(int work) = 0;
};

NAMESPACE_END


#endif

