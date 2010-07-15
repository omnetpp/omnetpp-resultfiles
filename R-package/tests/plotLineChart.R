require(omnetpp)
d <- loadDataset(system.file('extdata/PureAloha1-0.vec', package='omnetpp'),
                 add('vector', select='name("channel utilization")'))
d <- loadVectors(d, NULL, apply(winavg(windowSize=10)), compute(multiplyBy(2)))
vectors <- makeLineChartDataset(d)
plotLineChart(vectors, list())
