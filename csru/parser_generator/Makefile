GENFILE = parser.cc symbols
SRCS    = \
cpp_code_builder.rb \
cpp_helper_builder.rb \
main.rb \
parser_builder.rb \
ppsample.rb \
printer.rb \
sru_syntax.rb \
yapp.rb \


all: $(GENFILE)

parser.cc: $(SRCS)
	ruby main.rb -c sru_syntax.rb > $@.tmp
	mv $@.tmp $@

symbols: $(SRCS)
	ruby main.rb -s sru_syntax.rb > $@.tmp
	mv $@.tmp $@

depend: all

clean:
	rm -f *~ .gitignore~ Makefile.bak tags $(GENFILE)

print:
	ruby main.rb -p sru_syntax.rb

helper:
	ruby main.rb -h sru_syntax.rb
