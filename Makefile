SRC = $(wildcard src/*.cc) \
	  $(wildcard src/engine/*.cc) \
	  $(wildcard src/geometry/*.cc) \
	  $(wildcard src/math/*.cc) \
	  $(wildcard src/utils/*.cc)
OBJ := $(addsuffix .o,$(basename $(SRC)))

INC = -I src/ -I src/engine/ -I src/geometry/ -I src/math/ -I src/utils/

export DFLAGS

CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -Weffc++ $(INC) $(DFLAGS) 
LDFLAGS = -lGLEW -lglfw -lGL -lm -lX11 -lpthread -ldl -lstdc++fs


terrain: $(OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS) 

.PHONY: clean run debug
clean:
	rm -f $(OBJ) terrain; rm -rf logs/

run: terrain
	./terrain

debug: DFLAGS=-D LOG_FULL_VERBOSE
debug: terrain
	./terrain
