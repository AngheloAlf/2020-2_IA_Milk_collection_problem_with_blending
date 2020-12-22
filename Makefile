CC = clang++
FLAGS = -std=c++17 -Wall -Wextra -Wshadow
EXECUTABLE = MCPwB.elf

FLAGS_OPT = -Os
FLAGS_DEBUG = -g -Og

FLAGS_OTHER = $(FLAGS_OPT)

all: main.o instance.o milk_type.o node.o truck.o
	$(CC) $(FLAGS) $(FLAGS_OTHER) obj/*.o -o $(EXECUTABLE)


main.o: src/main.cpp src/instance.hpp
	$(CC) $(FLAGS) $(FLAGS_OTHER) src/main.cpp -c -o obj/main.o

instance.o: src/instance.cpp src/instance.hpp src/milk_type.hpp src/node.hpp src/node.hpp src/truck.hpp
	$(CC) $(FLAGS) $(FLAGS_OTHER) src/instance.cpp -c -o obj/instance.o

milk_type.o: src/milk_type.cpp src/milk_type.hpp
	$(CC) $(FLAGS) $(FLAGS_OTHER) src/milk_type.cpp -c -o obj/milk_type.o

node.o: src/node.cpp src/node.hpp
	$(CC) $(FLAGS) $(FLAGS_OTHER) src/node.cpp -c -o obj/node.o

truck.o: src/truck.cpp src/truck.hpp
	$(CC) $(FLAGS) $(FLAGS_OTHER) src/truck.cpp -c -o obj/truck.o


clean:
	rm -f $(EXECUTABLE) obj/*.o
