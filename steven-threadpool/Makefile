# compiler + assembler + linker
CXX = g++
CXXFLAGS = -g -Wall -std=c++11

SOURCES = \
	main.cpp \
	threadpool.cpp 
	
HEADERS = $(SOURCES:.cpp=.h)
OBJECTS = $(SOURCES:.cpp=.o)
TARGETS = main


# "all" is name of the default target, running "make" without params would use it
all: main

main: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o main.o -c main.cpp
	$(CXX) $(CXXFLAGS) -o threadpool.o -c threadpool.cpp	
	$(CXX) $(CXXFLAGS) -o main threadpool.o main.o

clean:
	rm -f main *.o
