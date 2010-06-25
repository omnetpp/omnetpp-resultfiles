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

#ifndef __IOUTPUTVECTOR_H
#define __IOUTPUTVECTOR_H

/**
 * An output vector. Output vectors should not be instantiated directly;
 * instances should be obtained from an IOutputVectorManager.
 * Methods throw ResultRecordingException on errors.
 *
 * @author Andras
 */
class IOutputVector
{
  protected:
    /**
     * Flush this output vector, i.e. force any potentially buffered data to
     * be written out.
     */
    virtual void flush() = 0;

    /**
     * Close this output vector. This may free resources in the output
     * vector manager, but otherwise it is optional. (The output vector
     * manager's close method closes all open vectors as well.)
     */
    virtual void close() = 0;

  public:
    /**
     * Virtual destructor.
     */
    virtual ~IOutputVector() {}

    /**
     * Record a value with the current simulation time as timestamp into the
     * output vector. The return value is true if the data was actually
     * recorded, and false if it was not recorded (because of filtering, etc.)
     * The current simulation time is obtained from the ISimulationTimeProvider
     * instance given to IOutputVectorManager.
     */
     virtual bool record(double value) = 0;

    /**
     * Record a value with the given time stamp into the output vector. The
     * return value is true if the data was actually recorded, and false
     * if it was not recorded (because of filtering, etc.)
     *
     * Values MUST be recorded increasing timestamp order.
     */
    virtual bool record(double time, double value) = 0;
};

#endif
