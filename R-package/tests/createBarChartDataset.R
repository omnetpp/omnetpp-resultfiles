library('omnetpp')
dataset <- loadDataset(add('scalar', system.file('extdata/PureAloha1-0.sca', package='omnetpp')))
d <- createBarChartDataset(dataset, rows=c('module','experiment','measurement'), columns=c('name'))
print(d)
