CXX ?= g++

BIN = terrain

SRC = $(wildcard src/*.cc) \
	  $(wildcard src/engine/*.cc) \
	  $(wildcard src/math/*.cc) \
	  $(wildcard src/geometry/*.cc) \
	  $(wildcard src/utils/*.cc) \
	  $(wildcard src/processing/*.cc)

OBJ := $(addsuffix .o,$(basename $(SRC)))

INC = -I src/ -I src/engine/ -I src/geometry/ -I src/math/ -I src/utils/ -I src/processing/ -I src/environment/ -I assets/include/stb/

export CPPFLAGS

CXXFLAGS := $(CXXFLAGS) -std=c++17 -Wall -Wextra -pedantic -Weffc++ $(INC) 
LDFLAGS = -lGLEW -lglfw -lGL -lm -lX11 -lpthread -ldl -lstdc++fs


$(BIN): $(OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS) 

.PHONY: clean run debug single_thread stats TODO
clean:
	rm -f $(OBJ) $(BIN); rm -rf logs/

run: $(BIN)
	./$(BIN)

debug: CPPFLAGS := $(CPPFLAGS) -D LOG_FULL_VERBOSE
debug: $(BIN)
	./$(BIN)

single_thread : CPPFLAGS := $(CPPFLAGS) -D RESTRICT_THREAD_USAGE
single_thread: $(BIN)
	./$(BIN)

stats:
	find assets/shaders src -type f \( -iname \*.cc -o -iname \*.tcc -o -iname \*.h -o -iname \*.vert -o -iname \*.frag -o -iname \*.glsl \) | xargs wc -l

TODO:
	grep -lr TODO . | grep -vE 'Makefile|git'
