OBJS	= basic_object.o object_pool.o
TESTS   = object_pool_test.result
CPP	= g++

a.out: $(OBJS) main.o
	$(CPP) -g -o $@ $(OBJS) main.o

.cc.o:
	$(CPP) -g -c $<

basic_object.o: basic_object.h object_pool.h
object_pool.o: basic_object.h object_pool.h

clean:
	rm -f *.o a.out *.test *.result

test: $(TESTS)

.SUFFIXES: .test .result
.test.result:
	./$< | tee $@

object_pool_test.test: object_pool_test.o $(OBJS)
	$(CPP) -g -o $@ object_pool_test.o $(OBJS)

