CXXFLAGS=-Wall -Werror -Weffc++ -I../..
LDFLAGS=-shared

hello.so: hellolib.cc
	$(CXX) $(CXXFLAGS) hellolib.cc -o hello.so -shared

clean:
	rm hello.so
