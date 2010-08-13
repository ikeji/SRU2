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
object_container.o \
object_pool.o \
parser.o \
parser_call.o \
parser_class.o \
parser_closure.o \
parser_constants.o \
parser_def.o \
parser_expression.o \
parser_helper.o \
parser_if.o \
parser_let.o \
parser_method_call.o \
parser_program.o \
parser_spacing.o \
parser_statements.o \
parser_util.o \
parser_while.o \
proc.o \
require.o \
sru_array.o \
sru_string.o \
stack_frame.o \
symbol.o \
utils.o \

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
sru_string_test.o \
symbol_test.o \

DFLAGS   = -DDEBUG -g -O0
GENFILES = testing_ast.h testing_sru.h parser.cc constants.cc constants.h
CXXFLAGS = -Wall -Werror -Weffc++ -DSRU_MAIN $(DFLAGS)
LDFLAGS  = $(DFLAGS) -Wl,-export-dynamic
CPFLAGS  = -u
ERB      = erb

ifeq ($(findstring FreeBSD,$(shell uname)),FreeBSD)
  # BSD cp doesn't support -u option.
  CPFLAGS = -p
endif

ifeq ($(findstring Linux,$(shell uname)),Linux)
  LDFLAGS  += -ldl
endif

ifeq ($(findstring CYGWIN_NT,$(shell uname)),CYGWIN_NT)
  # g++-4 doesn't support mingw now. use g++-3
  CXX      = g++-3
  # g++-3 doesn't work with -Weffc++
  CXXFLAGS = -Wall -Werror $(DFLAGS) -mno-cygwin -I/usr/include/mingw -DUSEOLDGCC -DSRU_MAIN
  LDFLAGS  = $(DFLAGS) -mno-cygwin -L/usr/lib/mingw
endif

all: sru tags library

test: all sru_test
	./sru_test

sru: $(OBJS) main.o
	$(CXX) $(LDFLAGS) -g -o sru $(OBJS) main.o

sru_test: testing.o $(TESTS) $(OBJS)
	$(CXX) $(LDFLAGS) -g -o sru_test $(TESTS) testing.o $(OBJS)

clean:
	$(MAKE) -C parser_generator clean
	rm -f *.o *.so sru sru_test *~ .gitignore~ Makefile.bak tags symbols $(GENFILES)

depend: parser $(GENFILES)
	$(MAKE) -C parser_generator depend
	makedepend *.cc

tags:
	ctags *

.SUFFIXES: .h .herb

.herb.h:
	ruby -e 'n=2;ARGF.each{|l|puts "#{l}#line #{n} \"#{ARGF.filename}\"";n+=1}' $< | $(ERB) > $@.tmp
	mv $@.tmp $@

parser.cc: parser_generator/parser.cc
	cp $(CPFLAGS) parser_generator/parser.cc parser.cc

parser_generator/parser.cc: parser

symbols: parser_generator/symbols
	cp $(CPFLAGS) parser_generator/symbols symbols

parser_generator/symbols: parser

constants.h: constants_generator.rb symbols
	ruby constants_generator.rb -h > $@.tmp
	mv $@.tmp $@

constants.cc: constants_generator.rb symbols
	ruby constants_generator.rb -c > $@.tmp
	mv $@.tmp $@

parser:
	$(MAKE) -C parser_generator

library:
	$(MAKE) -C lib

# DO NOT DELETE

