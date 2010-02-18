GENFILE = parser.cc
SRCS    = \
cpp_code_builder.rb \
cpp_helper_builder.rb \
main.rb \
parser_builder.rb \
ppsample.rb \
printer.rb \
yapp.rb \


all: $(GENFILE)

parser.cc: $(SRCS)
	ruby main.rb -c ppsample.rb > parser.cc

depend: all

clean:
	rm -f *~ .gitignore~ Makefile.bak tags $(GENFILE)

print:
	ruby main.rb -p ppsample.rb

helper:
	ruby main.rb -h ppsample.rb
