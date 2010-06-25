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

\name{loadDataset}
\alias{loadDataset}
\title{Loads data from result files}
\description{
  Loads data from result files.
}

\usage{loadDataset(files, \dots)}
\arguments{
	\item{files}{Character vector containing the names of the files to be loaded. Wildcards are allowed in file names.}
    \item{\dots}{The add/discard operations selecting the data to load.}
}

\details{
  The \dots argument is a sequence of add/discard forms:
  \tabular{ll}{
    \code{add(type,select)} \tab Adds the selected result items to the dataset. \cr
    \code{discard(type,select)}   \tab Removes the selected result items from the dataset. \cr
  }

  where
  \describe{
  	\item{type:}{the type of the result item, one of 'scalar', 'vector' or 'histogram'}
  	\item{select:}{a character value containing a pattern to match. See help for \link{patterns} for the description of the pattern language.}
  }
  
  If no add/remove operations are given, all data is loaded from the specified files.
}

\value{
  an object of class '"omnetpp_dataset"' which is a list with 9 components:
  \item{runattrs}{dataframe of run attributes with (runid, name, value) columns}
  \item{fileruns}{dataframe of run/file pairs (runid, file) columns}
  \item{scalars}{dataframe of scalars with (scalar_key, runid, file, module, name, value) columns}
  \item{vectors}{dataframe of vectors with (vector_key, runid, file, vectorid, module, name) columns}
  \item{histograms}{dataframe of histograms with (histogram_key, runid, file, module, name) columns}
  \item{fields}{dataframe of histogram fields with (histogram_key, name, value) columns (names are 'count', 'min', 'max', 'mean', 'variance', 'stddev', \ldots)}
  \item{bins}{bounds and counts of bins of histograms, a dataframe with (histogram_key, lowerbound, upperbound, count) columns}
  \item{params}{dataframe of module parameters in each run as a table with (runid, name, value) columns}
  \item{attrs}{dataframe of attributes of scalars, vectors and histograms as a data.frame with (type, key, name, value) columns}
  
  The key columns represent the links between the objects.
}

\seealso{\link{patterns}, \link{filters}}

\examples{
# load scalars from Aloha-1.sca and Aloha-2.sca written by module 'Aloha.server'
# and vectors from 'PureAloha1-*.vec' files except 'channel utilization'
loadDataset(c('Aloha-1.sca', 'Aloha-2.sca', 'PureAloha1-*.vec'),
  add(type='scalar', select='module(Aloha.server)'),
  add('vector'),
  discard('vector', 'name("channel utilization")')
)

}

\keyword{file}
