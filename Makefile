

src = $(wildcard src/*.cc) \
	  $(wildcard src/*.cpp) \
	  $(wildcard src/engine/*.cc)
obj := $(addsuffix .o,$(basename $(src)))

inc = -I src/ -I src/engine/

CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -Weffc++ $(inc)
LDFLAGS = -lGLEW -lglfw3 -lGL -lm -lX11 -lpthread -ldl

terrain: $(obj)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS) $(inc)

.PHONY: clean run
clean:
	rm -f $(obj) terrain

run: terrain
	./terrain