ast.o: ast.h basic_object.h logging.h symbol.h object_container.h library.h
ast.o: sru_string.h utils.h
ast_test.o: testing.h ast.h basic_object.h logging.h symbol.h
ast_test.o: object_container.h
basic_object.o: basic_object.h logging.h symbol.h object_pool.h constants.h
basic_object.o: sru_string.h
basic_object_test.o: testing.h basic_object.h logging.h symbol.h
binding.o: binding.h basic_object.h logging.h symbol.h sru_string.h class.h
binding.o: library.h constants.h native_proc.h proc.h object_container.h
binding.o: interpreter.h stack_frame.h
binding_test.o: testing.h testing_ast.h ast.h basic_object.h logging.h
binding_test.o: symbol.h object_container.h sru_string.h interpreter.h
binding_test.o: library.h constants.h
boolean.o: boolean.h basic_object.h logging.h symbol.h class.h library.h
boolean.o: native_proc.h proc.h object_container.h sru_string.h constants.h
boolean.o: testing_sru.h interpreter.h stack_frame.h
boolean_test.o: testing.h library.h basic_object.h logging.h symbol.h
boolean_test.o: constants.h
class.o: class.h basic_object.h logging.h symbol.h constants.h library.h
class.o: sru_string.h native_proc.h proc.h object_container.h interpreter.h
class.o: stack_frame.h
class_test.o: testing.h class.h basic_object.h logging.h symbol.h constants.h
class_test.o: library.h
constants.o: constants.h symbol.h
interpreter.o: interpreter.h object_container.h ast.h basic_object.h
interpreter.o: logging.h symbol.h stack_frame.h library.h binding.h
interpreter.o: native_proc.h proc.h constants.h numeric.h sru_string.h
interpreter.o: utils.h require.h
interpreter_test.o: interpreter.h object_container.h testing.h testing_ast.h
interpreter_test.o: ast.h basic_object.h logging.h symbol.h sru_string.h
interpreter_test.o: library.h constants.h
library.o: library.h basic_object.h logging.h symbol.h numeric.h parser.h
library.o: object.h sru_string.h constants.h binding.h boolean.h proc.h
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
numeric.o: native_proc.h proc.h object_container.h sru_string.h constants.h
numeric_test.o: testing.h numeric.h basic_object.h logging.h symbol.h
numeric_test.o: library.h testing_sru.h proc.h object_container.h
numeric_test.o: interpreter.h stack_frame.h sru_string.h constants.h
numeric_test.o: testing_ast.h ast.h
object.o: object.h basic_object.h logging.h symbol.h sru_string.h constants.h
object.o: class.h library.h
object_container.o: object_container.h basic_object.h logging.h symbol.h
object_container_test.o: testing.h object_container.h basic_object.h
object_container_test.o: logging.h symbol.h object_pool.h
object_pool.o: object_pool.h basic_object.h logging.h symbol.h
object_pool.o: object_container.h
object_pool_test.o: testing.h basic_object.h logging.h symbol.h object_pool.h
object_test.o: testing.h object.h basic_object.h logging.h symbol.h
parser.o: parser.h basic_object.h logging.h symbol.h native_proc.h proc.h
parser.o: object_container.h constants.h binding.h sru_string.h numeric.h
parser.o: library.h stack_frame.h interpreter.h parser_helper.h parser_util.h
parser.o: testing_ast.h ast.h testing_sru.h
parser_call.o: native_proc.h proc.h basic_object.h logging.h symbol.h
parser_call.o: object_container.h constants.h library.h numeric.h
parser_call.o: parser_util.h testing_ast.h ast.h sru_string.h
parser_class.o: native_proc.h proc.h basic_object.h logging.h symbol.h
parser_class.o: object_container.h constants.h library.h numeric.h
parser_class.o: parser_util.h utils.h testing_ast.h ast.h sru_string.h
parser_closure.o: native_proc.h proc.h basic_object.h logging.h symbol.h
parser_closure.o: object_container.h constants.h library.h numeric.h
parser_closure.o: parser_util.h testing_ast.h ast.h sru_string.h
parser_constants.o: native_proc.h proc.h basic_object.h logging.h symbol.h
parser_constants.o: object_container.h constants.h sru_string.h numeric.h
parser_constants.o: parser_util.h testing_ast.h ast.h
parser_def.o: native_proc.h proc.h basic_object.h logging.h symbol.h
parser_def.o: object_container.h constants.h library.h numeric.h
parser_def.o: parser_util.h testing_ast.h ast.h sru_string.h
parser_expression.o: native_proc.h proc.h basic_object.h logging.h symbol.h
parser_expression.o: object_container.h constants.h library.h numeric.h
parser_expression.o: parser_util.h testing_ast.h ast.h sru_string.h
parser_helper.o: native_proc.h proc.h basic_object.h logging.h symbol.h
parser_helper.o: object_container.h sru_string.h numeric.h library.h ast.h
parser_helper.o: constants.h parser_util.h testing_ast.h
parser_if.o: native_proc.h proc.h basic_object.h logging.h symbol.h
parser_if.o: object_container.h constants.h library.h numeric.h parser_util.h
parser_if.o: testing_ast.h ast.h sru_string.h
parser_let.o: native_proc.h proc.h basic_object.h logging.h symbol.h
parser_let.o: object_container.h constants.h library.h numeric.h
parser_let.o: parser_util.h testing_ast.h ast.h sru_string.h
parser_method_call.o: native_proc.h proc.h basic_object.h logging.h symbol.h
parser_method_call.o: object_container.h constants.h library.h numeric.h
parser_method_call.o: parser_util.h testing_ast.h ast.h sru_string.h
parser_program.o: native_proc.h proc.h basic_object.h logging.h symbol.h
parser_program.o: object_container.h constants.h library.h numeric.h
parser_program.o: parser_util.h testing_ast.h ast.h sru_string.h
parser_spacing.o: native_proc.h proc.h basic_object.h logging.h symbol.h
parser_spacing.o: object_container.h sru_string.h numeric.h constants.h
parser_spacing.o: library.h parser_util.h
parser_statements.o: native_proc.h proc.h basic_object.h logging.h symbol.h
parser_statements.o: object_container.h constants.h library.h numeric.h
parser_statements.o: parser_util.h testing_ast.h ast.h sru_string.h
parser_test.o: parser.h basic_object.h logging.h symbol.h testing.h
parser_test.o: interpreter.h object_container.h numeric.h library.h
parser_test.o: constants.h sru_string.h
parser_util.o: basic_object.h logging.h symbol.h constants.h library.h
parser_util.o: sru_string.h numeric.h testing_ast.h ast.h object_container.h
parser_while.o: native_proc.h proc.h basic_object.h logging.h symbol.h
parser_while.o: object_container.h constants.h library.h numeric.h
parser_while.o: parser_util.h testing_ast.h ast.h sru_string.h
proc.o: proc.h basic_object.h logging.h symbol.h object_container.h ast.h
proc.o: binding.h interpreter.h library.h stack_frame.h constants.h class.h
proc.o: native_proc.h sru_string.h numeric.h utils.h testing_ast.h
proc.o: testing_sru.h
proc_test.o: testing.h proc.h basic_object.h logging.h symbol.h
proc_test.o: object_container.h ast.h interpreter.h testing_ast.h
proc_test.o: sru_string.h library.h numeric.h constants.h
require.o: require.h native_proc.h proc.h basic_object.h
require.o: logging.h symbol.h object_container.h library.h sru_string.h
require.o: interpreter.h stack_frame.h constants.h
sru_regression_test.o: testing.h logging.h basic_object.h symbol.h
sru_regression_test.o: interpreter.h object_container.h
sru_string.o: sru_string.h basic_object.h logging.h symbol.h library.h
sru_string.o: class.h
sru_string_test.o: testing.h sru_string.h basic_object.h logging.h symbol.h
sru_string_test.o: library.h constants.h
stack_frame.o: stack_frame.h basic_object.h logging.h symbol.h
stack_frame.o: object_container.h sru_string.h ast.h proc.h library.h
stack_frame.o: binding.h constants.h utils.h
stack_frame_test.o: testing.h stack_frame.h basic_object.h logging.h symbol.h
stack_frame_test.o: object_container.h ast.h binding.h
symbol.o: symbol.h logging.h
symbol_test.o: symbol.h testing.h
testing.o: testing.h logging.h
utils.o: utils.h logging.h library.h basic_object.h symbol.h
