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

#ifndef __IRESULTATTRIBUTES_H
#define __IRESULTATTRIBUTES_H

#include <string>

/**
 * Provides constants for IOutputScalarManager and IOutputVectorManager.
 *
 * @author Andras
 */

//FIXME these should be "extern", and in a namespace or class!

// --- run attributes ---

/**
 * Run attribute: the name of the simulation model being run.
 */
static const std::string ATTR_NETWORK = "network";

/**
 * Run attribute: the sequence number of the simulation run among the total
 * number of runs in a parameter study. For example, if the simulator is run
 * with 60 inputs, 10 trials (repetitions) each, the run number is between
 * 0 and 599.
 */
static const std::string ATTR_RUNNUMBER = "runnumber";

/**
 * Run attribute: name of the experiment (=parameter study).
 */
static const std::string ATTR_EXPERIMENT = "experiment";

/**
 * Run attribute: identifies the measurement within the experiment (=parameter study).
 * A recommended measurement label is the concatenation of the inputs, e.g.
 * <code>"nhosts=10,mean=0.2s"</code>. That is, the value of ATTR_ITERATIONVARS
 * may be used as measurement label, if present.
 */
static const std::string ATTR_MEASUREMENT = "measurement";

/**
 * Run attribute: label of the given trial within the measurement. The recommended
 * value is "#" plus the value of ATTR_REPETITION, if present.
 */
static const std::string ATTR_REPLICATION = "replication";

/**
 * Concatenation of the inputs being varied during the experiment (=parameter study),
 * e.g. <code>"nhosts=10,mean=0.2s"</code>.
 */
static const std::string ATTR_ITERATIONVARS = "iterationvars";

/**
 * The value of ATTR_ITERATIONVARS plus the replication label (ATTR_REPLICATION).
 */
static const std::string ATTR_ITERATIONVARS2 = "iterationvars2";

/**
 * Run attribute: the date and time of running the simulation. The preferred
 * format is <code>"yyyyMMdd-hh:mm:ss"</code>.
 */
static const std::string ATTR_DATETIME = "datetime";

/**
 * Run attribute: the process ID of running the simulation.
 */
static const std::string ATTR_PROCESSID = "processid";

/**
 * Run attribute: serial number of the given trial within the measurement.
 * For example, if the simulation is run 10 times with every input
 * (i.e. with 10 different seed sets), then repetition is a number in the
 * 0..9 interval.
 */
static const std::string ATTR_REPETITION = "repetition";

/**
 * Identifies the RNG seed or set of RNG seeds used for the simulation.
 */
static const std::string ATTR_SEEDSET = "seedset";

/**
 * Run attribute; name of the (main) configuration file of the simulation
 * if such thing exists in the given simulator framework; otherwise this
 * attribute should be omitted.
 */
static const std::string ATTR_INIFILE = "inifile";

/**
 * Run attribute; name of the active configuration in the config file
 * of the simulation if such thing exists in the given simulator
 * framework; otherwise this attribute should be omitted.
 */
static const std::string ATTR_CONFIGNAME = "configname";

// --- vector/scalar attributes ---

/**
 * Vector/scalar attribute: unit of measurement in which the numeric value(s)
 * of the scalar or vector are understood.
 */
static const std::string ATTR_UNIT = "unit";

/**
 * Vector/scalar attribute: data type of the scalar or vector. Potential values
 * are provided as TYPE_xxx constants.
 */
static const std::string ATTR_TYPE = "type";

/**
 * Vector/scalar attribute: if the data type of the scalar or vector is enum,
 * what is the name of the enum.
 */
static const std::string ATTR_ENUMNAME = "enumname";

/**
 * Vector/scalar attribute: defines symbolic names for numeric values.
 * Syntax for the attribute value: <code>"NAME1=value1,NAME2=value2,NAME3=value3"</code>
 */
static const std::string ATTR_ENUM = "enum";

/**
 * Vector attribute: defines how values in the vector should be plotted:
 * with linear interpolation, sample-hold, backward sample-hold, or no
 * interpolation (i.e. dots should not be connected). Values are provided
 * as IM_xxx constants.
 */
static const std::string ATTR_INTERPOLATIONMODE = "interpolationmode";

/**
 * Vector attribute: declares the theoretical lower bound of the values
 * in the vector. This value may be used as a hint during result analysis.
 */
static const std::string ATTR_MIN = "min";

/**
 * Vector attribute: declares the theoretical upper bound of the values
 * in the vector. This value may be used as a hint during result analysis.
 */
static const std::string ATTR_MAX = "max";

// values for ATTR_TYPE
static const std::string TYPE_INT = "int";
static const std::string TYPE_DOUBLE = "double";
static const std::string TYPE_ENUM = "enum";

// values for ATTR_INTERPOLATIONMODE
static const std::string IM_NONE = "none";
static const std::string IM_LINEAR = "linear";
static const std::string IM_SAMPLE_HOLD = "sample-hold";
static const std::string IM_BACKWARD_SAMPLE_HOLD = "backward-sample-hold";

#endif

