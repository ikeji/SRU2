GENFILE = parser.cc

all: $(GENFILE)

parser.cc: ppsample.rb yapp.rb
	ruby main.rb -c ppsample.rb > parser.cc

depend: all
	makedepend *.cc

clean:
	rm -f *~ .gitignore~ Makefile.bak tags $(GENFILE)

pering:
	ruby main.rb -p ppsample.rb

# DO NOT DELETE

parser.o: /usr/include/string.h /usr/include/_ansi.h /usr/include/newlib.h
parser.o: /usr/include/sys/config.h /usr/include/machine/ieeefp.h
parser.o: /usr/include/sys/reent.h /usr/include/sys/_types.h
parser.o: /usr/include/machine/_types.h /usr/include/machine/_default_types.h
parser.o: /usr/include/sys/lock.h /usr/include/sys/string.h
