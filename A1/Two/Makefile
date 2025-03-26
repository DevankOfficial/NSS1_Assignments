CXX := g++

CXXFLAGS := -std=c++11 -Wall

SRCS := sudo.cc cd.cc create_dir.cc fget.cc fput.cc

EXECS := sudo cd create_dir fget fput

all : $(EXECS)

# To build all executables
%: %.cc
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(EXECS)

.PHONY: all clean