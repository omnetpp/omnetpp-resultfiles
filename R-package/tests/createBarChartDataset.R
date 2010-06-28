require(omnetpp)
dataset <- loadDataset(system.file('extdata/PureAloha1-0.sca', package='omnetpp'), add('scalar'))
d <- makeBarChartDataset(dataset, rows=c('module','experiment','measurement'), columns=c('name'))
print(d)
