#ifndef BUFFER_OFFSET
#define BUFFER_OFFSET(offset) ((GLvoid *)(offset))
#endif
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <string>
#include <glm/glm.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

const glm::vec3 WHITE(1.0, 1.0, 1.0);
const glm::vec3 BLACK(0.0, 0.0, 0.0);
const glm::vec3 RED(1.0, 0.0, 0.0);
const glm::vec3 GREEN(0.0, 1.0, 0.0);
const glm::vec3 BLUE(0.0, 0.0, 1.0);
const int CIRCLE_NUM_POINTS = 100;
const int ELLIPSE_NUM_POINTS = 100;
const int TRIANGLE_NUM_POINTS = 3;
const int SQUARE_NUM = 6;
const int SQUARE_NUM_POINTS = 4 * SQUARE_NUM;
const int LINE_NUM_POINTS = 2;

std::string readFile(const char *filename) {
  std::ifstream in(filename);
  if (!in) {
    std::cerr << "Cannot open " << filename << std::endl;
    exit(EXIT_FAILURE);
  }
  std::stringstream ss;
  ss << in.rdbuf();
  return ss.str();
}

GLuint InitShader(const char *vShaderFile, const char *fShaderFile) {
  std::string vsrc = readFile(vShaderFile);
  std::string fsrc = readFile(fShaderFile);

  const char *vShaderSrc = vsrc.c_str();
  const char *fShaderSrc = fsrc.c_str();

  GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vShader, 1, &vShaderSrc, NULL);
  glCompileShader(vShader);

  GLint compiled;
  glGetShaderiv(vShader, GL_COMPILE_STATUS, &compiled);
  if (!compiled) {
    char log[1024];
    glGetShaderInfoLog(vShader, 1024, NULL, log);
    std::cerr << "Vertex shader compile error:\n" << log << std::endl;
  }

  GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fShader, 1, &fShaderSrc, NULL);
  glCompileShader(fShader);

  glGetShaderiv(fShader, GL_COMPILE_STATUS, &compiled);
  if (!compiled) {
    char log[1024];
    glGetShaderInfoLog(fShader, 1024, NULL, log);
    std::cerr << "Fragment shader compile error:\n" << log << std::endl;
  }

  GLuint program = glCreateProgram();
  glAttachShader(program, vShader);
  glAttachShader(program, fShader);
  glLinkProgram(program);

  GLint linked;
  glGetProgramiv(program, GL_LINK_STATUS, &linked);
  if (!linked) {
    char log[1024];
    glGetProgramInfoLog(program, 1024, NULL, log);
    std::cerr << "Shader program link error:\n" << log << std::endl;
  }

  glDeleteShader(vShader);
  glDeleteShader(fShader);
  return program;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

float generateAngleColor(double angle) { return 1.0 / (2 * M_PI) * angle; }

double getTriangleAngle(int point) { return 2 * M_PI / 3 * point; }

double getSquareAngle(int point) { return M_PI / 4 + (M_PI / 2 * point); }

glm::vec2 getEllipseVertex(glm::vec2 center, double scale, double verticalScale,
                           double angle) {
  glm::vec2 vertex(sin(angle), cos(angle));
  vertex *= scale;
  vertex.y *= verticalScale;
  vertex += center;
  return vertex;
}

void generateEllipsePoints(glm::vec2 vertices[], glm::vec3 colors[],
                           int startVertexIndex, int numPoints,
                           glm::vec2 center, double scale,
                           double verticalScale) {
  double angleIncrement = (2 * M_PI) / numPoints;
  double currentAngle = M_PI / 2;

  for (int i = startVertexIndex; i < startVertexIndex + numPoints; ++i) {
    vertices[i] = getEllipseVertex(center, scale, verticalScale, currentAngle);
    if (verticalScale == 1.0) {
      colors[i] = glm::vec3(generateAngleColor(currentAngle), 0.0, 0.0);
    } else {
      colors[i] = RED;
    }
    currentAngle += angleIncrement;
  }
}

