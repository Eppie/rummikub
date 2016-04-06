CXX=g++
CXXFLAGS=-march=native -mtune=native -std=c++14 -g -Ofast

all:
	$(CXX) $(CXXFLAGS) main.cpp -o rummikub

test:
	$(CXX) $(CXXFLAGS) test.cpp -o test

client:
	$(CXX) $(CXXFLAGS) client.cpp -o client

server:
	$(CXX) $(CXXFLAGS) server.cpp -o server

clean:
	rm -f rummikub test client server

