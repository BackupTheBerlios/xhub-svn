CFLAGS= -O3 -Wall
CPP= g++

preklad: main.cpp
	$(CPP) $(CFLAGS) -oserver main.cpp

clean:
	rm -f server
