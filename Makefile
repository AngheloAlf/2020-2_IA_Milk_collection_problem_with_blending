CC = clang++
STD = -std=c++17
WARNINGS = -Wall -Wextra -Wshadow -Wpedantic # -Wsign-conversion -Wpadded
WARNINGS_EVERYTHING = $(WARNINGS) -Weverything -Wno-c++98-compat -Wno-exit-time-destructors -Wno-format-nonliteral -Wno-padded -Wno-old-style-cast
FLAGS = $(STD) $(WARNINGS)
# FLAGS = $(STD) $(WARNINGS_EVERYTHING)
EXECUTABLE = MCPwB.elf

FLAGS_RELEASE = -Os -DNDEBUG
FLAGS_DEBUG = -g -Og

FLAGS_OTHER = $(FLAGS_DEBUG)

all: obj/main.o obj/instance.o obj/milk_type.o obj/node.o obj/truck.o obj/route.o obj/utils.o
	$(CC) $(FLAGS) $(FLAGS_OTHER) obj/*.o -o $(EXECUTABLE)


obj/main.o: src/main.cpp src/instance.hpp obj/utils.o
	$(CC) $(FLAGS) $(FLAGS_OTHER) src/main.cpp -c -o obj/main.o

obj/instance.o: src/instance.cpp src/instance.hpp src/milk_type.hpp obj/milk_type.o src/node.hpp obj/node.o src/truck.hpp obj/truck.o src/route.hpp obj/route.o src/utils.hpp obj/utils.o
	$(CC) $(FLAGS) $(FLAGS_OTHER) src/instance.cpp -c -o obj/instance.o

obj/milk_type.o: src/milk_type.cpp src/milk_type.hpp obj/utils.o
	$(CC) $(FLAGS) $(FLAGS_OTHER) src/milk_type.cpp -c -o obj/milk_type.o

obj/node.o: src/node.cpp src/node.hpp obj/utils.o
	$(CC) $(FLAGS) $(FLAGS_OTHER) src/node.cpp -c -o obj/node.o

obj/truck.o: src/truck.cpp src/truck.hpp obj/utils.o
	$(CC) $(FLAGS) $(FLAGS_OTHER) src/truck.cpp -c -o obj/truck.o

obj/route.o: src/route.cpp src/route.hpp obj/utils.o
	$(CC) $(FLAGS) $(FLAGS_OTHER) src/route.cpp -c -o obj/route.o

obj/utils.o: src/utils.cpp src/utils.hpp
	$(CC) $(FLAGS) $(FLAGS_OTHER) src/utils.cpp -c -o obj/utils.o


clean:
	rm -f $(EXECUTABLE) obj/*.o

tidy:
	clang-tidy -header-filter=.* -checks=-*,readability-*,clang-diagnostic-*,clang-analyzer-*,bugprone*,modernize*,performance*,-modernize-pass-by-value,-modernize-use-auto,-modernize-use-using,-modernize-use-trailing-return-type,-clang-analyzer-valist.Uninitialized,-readability-braces-around-statements src/*.cpp -- $(FLAGS) $(FLAGS_OTHER)
