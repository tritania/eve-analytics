OBJS = $ $(shell find ./ -type f -name "*.cpp")
CXX = clang++

COMPILER_FLAGS = -std=c++11 -g -O3 -Wall

LINKER_FLAGS = -lsqlite3 -lcurl -lboost_iostreams -lz 

OUTOBJ = evea

all:
	$ $(CXX) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OUTOBJ)

clean:
	rm evea
