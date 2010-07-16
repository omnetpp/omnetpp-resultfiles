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

\name{plotHistogramChart}
\alias{plotHistogramChart}
\title{Plot a histogram chart.}
\description{
  Plot one or more histograms.
}
\usage{plotHistogramChart(data, \dots)}
\arguments{
    \item{data}{a list of histogram objects}
    \item{\dots}{a list of graphical properties of the chart (like titles, colors, \ldots)}
}

\details{
  The histograms chart supports
  \itemize{
    \item Drawing more histograms on one chart.
    \item Display data counts, probability density, or cumulative density of the distribution.
    \item Drawing filled/outline bars.
    \item A common baseline of the bars can be specified.
  }

  These options and other graphical properties can be given in the \code{properties} argument:
  \describe{
  \item{Graph.Title}{title of the chart}
  \item{X.Axis.Title}{title of the X axis}
  \item{Y.Axis.Title}{title of the Y axis}
  \item{Y.Axis.Log}{logical, indicating that the Y axis is logarithmic}
  \item{Legend.Display}{logical, indicating that the legend is to be drawn}
  \item{Legend.Border}{logical, indicating that the legend border is to be drawn}
  \item{Legend.Position}{position of the legend (one of 'Inside', 'Above', 'Below', 'Left' or 'Right')}
  \item{Hist.Bar}{'Solid' or 'Outline'}
  \item{Hist.Data}{'Count', 'Pdf' or 'Cdf'}
  \item{Hist.ShowOverflowCell}{logical, indicating that over/underflow cells are displayed}
  \item{Hist.Baseline}{numeric value, common base line of the histograms}
  \item{Hist.Color/\var{hist_name}}{color of the named histogram}
  }
}

\examples{
data <- list(norm=hist(rnorm(1000), plot=FALSE), unif=hist(runif(1000), plot=FALSE))
plotHistogramChart(data, Graph.Title='Test', 'Hist.Color/norm'='#000080', 'Hist.Color/unif'='#008000')
}
\keyword{hplot}
