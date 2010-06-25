require(omnetpp)
d <- loadDataset(system.file('extdata/PureAloha1-0.vec', package='omnetpp'),
                 add('vector', select='name("channel utilization")'))
d <- loadVectors(d$vectors, apply(winavg(windowSize=10)), compute(multiplyBy(2)))
vectors <- split(d$vectordata, d$vectordata$resultkey)
plotLineChart(vectors, list())
