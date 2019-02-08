CXX ?= g++
UNAME := $(shell uname -s)

SRC = $(wildcard src/*.cc) \
	  $(wildcard src/engine/*.cc) \
	  $(wildcard src/math/*.cc) \
	  $(wildcard src/geometry/*.cc) \
	  $(wildcard src/utils/*.cc)

OBJ := $(addsuffix .o,$(basename $(SRC)))

INC = -I src/ -I src/engine/ -I src/geometry/ -I src/math/ -I src/utils/

ifeq ($(findstring Linux, $(UNAME)), Linux)
XRUNNING := $(shell echo $(DISPLAY) &>/dev/null; echo $$?)
ifeq ($(XRUNNING), 0)
CPPFLAGS := $(CPPFLAGS) -D X_ENV
endif
endif

export CPPFLAGS

CXXFLAGS := $(CXXFLAGS) -std=c++17 -Wall -Wextra -pedantic -Weffc++ $(INC) 
LDFLAGS = -lGLEW -lglfw -lGL -lm -lX11 -lpthread -ldl -lstdc++fs


terrain: $(OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS) 

.PHONY: clean run debug single_thread stats TODO
clean:
	rm -f $(OBJ) terrain; rm -rf logs/

run: terrain
	./terrain

debug: CPPFLAGS := $(CPPFLAGS) -D LOG_FULL_VERBOSE
debug: terrain
	./terrain

single_thread : CPPFLAGS := $(CPPFLAGS) -D RESTRICT_THREAD_USAGE
single_thread: terrain
	./terrain

stats:
	find ./ -type f \( -iname \*.cc -o -iname \*.tcc -o -iname \*.h -o -iname \*.vert -o -iname \*.frag -o -iname \*.glsl \) | xargs wc -l

TODO:
	grep -lr TODO . | grep -v Makefile
