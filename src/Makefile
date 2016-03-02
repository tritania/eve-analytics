OBJS = $ $(shell find ./ -type f -name "*.cpp")
CXX = clang++

COMPILER_FLAGS = -std=c++11 -g -Wall

LINKER_FLAGS = -lsqlite3 -lcurl

OUTOBJ = evea

all:
	$ $(CXX) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OUTOBJ)

clean:
	rm data.db evea
