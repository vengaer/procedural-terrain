

SRC = $(wildcard src/*.cc) \
	  $(wildcard src/engine/*.cc) \
	  $(wildcard src/geometry/*.cc) \
	  $(wildcard src/math/*.cc) \
	  $(wildcard src/utils/*.cc)
OBJ := $(addsuffix .o,$(basename $(SRC)))

INC = -I src/ -I src/engine/ -I src/geometry/ -I src/math/ -I src/utils

CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -Weffc++ $(INC) 
LDFLAGS = -lGLEW -lglfw -lGL -lm -lX11 -lpthread -ldl

terrain: $(OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS) 

.PHONY: clean run
clean:
	rm -f $(OBJ) terrain

run: terrain
	./terrain
