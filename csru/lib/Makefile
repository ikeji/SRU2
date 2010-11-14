# Programing Language SRU
# Copyright(C) 2005-2010 IKeJI
#

include ../Makefile.in

CXXFLAGS += -I../..
LDFLAGS += -L../

hello.so: hellolib.cc
	$(CXX) $(LDFLAGS) $(CXXFLAGS) hellolib.cc -o hello.so -shared -lsru

clean:
	rm -f *.o *.so *~
