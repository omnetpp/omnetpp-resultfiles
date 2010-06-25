require(omnetpp)
dataset <- loadDataset(system.file('extdata/PureAloha1-0.sca', package='omnetpp'), add('statistic'))
histograms <- createHistograms(dataset)
plotHistogramChart(histograms, list(Hist.Bar='Solid', Hist.Data='Count'))
