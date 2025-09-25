# Compiler
CXX = g++
CXXFLAGS = -Iinclude -Wall -g

# Libraries
LDFLAGS = -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl

# Programs
PROGRAMS = red_triangle blue_square task2

# Default rule: build everything
all: $(PROGRAMS)

red_triangle: src/red_triangle.cpp src/glad.c
	$(CXX) $^ $(CXXFLAGS) $(LDFLAGS) -o $@

blue_square: src/blue_square.cpp src/glad.c
	$(CXX) $^ $(CXXFLAGS) $(LDFLAGS) -o $@

task2: src/task2_picture.cpp src/glad.c
	$(CXX) $^ $(CXXFLAGS) $(LDFLAGS) -o $@

# Clean build files
clean:
	rm -f $(PROGRAMS)

# Run a specific program: make run NAME=red_triangle
run: $(NAME)
	./$(NAME)
