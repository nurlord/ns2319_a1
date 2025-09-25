#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

std::string loadShaderSource(const char *filepath) {
  std::ifstream file(filepath);
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

GLuint createShader(const char *path, GLenum type) {
  std::string src = loadShaderSource(path);
  const char *src_c = src.c_str();
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &src_c, nullptr);
  glCompileShader(shader);
  int success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    std::cerr << "Shader compile error: " << infoLog << std::endl;
  }
  return shader;
}

GLuint createProgram(const char *vShader, const char *fShader) {
  GLuint vertex = createShader(vShader, GL_VERTEX_SHADER);
  GLuint fragment = createShader(fShader, GL_FRAGMENT_SHADER);
  GLuint program = glCreateProgram();
  glAttachShader(program, vertex);
  glAttachShader(program, fragment);
  glLinkProgram(program);
  int success;
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetProgramInfoLog(program, 512, NULL, infoLog);
    std::cerr << "Program link error: " << infoLog << std::endl;
  }
  glDeleteShader(vertex);
  glDeleteShader(fragment);
  return program;
}

// Utility: create VAO for vertices
GLuint createVAO(const std::vector<float> &vertices) {
  GLuint VAO, VBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
               vertices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
  return VAO;
}

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(500, 500, "Task 2 Shapes", NULL, NULL);
  if (!window) {
    std::cerr << "Failed to create window\n";
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to init GLAD\n";
    return -1;
  }

  GLuint program = createProgram("shaders/vertex_shader.glsl",
                                 "shaders/fragment_shader.glsl");
  glUseProgram(program);

  // --- Circle (GL_TRIANGLE_FAN) ---
  std::vector<float> circle;
  circle.push_back(0.0f);
  circle.push_back(0.0f); // center
  circle.push_back(0.0f);
  circle.push_back(0.0f);
  circle.push_back(0.0f);
  int segments = 100;
  for (int i = 0; i <= segments; i++) {
    float angle = i * 2.0f * M_PI / segments;
    float x = 0.3f * cos(angle) - 0.5f;
    float y = 0.3f * sin(angle) + 0.5f;
    float r = (cos(angle) + 1.0f) / 2.0f; // vary red
    circle.push_back(x);
    circle.push_back(y);
    circle.push_back(r);
    circle.push_back(0.0f);
    circle.push_back(1.0f - r);
  }
  // GLuint circleVAO = createVAO(circle);
  // int circleVertices = (segments + 2);

  // --- Ellipse ---
  std::vector<float> ellipse;
  ellipse.push_back(0.0f);
  ellipse.push_back(0.0f);
  ellipse.push_back(0.0f);
  ellipse.push_back(0.0f);
  ellipse.push_back(0.0f);
  for (int i = 0; i <= segments; i++) {
    float angle = i * 2.0f * M_PI / segments;
    float x = 0.3f * cos(angle) + 0.5f;
    float y = 0.18f * sin(angle) - 0.5f; // y scaled to 60%
    ellipse.push_back(x);
    ellipse.push_back(y);
    ellipse.push_back(0.0f);
    ellipse.push_back((sin(angle) + 1.0f) / 2.0f);
    ellipse.push_back(1.0f);
  }
  // GLuint ellipseVAO = createVAO(ellipse);
  // int ellipseVertices = (segments + 2);

  // --- Squares (nested, gradient) ---
  std::vector<float> squares;
  int numSquares = 5;
  for (int s = 0; s < numSquares; s++) {
    float scale = 0.4f - s * 0.03f;
    float shade = (float)s / numSquares;
    std::vector<std::pair<float, float>> points = {
        {scale, scale}, {-scale, scale}, {-scale, -scale}, {scale, -scale}};
    for (auto &p : points) {
      squares.push_back(p.first);
      squares.push_back(p.second);
      squares.push_back(shade);
      squares.push_back(shade);
      squares.push_back(shade);
    }
  }
  GLuint squaresVAO = createVAO(squares);
  // int squaresVertices = numSquares * 4;

  std::vector<float> triangle = {0.0f,  -0.2f, 1.0f, 0.0f, 0.0f,
                                 -0.3f, -0.7f, 0.0f, 1.0f, 0.0f,
                                 0.3f,  -0.7f, 0.0f, 0.0f, 1.0f};
  GLuint triVAO = createVAO(triangle);

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);

    // // Circle
    // glBindVertexArray(circleVAO);
    // glDrawArrays(GL_TRIANGLE_FAN, 0, circleVertices);

    // // Ellipse
    // glBindVertexArray(ellipseVAO);
    // glDrawArrays(GL_TRIANGLE_FAN, 0, ellipseVertices);

    // Squares
    glBindVertexArray(squaresVAO);
    for (int s = 0; s < numSquares; s++) {
      glDrawArrays(GL_LINE_LOOP, s * 4, 5);
    }

    // Triangle
    glBindVertexArray(triVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
