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

\name{createBarChartDataset}
\alias{createBarChartDataset}
\title{Create a bar chart dataset from scalars}
\description{
  Create a bar chart dataset from scalars by grouping them according their attributes.
}
\usage{createBarChartDataset(dataset, rows, columns, aggregate='mean')}
\arguments{
    \item{dataset}{an object of class "dataset", holding the scalars and runs. (See \code{\link{loadDataset}})}
    \item{rows}{a charater vector holding the field names which identifies the rows of the result.}
    \item{columns}{a character vector that determines the columns of the result.}
    \item{aggregate}{a function used to aggregate the scalars with the same column and row fields.}
}

\details{
  The scalars are grouped into cells of a table. For a scalar
  the row of the cell is determined by the values of the \code{rows} fields,
  the column of the cell is determined by the values of the \code{columns} fields.
  Scalars in the same cell are aggregated using the supplied function.
}

\value{
  A numeric matrix containing the aggregated scalar values.
  The values of the row/column fields can be accessed from the 'rowid', 'columnid' attributes (character matrices).
  The rows and columns of the matrix are named by concatenating the values of the row/column fields (separated by a '.').
}

\seealso{
  \code{\link{loadDataset}}
}

\examples{
d <- loadDataset(add(type='scalar', file=system.file('extdata/PureAloha1-0.sca', package='omnetpp')))
createBarChartDataset(d, rows=c('module','experiment','measurement'), columns='name')
}

\keyword{manip}
