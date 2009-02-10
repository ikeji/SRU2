# Programing Language SRU
# Copyright(C) 2005-2008 IKeJI
#

OBJS     = ast.o basic_object.o class.o interpreter.o library.o native_proc.o object_pool.o object_vector.o proc.o stack_frame.o string.o
TESTS    = ast_test.o basic_object_test.o class_test.o interpreter_test.o library_test.o native_proc_test.o object_pool_test.o object_vector_test.o proc_test.o stack_frame_test.o string_test.o
GENFILES = testing_ast.h
CXXFLAGS = -g -O0 -Wall -Werror -Weffc++
ERB      = erb

all: sru sru_test tags

test: all
	./sru_test

sru: $(OBJS) main.o
	$(CXX) -g -o sru $(OBJS) main.o

sru_test: testing.o $(TESTS) $(OBJS)
	$(CXX) -g -o sru_test $(TESTS) testing.o $(OBJS)

clean:
	rm -f *.o sru sru_test *~ Makefile.bak tags $(GENFILES)

depend: $(GENFILES)
	makedepend *.cc

tags:
	ctags *

.SUFFIXES: .h .herb

.herb.h:
	$(ERB) < $< > $@.tmp
	mv $@.tmp $@

# DO NOT DELETE

ast.o: ast.h basic_object.h object_vector.h
ast_test.o: testing.h ast.h basic_object.h object_vector.h
basic_object.o: basic_object.h object_pool.h
basic_object_test.o: testing.h basic_object.h
class.o: class.h basic_object.h
class_test.o: testing.h class.h basic_object.h
interpreter.o: interpreter.h ast.h basic_object.h object_vector.h
interpreter.o: stack_frame.h library.h
interpreter_test.o: interpreter.h testing.h basic_object.h ast.h
interpreter_test.o: object_vector.h string.h library.h
library.o: library.h basic_object.h
library_test.o: testing.h library.h basic_object.h
native_proc.o: native_proc.h proc.h basic_object.h object_vector.h
native_proc.o: interpreter.h stack_frame.h
native_proc_test.o: testing.h native_proc.h proc.h basic_object.h
native_proc_test.o: object_vector.h library.h stack_frame.h interpreter.h
object_pool.o: object_pool.h basic_object.h object_vector.h
object_pool_test.o: testing.h basic_object.h object_pool.h
object_vector.o: object_vector.h basic_object.h
object_vector_test.o: testing.h object_vector.h basic_object.h object_pool.h
proc.o: proc.h basic_object.h object_vector.h stack_frame.h interpreter.h
proc.o: library.h
proc_test.o: testing.h proc.h basic_object.h object_vector.h ast.h
proc_test.o: interpreter.h testing_ast.h library.h
stack_frame.o: stack_frame.h basic_object.h object_vector.h string.h ast.h
stack_frame.o: proc.h
stack_frame_test.o: testing.h stack_frame.h basic_object.h object_vector.h
stack_frame_test.o: ast.h
string.o: string.h basic_object.h library.h class.h
string_test.o: testing.h string.h basic_object.h library.h
testing.o: testing.h
