#
# Copyright (c) 2010 Opensim Ltd.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of the Opensim Ltd. nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL Andras Varga or Opensim Ltd. BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

# filters

# TODO conflicts with loadDataset(add())
#add <- function (c) {
#}

compare <- function (eq, gt, lt, x) {
  list("compare", ifEqual=eq, ifGreater=gt, ifLess=lt, threshold=x)
}

crop <- function (t1, t2) {
  list("crop", t1=t1, t2=t2)
}

difference <- function () {
  list("difference")
}

diffquot <- function () {
  list("diffquot")
}

divideBy <- function (a) {
  list("divide-by", a=a)
}

divtime <- function () {
  list("divtime")
}

expression <- function (expression) {
  list("expression", expression=expression)
}

integrate <- function (interpolationMode) {
  list("integrate", 'interpolation-mode'=interpolationMode)
}

lineartrend <- function (a) {
  list("lineartrend", a=a)
}

mean <- function () {
  list("mean")
}

modulo <- function (a) {
  list("modulo", a=a)
}

movingavg <- function (alpha) {
  list("movingavg", alpha=alpha)
}

multiplyBy <- function (a) {
  list("multiply-by", a=a)
}

removerepeats <- function () {
  list("removerepeats")
}

slidingwinavg <- function (windowSize) {
  list("slidingwinavg", windowSize=windowSize)
}

sum <- function () {
  list("sum")
}

timeavg <- function(interpolationMode) {
  list("timeavg", 'interpolation-mode'=interpolationMode)
}

timediff <- function() {
  list("timediff")
}

timeshift <- function(dt) {
  list("timeshift", dt=dt)
}

timetoserial <- function () {
  list("timetoserial")
}

timewinavg <- function (windowSize) {
  list("timewinavg", windowSize=windowSize)
}

winavg <- function (windowSize) {
  list("winavg", windowSize=windowSize)
}

# Commands

apply <- function (fun, select) {
  if(missing(select)) {
    list("apply", fun=fun)
  }
  else {
    list("apply", fun=fun, select=select)
  }
}

compute <- function (fun, select) {
  if(missing(select)) {
    list("compute", fun=fun)
  }
  else {
    list("compute", fun=fun, select=select)
  }
}

loadVectors <- function (vectors, ...) {
  vectors$file <- as.character(vectors$file)
  commands <- list(...)

  dataset <- .Call('callLoadVectors', vectors, commands)

  structure(
    list(
      vectors = as.data.frame(dataset$vectors),
      vectordata = as.data.frame(dataset$vectordata),
      attributes = as.data.frame(dataset$attributes)
    ),
    class='omnetpp_dataset'
  )
}
