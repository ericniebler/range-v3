CXX="C:/cygwin/usr/local/clang-trunk/bin/clang++.exe"
#CXX="C:/cygwin/usr/local/clang-3.4/bin/clang++.exe"
CFLAGS=-c -O0 -g
#CXXFLAGS=-Xclang -std=gnu++1y -I../../../../include
#CXXFLAGS=--std=gnu++11 -I../../../../include -I/cygdrive/c/Users/eric/Code/modular-boost -ftemplate-backtrace-limit=0
CXXFLAGS=--std=gnu++11 -I../../../../include -ftemplate-backtrace-limit=0
LDFLAGS=

all: range.cpp range.exe

range.exe: range.o
	$(CXX) $(LDFLAGS) range.o -o $@

.cpp.o:
	$(CXX) $(CFLAGS) $(CXXFLAGS) $< -o $@

clean:
	rm -f range.o range.exe
