
CC = clang++

SRC = $(wildcard ./src/**/*.cpp ./src/*.cpp) main.cpp
LIB = -I./lib/ $(shell mariadb_config --cflags)
OBJ = obj

CFLAGS = -Wall -Wextra -std=c++20
DEBUG_FLAGS = -g
RELEASE_FLAGS = -O2

LD_FLAGS = -lpthread -lm -luuid -lcrypto -lssl $(shell mariadb_config --libs)
LD_FLAGS_WINDOWS = -lole32 -lpthread -lws2_32 -lm -luuid -lcrypto -lssl $(shell mariadb_config --libs)

debug:
	$(CC) -c $(SRC) $(CFLAGS) $(DEBUG_FLAGS) $(LIB)
	mv *.o $(OBJ)

release:
	$(CC) -c $(SRC) $(CFLAGS) $(RELEASE_FLAGS) $(LIB)
	mv *.o $(OBJ)

build:
	$(CC) $(OBJ)/*.o $(LD_FLAGS) -o server

build_windows:
	$(CC) -std=c++20 $(OBJ)/*.o $(LD_FLAGS_WINDOWS) -o server

run:
	./server

gdb:
	gdb ./server

clean:
	rm -f $(OBJ)/*.o

