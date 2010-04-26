# Programing Language SRU
# Copyright(C) 2005-2008 IKeJI
#

OBJS     = \
ast.o \
basic_object.o \
binding.o \
boolean.o \
class.o \
constants.o \
interpreter.o \
library.o \
logging.o \
native_proc.o \
numeric.o \
object.o \
object_pool.o \
object_container.o \
parser.o \
parser_spacing.o \
parser_constants.o \
proc.o \
stack_frame.o \
string.o \
symbol.o \

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
object_container_test.o \
object_pool_test.o \
object_test.o \
parser_test.o \
proc_test.o \
sru_regression_test.o \
stack_frame_test.o \
string_test.o \
symbol_test.o \

DFLAGS   = -DDEBUG -g -O0
GENFILES = testing_ast.h testing_sru.h parser.cc constants.cc constants.h
CXXFLAGS = -Wall -Werror -Weffc++ $(DFLAGS)
LDFLAGS  = $(DFLAGS)
CPFLAGS  = -u
ERB      = erb

ifeq ($(findstring FreeBSD,$(shell uname)),FreeBSD)
  # BSD cp doesn't support -u option.
  CPFLAGS = -p
endif

ifeq ($(findstring CYGWIN_NT,$(shell uname)),CYGWIN_NT)
  # g++-4 doesn't support mingw now. use g++-3
  CXX      = g++-3
  # g++-3 doesn't work with -Weffc++
  CXXFLAGS = -Wall -Werror $(DFLAGS) -mno-cygwin -I/usr/include/mingw -DUSEOLDGCC
  LDFLAGS  = $(DFLAGS) -mno-cygwin -L/usr/lib/mingw
endif

all: parser sru sru_test

test: all
	./sru_test

sru: $(OBJS) main.o
	$(CXX) $(LDFLAGS) -g -o sru $(OBJS) main.o

sru_test: testing.o $(TESTS) $(OBJS)
	$(CXX) $(LDFLAGS) -g -o sru_test $(TESTS) testing.o $(OBJS)

clean:
	make -C parser_generator clean
	rm -f *.o sru sru_test *~ .gitignore~ Makefile.bak tags $(GENFILES)

depend: parser $(GENFILES)
	make -C parser_generator depend
	makedepend *.cc

tags:
	ctags *

.SUFFIXES: .h .herb

.herb.h:
	ruby -e 'n=2;ARGF.each{|l|puts "#{l}#line #{n} \"#{ARGF.filename}\"";n+=1}' $< | $(ERB) > $@.tmp
	mv $@.tmp $@

parser.cc: parser parser_generator/parser.cc
	cp $(CPFLAGS) parser_generator/parser.cc parser.cc

symbols: parser parser_generator/symbols
	cp $(CPFLAGS) parser_generator/symbols symbols

constants.h: constants_generator.rb symbols
	ruby constants_generator.rb -h > $@.tmp
	mv $@.tmp $@

constants.cc: constants_generator.rb symbols
	ruby constants_generator.rb -c > $@.tmp
	mv $@.tmp $@

parser:
	make -C parser_generator

# DO NOT DELETE

