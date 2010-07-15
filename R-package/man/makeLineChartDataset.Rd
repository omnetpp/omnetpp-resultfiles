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

\name{makeLineChartDataset}
\alias{makeLineChartDataset}
\title{Create a line chart dataset from vectors.}
\description{
  Splits the vectordata of a dataset into separate vectors and names them.
  The result can be displayed with \code{plotLineChart()}.
}

\usage{makeLineChartDataset(dataset, nameFormat=NULL)}
\arguments{
    \item{dataset}{Object of class "dataset", holding the vectors and runs.}
    \item{nameFormat}{The format of the generated line names
          (e.g. '\{config\}/\{runnumber\} - \{module\} \{name\}').}
}

\value{
  A list of data frames, the rows are named according the the specified format.
  The data frames has the x and y coordinates of the corresponding vector.
}

\examples{
d <- loadDataset('Aloha.vec', add(type='vector'))
v <- loadVectors(d, NULL)

makeLineChartDataset(v, '{module} {name}')
}

\keyword{manip}
