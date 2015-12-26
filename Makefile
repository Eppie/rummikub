
all:
	g++ -std=c++14 -g -Ofast main.cpp -o rummikub

test:
	g++ -std=c++14 -g -Ofast test.cpp -o test

clean:
	rm -f rummikub test
