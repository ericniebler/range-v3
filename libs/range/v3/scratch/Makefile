CXX="C:/cygwin/usr/local/clang-3.3/bin/clang++.exe"
CFLAGS=-c -O0
#CXXFLAGS=-Xclang -std=gnu++1y -I../../../../include
CXXFLAGS=--std=gnu++11 -I../../../../include -I/cygdrive/c/Users/eric/Code/modular-boost
LDFLAGS=

all: range.cpp range.exe

range.exe: range.o
	$(CXX) $(LDFLAGS) range.o -o $@

.cpp.o:
	$(CXX) $(CFLAGS) $(CXXFLAGS) $< -o $@

clean:
	rm -f range.o range.exe
