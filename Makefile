CXX=g++
CXXFLAGS=-march=native -mtune=native -std=c++14 -g -Ofast

# Performance tracing flag
TRACE ?= 0
ifeq ($(TRACE), 1)
	CXXFLAGS += -DENABLE_PERFORMANCE_TRACING
endif

all:
	$(CXX) $(CXXFLAGS) main.cpp -o rummikub

test: test.o Tile.o
	$(CXX) $(CXXFLAGS) test.o Tile.o -o test

test.o: test.cpp Board.hpp Tile.hpp utilities.hpp groups.hpp runs.hpp PerformanceTracer.hpp GameTypes.hpp SetFinder.hpp
	$(CXX) $(CXXFLAGS) -c test.cpp -o test.o

Tile.o: Tile.cpp Tile.hpp color.h
	$(CXX) $(CXXFLAGS) -c Tile.cpp -o Tile.o

client:
	$(CXX) $(CXXFLAGS) client.cpp -o client

server:
	$(CXX) $(CXXFLAGS) server.cpp -o server

clean:
	rm -f rummikub test client server test.o Tile.o
