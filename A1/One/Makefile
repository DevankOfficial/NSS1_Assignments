CXX := g++

CXXFLAGS := -std=c++11 -Wall -lacl

SRCS := getacl.cc setacl.cc checker.cc

EXECS := getacl setacl checker

all : $(EXECS)

# To build all executables
%: %.cc
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(EXECS)

.PHONY: all clean