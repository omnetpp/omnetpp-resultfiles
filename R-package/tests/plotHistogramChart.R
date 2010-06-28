require(omnetpp)
dataset <- loadDataset(system.file('extdata/PureAloha1-0.sca', package='omnetpp'), add('statistic'))
histograms <- makeHistograms(dataset)
plotHistogramChart(histograms, list(Hist.Bar='Solid', Hist.Data='Count'))
