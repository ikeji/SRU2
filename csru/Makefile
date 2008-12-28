# Programing Language SRU
# Copyright(C) 2005-2008 IKeJI
#

OBJS	= basic_object.o object_pool.o
TESTS   = basic_object_test.o object_pool_test.o
CPP	= g++

all: sru

sru: $(OBJS) main.o
	$(CPP) -g -o sru $(OBJS) main.o

.cc.o:
	$(CPP) -g -c $<

basic_object.o: basic_object.cc basic_object.h object_pool.h
basic_object_test.o: basic_object_test.cc basic_object.h testing.h
object_pool.o: object_pool.cc basic_object.h object_pool.h
object_pool_test.o: object_pool_test.cc basic_object.h object_pool.h testing.h

testing.o: testing.cc testing.h

clean:
	rm -f *.o sru sru_test *~

test: sru_test
	./sru_test

sru_test: testing.o $(TESTS) $(OBJS)
	$(CPP) -g -o sru_test $(TESTS) testing.o $(OBJS)

