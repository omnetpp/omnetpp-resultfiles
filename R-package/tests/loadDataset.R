require(omnetpp)
datadir <- system.file('extdata', package='omnetpp')
d <- loadDataset(c(file.path(datadir, 'PureAloha1-*.sca'), file.path(datadir, 'PureAloha1-0.vec')),
       add('scalar'),
       add('vector'))

print(d)
