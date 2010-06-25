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

\name{patterns}
\alias{patterns}
\title{Pattern language}
\description{
  Description of the pattern language used to select result items in datasets.
}
\details{
  A filter expression is composed of atomic patterns with the AND, OR, NOT operators. An atomic
  pattern filters for the value of a field of the result item and has the form <field_name>(<pattern>).
  The following table shows the valid field names. You can omit the name field and simply use the name
  pattern as a filter expression. It must be quoted if it contains whitespace or parentheses.

  \tabular{ll}{
    name \tab the name of the scalar, vector or statistic \cr
    module \tab the name of the module \cr
    file \tab the file of the result item \cr
    run \tab the run identifier \cr
    attr:\emph{name} \tab the value of the run attribute named \emph{name}, e.g. attr:experiment \cr
    param:\emph{name} \tab the value of the module parameter named \emph{name} \cr
  }

  In the pattern specifying the field value, you can use the following shortcuts:

  \tabular{ll}{
    ? \tab matches any character except '.' \cr
    \* \tab matches zero or more characters except '.' \cr
    \*\* \tab matches zero or more characters (any character) \cr
    \{a-z\} \tab matches a character in range a-z \cr
    \{^a-z\} \tab matches a character not in range a-z\cr
    \{32..255\} \tab any number (i.e. sequence of digits) in range 32..255 (e.g. "99") \cr
    [32..255] \tab any number in square brackets in range 32..255 (e.g. "[99]") \cr
    \\ \tab takes away the special meaning of the subsequent character \cr
  }
}

\seealso{
  \code{\link{loadDataset}}, \code{\link{loadVectors}}
}

\examples{
  'name("channel utilization")'

  'module("Aloha.Server") AND name("collision length")'
}
