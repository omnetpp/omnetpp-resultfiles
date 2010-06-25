require(omnetpp)

scalarFiles <- file.path(system.file('extdata', package='omnetpp'), 'PureAlohaExperiment-*.sca')
vectorFiles <- c(system.file('extdata', 'OneFifo-0.vec', package='omnetpp'),
                 system.file('extdata', 'TokenRing1-0.vec', package='omnetpp'))

#
# Load everything from scalar files. 
#
dataset <- loadDataset(scalarFiles)

#
# Load scalars only.
#
dataset <- loadDataset(scalarFiles, add(type='scalar'))

#
# Load everything except histograms.
#
dataset <- loadDataset(scalarFiles,
                       add(select='*'),
                       discard(type='histogram'))

#
# Load selected scalars.
#
dataset <- loadDataset(scalarFiles,
                       add(type='scalar',
                           select='module(Aloha.server) AND name("channel utilization")'))

#
# Load result items from vector files.
#
dataset <- loadDataset(vectorFiles)

#
# Load data of all vectors.
#
data <- loadVectors(dataset$vectors)
print(data$vectors[,c('module','name')])

#
# Load queueing times from the TokenRing simulation and compute their winavg too.
#
dataset <- loadDataset(vectorFiles, add(select='module("TokenRing.comp[*].mac") AND name("Queueing time (sec)")'))
data <- loadVectors(dataset$vectors, compute(winavg(windowSize=10)))
print(data$vectors[,c('module','name')])

#
# See the structure of the OMNeT++ dataset.
#
dataset <- loadDataset(c(scalarFiles, vectorFiles))
print(str(dataset))
