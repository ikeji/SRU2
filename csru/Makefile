# Programing Language SRU
# Copyright(C) 2005-2008 IKeJI
#

OBJS     = basic_object.o object_pool.o library.o
TESTS    = basic_object_test.o object_pool_test.o library_test.o
CXXFLAGS = -g

all: sru

sru: $(OBJS) main.o
	$(CXX) -g -o sru $(OBJS) main.o

clean:
	rm -f *.o sru sru_test *~

depend:
	makedepend *.cc

test: sru_test
	./sru_test

sru_test: testing.o $(TESTS) $(OBJS)
	$(CXX) -g -o sru_test $(TESTS) testing.o $(OBJS)

# DO NOT DELETE

basic_object.o: basic_object.h object_pool.h
basic_object_test.o: testing.h basic_object.h
library.o: library.h basic_object.h
library_test.o: testing.h library.h
object_pool.o: object_pool.h basic_object.h
object_pool_test.o: testing.h basic_object.h object_pool.h
testing.o: testing.h
