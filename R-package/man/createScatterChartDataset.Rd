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

\name{createScatterChartDataset}
\alias{createScatterChartDataset}
\title{Create an XY data from scalars.}
\description{
  Arranges scalars into an XY grid and optionally averages scalars from different
  replications of the same measurement.
}
\usage{createScatterChartDataset(dataset, xModule, xName, isoModule, isoName,
                                 averageReplications=FALSE)}
\arguments{
    \item{dataset}{Object of class "dataset", holding the scalars and runs.}
    \item{xModule}{module name of the scalar on the X axis}
    \item{xName}{name of the scalar on the X axis}
    \item{isoModule}{module name of the scalars of the iso lines}
    \item{isoName}{name of the scalars of the iso lines}
    \item{averageReplications}{if TRUE then scalars from runs of the same measurement are averaged.}
}

\value{
  A list of dataframes (one dataframe for each iso value). Each dataframe contains
  the X values in the first column and has an additional column for each non-iso scalars.
}

\examples{
d <- loadDataset(add(type='scalar', file='Aloha.sca'))
createScatterChartDataset(d, xModule='.', xName='iaTime', isoModule='.',
                          isoName='numHosts', averageReplications=TRUE)
}

\keyword{manip}
