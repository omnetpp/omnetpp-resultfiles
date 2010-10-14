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

\name{makeHistograms}
\alias{makeHistograms}
\title{Build histogram objects from histogram bins.}
\description{
  Build histogram objects from histogram bins.
}
\usage{makeHistograms(dataset, nameFormat=NULL)}
\arguments{
    \item{dataset}{an object of class "dataset" with 'statistics', 'fields' and 'bins' components.}
    \item{nameFormat}{an optional format string for the histogram names, e.g. "$\{module\} $\{name\}".}
}

\details{
  Builds histogram objects from a dataset loaded by \code{\link{loadDataset}}.
}

\value{
  A list of histogram objects.
  A histogram is an object is a list of class "histogram" with the following components (see \code{\link{hist}}):

  \item{breaks}{cell boundaries}
  \item{counts}{cell counts, may be fractional}
  \item{density}{estimated density values}
  \item{intensities}{same as 'density', for compatibility with \code{\link{hist}}}
  \item{mids}{the cell midpoints}
  \item{xname}{a character string with the histogram name}
  \item{equidist}{logical, indicating if the distances between 'breaks' are all the same.}
  \item{summary}{a numeric vector with components named 'mean', 'min', 'max', 'variance', 'stddev'}
  \item{overflowCount}{number of samples above the last break, or NULL}
  \item{underflowCount}{number of samples below the first break, or NULL}
}

\seealso{\code{\link{loadDataset}}, \code{\link{hist}}}

\examples{
data(aloha)
hists <- makeHistograms(aloha)
}

\keyword{manip}
