CXX = g++
CXXFLAGS = -Iinclude -Wall -g

LDFLAGS = -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl

PROGRAMS = red_triangle blue_square task2

all: $(PROGRAMS)

red_triangle: src/red_triangle.cpp src/glad.c
	$(CXX) $^ $(CXXFLAGS) $(LDFLAGS) -o $@

blue_square: src/blue_square.cpp src/glad.c
	$(CXX) $^ $(CXXFLAGS) $(LDFLAGS) -o $@

task2: src/task2_picture.cpp src/glad.c
	$(CXX) $^ $(CXXFLAGS) $(LDFLAGS) -o $@

clean:
	rm -f $(PROGRAMS)

run: $(NAME)
	./$(NAME)
