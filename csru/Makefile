# Programing Language SRU
# Copyright(C) 2005-2008 IKeJI
#

OBJS     = \
ast.o \
basic_object.o \
binding.o \
boolean.o \
class.o \
interpreter.o \
library.o \
native_proc.o \
numeric.o \
object.o \
object_pool.o \
object_vector.o \
parser.o \
proc.o \
stack_frame.o \
string.o \

TESTS    = \
ast_test.o \
basic_object_test.o \
binding_test.o \
boolean_test.o \
class_test.o \
interpreter_test.o \
library_test.o \
native_proc_test.o \
numeric_test.o \
object_pool_test.o \
object_test.o \
object_vector_test.o \
parser_test.o \
proc_test.o \
stack_frame_test.o \
string_test.o \

DFLAGS   = -DDEBUG
GENFILES = testing_ast.h testing_sru.h
CXXFLAGS = -g -O0 -Wall -Werror -Weffc++ $(DFLAGS)
ERB      = erb

ifeq ($(findstring CYGWIN_NT,$(shell uname)),CYGWIN_NT)
  # g++-4 doesn't support mingw now. use g++-3
  CXX      = g++-3
  # g++-3 doesn't work with -Weffc++
  CXXFLAGS = -g -O0 -Wall -Werror -mno-cygwin -I/usr/include/mingw $(DFLAGS)
  LDFLAGS  = -mno-cygwin -L/usr/lib/mingw
else
endif

all: sru sru_test

test: all
	./sru_test

sru: $(OBJS) main.o
	$(CXX) $(LDFLAGS) -g -o sru $(OBJS) main.o

sru_test: testing.o $(TESTS) $(OBJS)
	$(CXX) $(LDFLAGS) -g -o sru_test $(TESTS) testing.o $(OBJS)

clean:
	rm -f *.o sru sru_test *~ .gitignore~ Makefile.bak tags $(GENFILES)

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
basic_object.o: basic_object.h object_pool.h constants.h string.h
basic_object_test.o: testing.h basic_object.h
binding.o: binding.h basic_object.h library.h constants.h class.h string.h
binding_test.o: testing.h testing_ast.h ast.h basic_object.h object_vector.h
binding_test.o: interpreter.h library.h
boolean.o: boolean.h basic_object.h class.h library.h native_proc.h proc.h
boolean.o: object_vector.h string.h constants.h testing_sru.h interpreter.h
boolean.o: stack_frame.h
boolean_test.o: testing.h library.h basic_object.h constants.h
class.o: class.h basic_object.h constants.h library.h string.h
class_test.o: testing.h class.h basic_object.h constants.h library.h
interpreter.o: interpreter.h object_vector.h ast.h basic_object.h
interpreter.o: stack_frame.h library.h binding.h constants.h class.h
interpreter_test.o: interpreter.h object_vector.h testing.h testing_ast.h
interpreter_test.o: ast.h basic_object.h string.h library.h constants.h
library.o: library.h basic_object.h numeric.h parser.h object.h string.h
library.o: constants.h binding.h class.h boolean.h proc.h object_vector.h
library_test.o: testing.h library.h basic_object.h
native_proc.o: native_proc.h proc.h basic_object.h object_vector.h
native_proc.o: interpreter.h stack_frame.h
native_proc_test.o: testing.h native_proc.h proc.h basic_object.h
native_proc_test.o: object_vector.h library.h stack_frame.h interpreter.h
numeric.o: numeric.h basic_object.h class.h library.h native_proc.h proc.h
numeric.o: object_vector.h string.h constants.h
numeric_test.o: testing.h numeric.h basic_object.h library.h testing_sru.h
numeric_test.o: proc.h object_vector.h interpreter.h stack_frame.h string.h
numeric_test.o: constants.h
object.o: object.h basic_object.h string.h constants.h class.h library.h
object_pool.o: object_pool.h basic_object.h object_vector.h
object_pool_test.o: testing.h basic_object.h object_pool.h
object_test.o: testing.h object.h basic_object.h
object_vector.o: object_vector.h basic_object.h
object_vector_test.o: testing.h object_vector.h basic_object.h object_pool.h
parser.o: parser.h basic_object.h string.h constants.h
parser_test.o: parser.h basic_object.h testing.h
proc.o: proc.h basic_object.h object_vector.h ast.h binding.h library.h
proc.o: constants.h class.h interpreter.h stack_frame.h native_proc.h
proc.o: string.h testing_ast.h testing_sru.h
proc_test.o: testing.h proc.h basic_object.h object_vector.h ast.h
proc_test.o: interpreter.h testing_ast.h library.h
stack_frame.o: stack_frame.h basic_object.h object_vector.h string.h ast.h
stack_frame.o: proc.h library.h binding.h constants.h class.h
stack_frame_test.o: testing.h stack_frame.h basic_object.h object_vector.h
stack_frame_test.o: ast.h binding.h library.h constants.h class.h
string.o: string.h basic_object.h library.h class.h
string_test.o: testing.h string.h basic_object.h library.h constants.h
testing.o: testing.h
