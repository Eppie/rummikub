all:
	g++ -march=native -mtune=native -std=c++14 -g -Ofast main.cpp -o rummikub

test:
	g++ -march=native -mtune=native -std=c++14 -g -Ofast test.cpp -o test

clean:
	rm -f rummikub test
