OBJS	= basic_object.o object_pool.o
TESTS   = object_pool.result basic_object.result
CPP	= g++

all: $(OBJS) main.o
	$(CPP) -g -o sru $(OBJS) main.o

.cc.o:
	$(CPP) -g -c $<

basic_object.o: basic_object.h object_pool.h
basic_object.test: basic_object_test.o $(OBJS)
	$(CPP) -g -o $@ basic_object_test.o $(OBJS)

object_pool.o: basic_object.h object_pool.h
object_pool.test: object_pool_test.o $(OBJS)
	$(CPP) -g -o $@ object_pool_test.o $(OBJS)

clean:
	rm -f *.o sru *.test *.result *~

test: $(TESTS)

.SUFFIXES: .test .result
.test.result:
	./$< | tee $@

