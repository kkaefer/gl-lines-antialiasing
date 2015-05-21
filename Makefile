CXX := $(CXX)
CXXFLAGS := $(CXXFLAGS)
LDFLAGS := $(LDFLAGS)

WARNING_FLAGS := -Wall -Wextra -pedantic -Wsign-compare -Wsign-conversion -Wshadow -Wunused-parameter
COMMON_FLAGS := -fvisibility-inlines-hidden -std=c++11 $(WARNING_FLAGS)

INCLUDE_FLAGS += $(shell ~/.mason/mason cflags glfw 3.1)
INCLUDE_FLAGS += $(shell pkg-config libpng --cflags)
LINK_FLAGS += $(shell ~/.mason/mason static_libs glfw 3.1)
LINK_FLAGS += $(shell ~/.mason/mason ldflags glfw 3.1)
LINK_FLAGS += $(shell pkg-config libpng --libs)

test: src/main.o src/mat4.o src/png.o
	$(CXX) $(LINK_FLAGS) $(LDFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(INCLUDE_FLAGS) $(COMMON_FLAGS) $< -o $@

.PHONY: clean
clean:
	-rm test
	-rm src/*.o