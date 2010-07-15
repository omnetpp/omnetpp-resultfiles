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

\name{plotLineChart}
\alias{plotLineChart}
\title{Plots a line chart.}
\description{
  Plot a line chart.
}
\usage{plotLineChart(data, properties)}
\arguments{
    \item{data}{a list of two column matrices, the rows of the matrices are ordered by their first column}
    \item{properties}{the graphical properties of the chart (like titles, line types, colors, \ldots)}
}

\details{
  Graphical properties:
  \describe{
    \item{Graph.Title}{title of the graph}
    \item{X.Axis.Title}{title of the X axis}
    \item{Y.Axis.Title}{title of the Y axis}
    \item{Y.Axis.Log}{logical, indicating that the Y axis is logarithmic}
    \item{Legend.Display}{logical, indicating that the legend is to be drawn}
    \item{Legend.Border}{logical, indicating that the legend border is to be drawn}
    \item{Legend.Position}{position of the legend (one of \code{'Inside'}, \code{'Above'}, \code{'Below'}, \code{'Left'} or \code{'Right'})}
    \item{Line.Display/\var{line_name}}{logical, indicating if the named line is to be displayed}
    \item{Line.Type/\var{line_name}}{type of the named line: \code{'Linear'}, \code{'Pins'},
                                     \code{'Dots'}, \code{'Points'}, \code{'SampleHold'} or \code{'BackwardSampleHold'}}
    \item{Line.Color/\var{line_name}}{color of the named line}
    \item{Symbols.Type/\var{line_name}}{type of symbols on the named line: \code{'None'}, \code{'Cross'}, \code{'Diamond'},
                                        \code{'Dot'}, \code{'Plus'}, \code{'Square'} or \code{'Triangle'}}
    \item{Symbols.Size/\var{line_name}}{size of symbols on the named line}
  }
}

\examples{
d <- loadDataset(system.file('extdata/PureAloha1-0.vec', package='omnetpp'), add(type='vector'))
vectors <- loadVectors(d, NULL)
plotLineChart(split(vectors$vectordata, vectors$vectordata$resultkey), list())
}

\keyword{hplot}
