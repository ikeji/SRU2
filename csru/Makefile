# Programing Language SRU
# Copyright(C) 2005-2008 IKeJI
#

OBJS     = ast.o basic_object.o class.o interpreter.o library.o object_pool.o object_vector.o proc.o stack_frame.o string.o
TESTS    = ast_test.o basic_object_test.o class_test.o interpreter_test.o library_test.o object_pool_test.o object_vector_test.o proc_test.o stack_frame_test.o string_test.o
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

