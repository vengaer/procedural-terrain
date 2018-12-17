
CXXFLAGS = -Wall -Wextra -pedantic -Weffc++

src = $(wildcard src/*.cpp) \
	  $(wildcard src/*.cc)
obj := $(addsuffix .o,$(basename $(src)))

LDFLAGS = -lGLEW -lglfw3 -lGL -lm -lX11 -lpthread -ldl

terrain: $(obj)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

.PHONY: clean run
clean:
	rm -f $(obj) terrain

run: terrain
	./terrain
