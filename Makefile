CC = clang++
STD = -std=c++17
DEBUG_SYMBOLS = -g -g3
WARNINGS = -Wall -Wextra -Wshadow -Wpedantic# -Wpadded -Wsign-conversion
WARNINGS_EVERYTHING = $(WARNINGS) -Weverything -Wno-c++98-compat -Wno-exit-time-destructors -Wno-format-nonliteral -Wno-padded -Wno-old-style-cast
FLAGS = -pipe -fno-omit-frame-pointer $(STD) $(DEBUG_SYMBOLS) $(WARNINGS)
# FLAGS = -pipe $(STD) $(WARNINGS_EVERYTHING)
EXECUTABLE = MCPwB.elf

FLAGS_RELEASE = -O2 -DNDEBUG -march=native
FLAGS_DEBUG = -O0

MODE = DEBUG
ifeq ($(MODE), DEBUG)
	FLAGS_OTHER = $(FLAGS_DEBUG)
else
	FLAGS_OTHER = $(FLAGS_RELEASE)
endif


all: obj/main.o obj/solution.o obj/instance.o obj/milk_type.o obj/milk_types_list.o obj/node.o obj/truck.o obj/route.o obj/utils.o
	$(CC) $(FLAGS) $(FLAGS_OTHER) obj/*.o -o $(EXECUTABLE)


obj/main.o: src/main.cpp
	$(CC) $(FLAGS) $(FLAGS_OTHER) -c src/main.cpp -o obj/main.o

obj/solution.o: src/solution.cpp src/solution.hpp
	$(CC) $(FLAGS) $(FLAGS_OTHER) -c src/solution.cpp -o obj/solution.o

obj/instance.o: src/instance.cpp src/instance.hpp
	$(CC) $(FLAGS) $(FLAGS_OTHER) -c src/instance.cpp -o obj/instance.o

obj/milk_type.o: src/milk_type.cpp src/milk_type.hpp
	$(CC) $(FLAGS) $(FLAGS_OTHER) -c src/milk_type.cpp -o obj/milk_type.o

obj/milk_types_list.o: src/milk_types_list.cpp src/milk_types_list.hpp src/milk_type.hpp
	$(CC) $(FLAGS) $(FLAGS_OTHER) -c src/milk_types_list.cpp -o obj/milk_types_list.o

obj/node.o: src/node.cpp src/node.hpp
	$(CC) $(FLAGS) $(FLAGS_OTHER) -c src/node.cpp -o obj/node.o

obj/truck.o: src/truck.cpp src/truck.hpp
	$(CC) $(FLAGS) $(FLAGS_OTHER) -c src/truck.cpp -o obj/truck.o

obj/route.o: src/route.cpp src/route.hpp
	$(CC) $(FLAGS) $(FLAGS_OTHER) -c src/route.cpp -o obj/route.o

obj/utils.o: src/utils.cpp src/utils.hpp
	$(CC) $(FLAGS) $(FLAGS_OTHER) -c -Wno-pedantic src/utils.cpp -o obj/utils.o


clean:
	rm -f $(EXECUTABLE) obj/*.o


TIDY_CHECKS_INCLUDED = readability-*,clang-diagnostic-*,clang-analyzer-*,bugprone*,modernize*,performance*,portability*,diagnostic-*,analyzer-*,misc*,hicpp-special-member-functions
TIDY_CHECKS_EXCLUDED = ,-modernize-use-trailing-return-type,-clang-analyzer-valist.Uninitialized,-readability-braces-around-statements,-readability-isolate-declaration

tidy:
	clang-tidy -header-filter=.* -checks=$(TIDY_CHECKS_INCLUDED)$(TIDY_CHECKS_EXCLUDED) src/*.cpp -- $(FLAGS) $(FLAGS_OTHER)


perf:
	perf record ./test_instances.sh 1000
	perf report

perf-graph:
	perf record --call-graph dwarf ./test_instances.sh 1000
	perf report --call-graph --stdio -G
