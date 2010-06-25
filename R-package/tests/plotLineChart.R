library('omnetpp')
d <- loadDataset(system.file('extdata/PureAloha1-0.vec', package='omnetpp'),
                 add('vector', select='name("channel utilization")'))
vectors <- merge(d$files, d$vectors)[,c('ospath', 'vectorid')]
d <- loadVectors(vectors, apply(winavg(windowSize=10)), compute(multiplyBy(2)))
vectors <- split(d$vectordata, d$vectordata$vector_key)
plotLineChart(vectors, list())
