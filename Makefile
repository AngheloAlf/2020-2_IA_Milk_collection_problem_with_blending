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


obj/main.o: src/main.cpp src/instance.hpp src/milk_type.hpp src/node.hpp src/truck.hpp src/route.hpp src/utils.hpp
	$(CC) $(FLAGS) $(FLAGS_OTHER) src/main.cpp -c -o obj/main.o

obj/instance.o: src/instance.cpp src/instance.hpp src/milk_type.hpp src/node.hpp src/truck.hpp src/route.hpp src/utils.hpp
	$(CC) $(FLAGS) $(FLAGS_OTHER) src/instance.cpp -c -o obj/instance.o

obj/milk_type.o: src/milk_type.cpp src/milk_type.hpp
	$(CC) $(FLAGS) $(FLAGS_OTHER) src/milk_type.cpp -c -o obj/milk_type.o

obj/node.o: src/node.cpp src/node.hpp
	$(CC) $(FLAGS) $(FLAGS_OTHER) src/node.cpp -c -o obj/node.o

obj/truck.o: src/truck.cpp src/truck.hpp
	$(CC) $(FLAGS) $(FLAGS_OTHER) src/truck.cpp -c -o obj/truck.o

obj/route.o: src/route.cpp src/route.hpp
	$(CC) $(FLAGS) $(FLAGS_OTHER) src/route.cpp -c -o obj/route.o

obj/utils.o: src/utils.cpp src/utils.hpp
	$(CC) $(FLAGS) $(FLAGS_OTHER) src/utils.cpp -c -o obj/utils.o


clean:
	rm -f $(EXECUTABLE) obj/*.o


TIDY_CHECKS_INCLUDED = readability-*,clang-diagnostic-*,clang-analyzer-*,bugprone*,modernize*,performance*,portability*,diagnostic-*,analyzer-*,misc*,hicpp-special-member-functions
TIDY_CHECKS_EXCLUDED = ,-modernize-use-trailing-return-type,-clang-analyzer-valist.Uninitialized,-readability-braces-around-statements,-readability-isolate-declaration

tidy:
	clang-tidy -header-filter=.* -checks=$(TIDY_CHECKS_INCLUDED)$(TIDY_CHECKS_EXCLUDED) src/*.cpp -- $(FLAGS) $(FLAGS_OTHER)
