%
% Copyright (c) 2010 Opensim Ltd.
% All rights reserved.
%
% Redistribution and use in source and binary forms, with or without
% modification, are permitted provided that the following conditions are met:
%     * Redistributions of source code must retain the above copyright
%       notice, this list of conditions and the following disclaimer.
%     * Redistributions in binary form must reproduce the above copyright
%       notice, this list of conditions and the following disclaimer in the
%       documentation and/or other materials provided with the distribution.
%     * Neither the name of the Opensim Ltd. nor the
%       names of its contributors may be used to endorse or promote products
%       derived from this software without specific prior written permission.
%
% THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
% ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
% WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
% DISCLAIMED. IN NO EVENT SHALL Andras Varga or Opensim Ltd. BE LIABLE FOR ANY
% DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
% (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
% LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
% ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
% (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
% SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
%

\name{filters}
\alias{filters}
\title{Filters}
\description{
  Description of filters can be used in a \code{\link{loadVectors}}.
}
\details{
  \tabular{ll}{
  add(c) \tab Adds a constant to the input. \cr
  compare(eq,gt,lt,x) \tab Compares value against 'x', and optionally replaces it with a constant. \cr
  crop(t1,t2) \tab Discards values outside the [t1,t2] interval. \cr
  difference() \tab Subtract the previous value from every value. \cr
  diffquot \tab Calculates the difference quotient of every value and subsequent one. \cr
  divideBy(a) \tab Diveides input by a constant. \cr
  divtime() \tab Divides input by the current time. \cr
  expression(expression) \tab Evaluates an arbitrary expression. Use t for time, y for value, and tprev, yprev
                              for previous values. \cr
  integrate(interpolationMode) \tab Integrates the input as a step function (sample-hold or backward-sample-hold)
                                    or with linear interpolation. \cr
  lineartrend(a) \tab Adds a linear component to input series. \cr
  mean() \tab Calculates mean on (0,t) \cr
  modulo(a) \tab Computes input modulo a constant. \cr
  movingavg(alpha) \tab Applies the exponentially weighted moving average filter. \cr
  multiplyBy(a) \tab Multiplies input by a constant. \cr
  removerepeats() \tab Removes repeated y values. \cr
  slidingwinavg(windowSize) \tab Replaces every value with the mean of values in the window. \cr
  substractFirstValue() \tab Substract the first value from every subsequent values. \cr
  sum() \tab Sums up values. \cr
  timeavg(interpolationMode) \tab Calculates the time average of the input (integral devided by time) \cr
  timediff() \tab Returns the difference in time between this and the previous value. \cr
  timeshift(dt) \tab Shifts the input series in time by a contstant. \cr
  timetoserial() \tab Replaces time values with their index. \cr
  timewinavg(windowSize) \tab Calculates time average: replaces input values in every 'windowSize' interval with their mean. \cr
  winavg(windowSize) \tab Calculates batched average: replaces every 'windowSize' input values with their mean. \cr
  }
}

\seealso{
  \code{\link{loadVectors}}
}

\examples{
  # add 1 to each y value
  #add(1)

  # same with the expression filter
  #expression('y+1')

  # compute the difference quotient with the expression filter
  #expression('(y-yprev)/(t-tprev)')

  # calculate window averages (windowSize=100)
  #winavg(100)
}
