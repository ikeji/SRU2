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
load.o \
logging.o \
native_proc.o \
nil.o \
numeric.o \
object.o \
object_container.o \
object_pool.o \
p.o \
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
sru_math.o \
sru_string.o \
sru_sys.o \
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

GENFILES=testing_ast.h testing_sru.h parser.cc constants.cc constants.h

include Makefile.in

all: sru tags library

test: all sru_test
	LD_LIBRARY_PATH=. ./sru_test

libsru.$(LIBEXT): $(OBJS)
	$(CXX) $(LDFLAGS) -shared -o libsru.$(LIBEXT) $(OBJS)

sru: libsru.$(LIBEXT) main.o
	$(CXX) $(LDFLAGS) -g -o sru main.o libsru.$(LIBEXT)

sru_test: libsru.$(LIBEXT) testing.o $(TESTS)
	$(CXX) $(LDFLAGS) -g -o sru_test $(TESTS) testing.o libsru.$(LIBEXT)

clean:
	$(MAKE) -C parser_generator clean
	$(MAKE) -C lib clean
	rm -f *.o *.so *.dll *.core sru sru_test *~ .gitignore~ \
		Makefile.bak tags symbols $(GENFILES)

depend: parser $(GENFILES)
	$(MAKE) -C parser_generator depend
	$(CXX) -MM *.cc > makefile.depend

tags:
	$(CTAGS) -R --c++-kinds=+p --fields=+iaS --extra=+q .

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

library: libsru.$(LIBEXT)
	$(MAKE) -C lib

include makefile.depend
