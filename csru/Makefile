# Programing Language SRU
# Copyright(C) 2005-2008 IKeJI
#

OBJS     = basic_object.o class.o library.o object_pool.o object_vector.o string.o
TESTS    = basic_object_test.o class_test.o library_test.o object_pool_test.o object_vector_test.o string_test.o
CXXFLAGS = -g -O0 -Wall -Werror -Weffc++

all: sru sru_test tags

test: sru_test tags
	./sru_test

sru: $(OBJS) main.o
	$(CXX) -g -o sru $(OBJS) main.o

sru_test: testing.o $(TESTS) $(OBJS)
	$(CXX) -g -o sru_test $(TESTS) testing.o $(OBJS)

clean:
	rm -f *.o sru sru_test *~ Makefile.bak tags

depend: 
	makedepend *.cc

tags:
	ctags *

# DO NOT DELETE

basic_object.o: basic_object.h object_pool.h
basic_object_test.o: testing.h basic_object.h
class.o: class.h basic_object.h
class_test.o: testing.h class.h basic_object.h
library.o: library.h basic_object.h
library_test.o: testing.h library.h basic_object.h
object_pool.o: object_pool.h basic_object.h object_vector.h
object_pool_test.o: testing.h basic_object.h object_pool.h
object_vector.o: object_vector.h basic_object.h
object_vector_test.o: testing.h object_vector.h
string.o: string.h basic_object.h library.h class.h
string_test.o: testing.h string.h basic_object.h library.h
testing.o: testing.h
