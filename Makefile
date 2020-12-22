CC = clang++
FLAGS = -std=c++17 -Wall -Wextra -Wshadow
EXECUTABLE = MCPwB.elf

FLAGS_OPT = -Os
FLAGS_DEBUG = -g -Og

FLAGS_OTHER = $(FLAGS_OPT)

all: main.o
	$(CC) $(FLAGS) $(FLAGS_OTHER) obj/*.o -o $(EXECUTABLE)


main.o: src/main.cpp
	$(CC) $(FLAGS) $(FLAGS_OTHER) src/main.cpp -c -o obj/main.o


clean:
	rm -f $(EXECUTABLE) obj/*.o
