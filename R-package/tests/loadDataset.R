require(omnetpp)
datadir <- system.file('extdata', package='omnetpp')
filter <- 'name(duration)'
d <- loadDataset(c(file.path(datadir, 'PureAloha1-*.sca'), file.path(datadir, 'PureAloha1-0.vec')),
       add('scalar', filter),
       add('vector'))

print(d)
