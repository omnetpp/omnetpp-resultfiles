TARGET = BasicExample
OBJS = BasicExample.o

CXX = g++
INCLDIR = ../include
LIBDIR = ../lib

all: $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) -L $(LIBDIR) -lresultwriter

.SUFFIXES: .cc

%.o: %.cc
	$(CXX) -c -I $(INCLDIR) -o $@ $<

clean:
	-rm -rf *.o $(TARGET) $(TARGET).exe


