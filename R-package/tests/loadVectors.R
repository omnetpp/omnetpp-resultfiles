library('omnetpp')
dataset <- loadDataset(system.file('extdata/PureAloha1-0.vec', package='omnetpp'),
             add('vector', select='name("channel utilization")'))

ops <- list(compare(0,1,-1,2),
            crop(0, 1),
            difference(),
            diffquot(),
            divideBy(2),
            divtime(),
            expression('yprev'),
            integrate('linear'),
            lineartrend(1),
            mean(),
            modulo(2),
            movingavg(0.3),
            multiplyBy(2),
            removerepeats(),
            slidingwinavg(10),
            sum(),
            timeavg('linear'),
            timediff(),
            timeshift(dt=0.1),
            timetoserial(),
            timewinavg(10),
            winavg(10))            

for (op in ops) {
  print(loadVectors(dataset$vectors, apply(op)))
  print(loadVectors(dataset$vectors, compute(op)))
}
