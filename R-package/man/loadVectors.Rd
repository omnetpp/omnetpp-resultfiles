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

\name{loadVectors}
\alias{loadVectors}
\title{Loads vector data from result files and applies some processing}
\description{
  Loads data from result files and applies some processing.
  This function can process huge (GB) result files without loading the whole file into memory.
}

\usage{loadVectors(vectors, \dots)}
\arguments{
	\item{vectors}{a dataframe with (ospath,vectorid) columns.}
    \item{\dots}{The operations describing further processing of the vectors. The arguments are not evaluated.}
}

\details{
  The first argument identifies the vectors loaded from the result files.
  The other arguments describe the processing steps to by applied to the loaded vectors.
  Each step must be one of the following:
  \tabular{ll}{
    \code{apply(function, select)}   \tab Replaces the selected result items with the result of the function applied to them. \cr
    \code{compute(function, select)} \tab Adds new result items computed by applying the function to the selected result items. \cr
  }

  where
  \describe{
  	\item{select:}{a character value containing a pattern to match. See help for \link{patterns} for the description of the pattern language.}
  	\item{function:}{the function to be applied. List of function can be used (see help(\link{filters}) for complete description):
      \tabular{l}{
        \code{add(c)} \cr
        \code{compare(ifEqual,ifGreater,ifLess,threshold)} \cr
        \code{crop(t1,t2)} \cr
        \code{difference()} \cr
        \code{diffquot()} \cr
        \code{divideBy(a)} \cr
        \code{divtime()} \cr
        \code{expression(expression)} \cr
        \code{integrate(interpolation-mode)} \cr
        \code{lineartrend(a)} \cr
        \code{mean()} \cr
        \code{modulo(a)} \cr
        \code{movingavg(alpha)} \cr
        \code{multiplyBy(a)} \cr
        \code{removerepeats()} \cr
        \code{slidingwinavg(windowSize)} \cr
        \code{sum()} \cr
        \code{timeavg(interpolation-mode)} \cr
        \code{timediff()} \cr
        \code{timeshift(dt)} \cr
        \code{timetoserial()} \cr
        \code{timewinavg(windowSize)} \cr
        \code{winavg(windowSize)} \cr
      }
  	}
  }
}

\value{
  a list with 3 components:
  \item{vectors}{dataframe of vectors with (vector_key, ospath, vectorid, module, name) columns, ospath is '' for computed vectors}
  \item{vectordata}{dataframe of vector data with (vector_key, eventno, x, y) columns}
  \item{attributes}{dataframe of attributes of computed vectors with (vector_key, name, value) columns}

  The key columns represent the links between the objects.
}

\seealso{\link{patterns}, \link{filters}}

\examples{
d <- loadDataset(c('Aloha.sca', 'PureAloha1-*.vec'),
       add(type='scalar', select='module(Aloha.server)'),
       add('vector'))

vectorids = merge(d$files, d$vectors)[,c('ospath','vectorid')]

loadVectors(vectorids, compute(winavg(windowSize=10), select='name("channel utilization")'))
}
\keyword{file}