void generateTrianglePoints(glm::vec2 vertices[], glm::vec3 colors[],
                            int startVertexIndex) {
  glm::vec2 scale(0.25, 0.25);
  glm::vec2 center(0.0, 0.70);

  for (int i = 0; i < 3; ++i) {
    double currentAngle = getTriangleAngle(i);
    vertices[startVertexIndex + i] =
        glm::vec2(sin(currentAngle), cos(currentAngle)) * scale + center;
  }

  colors[startVertexIndex] = RED;
  colors[startVertexIndex + 1] = GREEN;
  colors[startVertexIndex + 2] = BLUE;
}

void generateSquarePoints(glm::vec2 vertices[], glm::vec3 colors[],
                          int squareNumber, int startVertexIndex) {
  glm::vec2 scale(0.90, 0.90);
  double scaleDecrease = 0.15;
  glm::vec2 center(0.0, -0.25);
  int vertexIndex = startVertexIndex;

  for (int i = 0; i < squareNumber; ++i) {
    glm::vec3 currentColor;
    currentColor = (i % 2) ? BLACK : WHITE;
    for (int j = 0; j < 4; ++j) {
      double currentAngle = getSquareAngle(j);
      vertices[vertexIndex] =
          glm::vec2(sin(currentAngle), cos(currentAngle)) * scale + center;
      colors[vertexIndex] = currentColor;
      vertexIndex++;
    }
    scale -= scaleDecrease;
  }
}

void generateLinePoints(glm::vec2 vertices[], glm::vec3 colors[],
                        int startVertexIndex) {
  vertices[startVertexIndex] = glm::vec2(-1.0, -1.0);
  vertices[startVertexIndex + 1] = glm::vec2(1.0, 1.0);

  colors[startVertexIndex] = WHITE;
  colors[startVertexIndex + 1] = BLUE;
}

GLuint vao[5], program;

