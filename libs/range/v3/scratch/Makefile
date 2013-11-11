CXX="C:/cygwin/usr/local/clang-trunk/bin/clang++.exe"
CFLAGS=-c -O0
#CXXFLAGS=-Xclang -std=gnu++1y -I../../../.. -I/cygdrive/c/Users/eric/Code/boost/org/trunk
CXXFLAGS=--std=gnu++11 -I../../../.. -I/cygdrive/c/Users/eric/Code/boost/org/trunk
LDFLAGS=

all: range.cpp range.exe

range.exe: range.o
	$(CXX) $(LDFLAGS) range.o -o $@

.cpp.o:
	$(CXX) $(CFLAGS) $(CXXFLAGS) $< -o $@

clean:
	rm -f range.o range.exe
