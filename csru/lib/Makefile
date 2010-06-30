CXXFLAGS=-Wall -Werror -Weffc++ -I../.. -fPIC
LDFLAGS=-shared

hello.so: hellolib.cc
	$(CXX) $(CXXFLAGS) hellolib.cc -o hello.so -shared

clean:
	rm hello.so
