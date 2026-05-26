# Programing Language SRU
# Copyright(C) 2005-2010 IKeJI
#

include ../Makefile.in

CXXFLAGS += -I../..
LDFLAGS += -L../

all: hello.so ffi.so

hello.so: hellolib.cc
	$(CXX) $(LDFLAGS) $(CXXFLAGS) hellolib.cc -o hello.so -shared -lsru

ffi.so: ffi.cc
	$(CXX) $(LDFLAGS) $(CXXFLAGS) ffi.cc -o ffi.so -shared -lsru -ldl -lffi

clean:
	rm -f *.o *.so *~