ast.o: ast.h basic_object.h logging.h symbol.h object_container.h library.h
ast.o: string.h
ast_test.o: testing.h ast.h basic_object.h logging.h symbol.h
ast_test.o: object_container.h
basic_object.o: basic_object.h logging.h symbol.h object_pool.h constants.h
basic_object.o: string.h
basic_object_test.o: testing.h basic_object.h logging.h symbol.h
binding.o: binding.h basic_object.h logging.h symbol.h string.h class.h
binding.o: library.h constants.h native_proc.h proc.h object_container.h
binding.o: interpreter.h stack_frame.h
binding_test.o: testing.h testing_ast.h ast.h basic_object.h logging.h
binding_test.o: symbol.h object_container.h string.h interpreter.h library.h
binding_test.o: constants.h
boolean.o: boolean.h basic_object.h logging.h symbol.h class.h library.h
boolean.o: native_proc.h proc.h object_container.h string.h constants.h
boolean.o: testing_sru.h interpreter.h stack_frame.h
boolean_test.o: testing.h library.h basic_object.h logging.h symbol.h
boolean_test.o: constants.h
class.o: class.h basic_object.h logging.h symbol.h constants.h library.h
class.o: string.h native_proc.h proc.h object_container.h interpreter.h
class.o: stack_frame.h
class_test.o: testing.h class.h basic_object.h logging.h symbol.h constants.h
class_test.o: library.h
constants.o: constants.h symbol.h
interpreter.o: interpreter.h object_container.h ast.h basic_object.h
interpreter.o: logging.h symbol.h stack_frame.h library.h binding.h
interpreter.o: native_proc.h proc.h constants.h string.h
interpreter_test.o: interpreter.h object_container.h testing.h testing_ast.h
interpreter_test.o: ast.h basic_object.h logging.h symbol.h string.h
interpreter_test.o: library.h constants.h
library.o: library.h basic_object.h logging.h symbol.h numeric.h parser.h
library.o: object.h string.h constants.h binding.h boolean.h proc.h
library.o: object_container.h class.h
library_test.o: testing.h library.h basic_object.h logging.h symbol.h
logging.o: logging.h
main.o: interpreter.h object_container.h basic_object.h logging.h symbol.h
native_proc.o: native_proc.h proc.h basic_object.h logging.h symbol.h
native_proc.o: object_container.h interpreter.h stack_frame.h
native_proc_test.o: testing.h native_proc.h proc.h basic_object.h logging.h
native_proc_test.o: symbol.h object_container.h library.h stack_frame.h
native_proc_test.o: interpreter.h
numeric.o: numeric.h basic_object.h logging.h symbol.h class.h library.h
numeric.o: native_proc.h proc.h object_container.h string.h constants.h
numeric_test.o: testing.h numeric.h basic_object.h logging.h symbol.h
numeric_test.o: library.h testing_sru.h proc.h object_container.h
numeric_test.o: interpreter.h stack_frame.h string.h constants.h
numeric_test.o: testing_ast.h ast.h
object.o: object.h basic_object.h logging.h symbol.h string.h constants.h
object.o: class.h library.h
object_container.o: object_container.h basic_object.h logging.h symbol.h
object_container_test.o: testing.h object_container.h basic_object.h
object_container_test.o: logging.h symbol.h object_pool.h
object_pool.o: object_pool.h basic_object.h logging.h symbol.h
object_pool.o: object_container.h
object_pool_test.o: testing.h basic_object.h logging.h symbol.h object_pool.h
object_test.o: testing.h object.h basic_object.h logging.h symbol.h
parser.o: parser.h basic_object.h logging.h symbol.h native_proc.h proc.h
parser.o: object_container.h constants.h binding.h string.h numeric.h
parser.o: library.h testing_ast.h ast.h testing_sru.h interpreter.h
parser.o: stack_frame.h
parser_constants.o: native_proc.h proc.h basic_object.h logging.h symbol.h
parser_constants.o: object_container.h constants.h library.h string.h
parser_constants.o: numeric.h testing_ast.h ast.h
parser_helper.o: native_proc.h proc.h basic_object.h logging.h symbol.h
parser_helper.o: object_container.h string.h numeric.h library.h ast.h
parser_helper.o: constants.h testing_ast.h
parser_spacing.o: native_proc.h proc.h basic_object.h logging.h symbol.h
parser_spacing.o: object_container.h string.h numeric.h constants.h library.h
parser_test.o: parser.h basic_object.h logging.h symbol.h testing.h
parser_test.o: interpreter.h object_container.h numeric.h library.h
parser_test.o: constants.h string.h
proc.o: proc.h basic_object.h logging.h symbol.h object_container.h ast.h
proc.o: binding.h interpreter.h library.h stack_frame.h constants.h class.h
proc.o: native_proc.h string.h testing_ast.h testing_sru.h
proc_test.o: testing.h proc.h basic_object.h logging.h symbol.h
proc_test.o: object_container.h ast.h interpreter.h testing_ast.h string.h
proc_test.o: library.h numeric.h constants.h
sru_regression_test.o: testing.h logging.h basic_object.h symbol.h
sru_regression_test.o: interpreter.h object_container.h
stack_frame.o: stack_frame.h basic_object.h logging.h symbol.h
stack_frame.o: object_container.h string.h ast.h proc.h library.h binding.h
stack_frame.o: constants.h
stack_frame_test.o: testing.h stack_frame.h basic_object.h logging.h symbol.h
stack_frame_test.o: object_container.h ast.h binding.h
string.o: string.h basic_object.h logging.h symbol.h library.h class.h
string_test.o: testing.h string.h basic_object.h logging.h symbol.h library.h
string_test.o: constants.h
symbol.o: symbol.h logging.h
symbol_test.o: symbol.h testing.h
testing.o: testing.h logging.h
