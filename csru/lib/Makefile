# Programing Language SRU
# Copyright(C) 2005-2010 IKeJI
#

include ../Makefile.in

CXXFLAGS += -I../..

hello.so: hellolib.cc
	$(CXX) $(LDFLAGS) $(CXXFLAGS) hellolib.cc -o hello.so -shared ../libsru.$(LIBEXT)

clean:
	rm -f *.o *.so *~
