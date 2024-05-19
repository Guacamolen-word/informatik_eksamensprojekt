
CC = clang++

SRC = $(wildcard ./src/**/*.cpp ./src/*.cpp) main.cpp
LIB = -I./lib/ $(shell mariadb_config --cflags) $(shell pkg-config --cflags gtk+-3.0) -I/usr/include/gtk-3.0/
OBJ = obj

CFLAGS = -Wall -Wextra -std=c++20
DEBUG_FLAGS = -g
RELEASE_FLAGS = -O2

LD_FLAGS = -rdynamic -lpthread -lm -luuid -lcrypto -lssl $(shell mariadb_config --libs) $(shell pkg-config --libs gtk+-3.0)
LD_FLAGS_WINDOWS = -mwindows -Wl,--export-all-symbols -lole32 -lpthread -lws2_32 -lm -luuid -lcrypto -lssl $(shell mariadb_config --libs) $(shell pkg-config --libs gtk+-3.0)

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
	./server --gui

gdb:
	gdb ./server --gui

clean:
	rm -f $(OBJ)/*.o