void init() {
  glm::vec2 triangle_vertices[TRIANGLE_NUM_POINTS];
  glm::vec3 triangle_colors[TRIANGLE_NUM_POINTS];

  glm::vec2 square_vertices[SQUARE_NUM_POINTS];
  glm::vec3 square_colors[SQUARE_NUM_POINTS];

  glm::vec2 circle_vertices[CIRCLE_NUM_POINTS];
  glm::vec3 circle_colors[CIRCLE_NUM_POINTS];

  glm::vec2 ellipse_vertices[ELLIPSE_NUM_POINTS];
  glm::vec3 ellipse_colors[ELLIPSE_NUM_POINTS];

  generateTrianglePoints(triangle_vertices, triangle_colors, 0);
  generateSquarePoints(square_vertices, square_colors, SQUARE_NUM, 0);

  glm::vec2 circle_center(0.65, 0.70);
  generateEllipsePoints(circle_vertices, circle_colors, 0, CIRCLE_NUM_POINTS,
                        circle_center, 0.25, 1.0);

  glm::vec2 ellipse_center(-0.65, 0.70);
  generateEllipsePoints(ellipse_vertices, ellipse_colors, 0, ELLIPSE_NUM_POINTS,
                        ellipse_center, 0.25, 0.50);

  std::string vshader, fshader;
  vshader = "shaders/vertex_shader_task2.glsl";
  fshader = "shaders/fragment_shader.glsl";
  program = InitShader(vshader.c_str(), fshader.c_str());
  glUseProgram(program);

  GLuint vbo[2];

  glGenVertexArrays(1, &vao[0]);
  glBindVertexArray(vao[0]);

  glGenBuffers(1, &vbo[0]);
  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices,
               GL_STATIC_DRAW);
  GLuint location = glGetAttribLocation(program, "vPosition");
  glEnableVertexAttribArray(location);
  glVertexAttribPointer(location, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2),
                        BUFFER_OFFSET(0));

  glGenBuffers(1, &vbo[1]);
  glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_colors), triangle_colors,
               GL_STATIC_DRAW);
  GLuint cLocation = glGetAttribLocation(program, "vColor");
  glEnableVertexAttribArray(cLocation);
  glVertexAttribPointer(cLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),
                        BUFFER_OFFSET(0));

  glGenVertexArrays(1, &vao[1]);
  glBindVertexArray(vao[1]);

  glGenBuffers(1, &vbo[0]);
  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(square_vertices), square_vertices,
               GL_STATIC_DRAW);
  location = glGetAttribLocation(program, "vPosition");
  glEnableVertexAttribArray(location);
  glVertexAttribPointer(location, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2),
                        BUFFER_OFFSET(0));

  glGenBuffers(1, &vbo[1]);
  glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(square_colors), square_colors,
               GL_STATIC_DRAW);
  cLocation = glGetAttribLocation(program, "vColor");
  glEnableVertexAttribArray(cLocation);
  glVertexAttribPointer(cLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),
                        BUFFER_OFFSET(0));

  glGenVertexArrays(1, &vao[3]);

  glBindVertexArray(vao[3]);

  glGenBuffers(1, &vbo[0]);

  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(circle_vertices), circle_vertices,
               GL_STATIC_DRAW);
  location = glGetAttribLocation(program, "vPosition");

  glEnableVertexAttribArray(location);

  glVertexAttribPointer(location, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2),
                        BUFFER_OFFSET(0));

  glGenBuffers(1, &vbo[1]);
  glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(circle_colors), circle_colors,
               GL_STATIC_DRAW);
  cLocation = glGetAttribLocation(program, "vColor");
  glEnableVertexAttribArray(cLocation);
  glVertexAttribPointer(cLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),
                        BUFFER_OFFSET(0));

  glGenVertexArrays(1, &vao[4]);
  glBindVertexArray(vao[4]);

  glGenBuffers(1, &vbo[0]);
  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(ellipse_vertices), ellipse_vertices,
               GL_STATIC_DRAW);
  location = glGetAttribLocation(program, "vPosition");
  glEnableVertexAttribArray(location);
  glVertexAttribPointer(location, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2),
                        BUFFER_OFFSET(0));

  glGenBuffers(1, &vbo[1]);
  glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(ellipse_colors), ellipse_colors,
               GL_STATIC_DRAW);
  cLocation = glGetAttribLocation(program, "vColor");
  glEnableVertexAttribArray(cLocation);
  glVertexAttribPointer(cLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),
                        BUFFER_OFFSET(0));

  glClearColor(0.0, 0.0, 0.0, 1.0);
}

void display(void) {

  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(program);

  glBindVertexArray(vao[0]);
  glDrawArrays(GL_TRIANGLES, 0, TRIANGLE_NUM_POINTS);

  glBindVertexArray(vao[1]);
  for (int i = 0; i < SQUARE_NUM; ++i) {
    glDrawArrays(GL_TRIANGLE_FAN, (i * 4), 4);
  }

  glBindVertexArray(vao[2]);
  glDrawArrays(GL_LINES, 0, LINE_NUM_POINTS);

  glBindVertexArray(vao[3]);

  glDrawArrays(GL_TRIANGLE_FAN, 0, CIRCLE_NUM_POINTS);

  glBindVertexArray(vao[4]);
  glDrawArrays(GL_TRIANGLE_FAN, 0, ELLIPSE_NUM_POINTS);

  glFlush();
}

int main(int argc, char **argv) {
  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  GLFWwindow *window = glfwCreateWindow(500, 500, "task2", NULL, NULL);

  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;

    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  init();

  std::cout << "OpenGL Vendor: " << glGetString(GL_VENDOR) << std::endl;
  std::cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << std::endl;
  std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
  std::cout << "Supported GLSL version is: "
            << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

  while (!glfwWindowShouldClose(window)) {
    display();
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  return 0;
}
