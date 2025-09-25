#include <glad/glad.h>
#include <GLFW/glfw3.h>
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

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(500, 500, "Red Triangle", NULL, NULL);
  if (!window) {
    std::cerr << "Failed to create window" << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwMakeContextCurrent(window);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  float vertices[] = {
      -0.5f, 0.5f,  0.0f, 0.0f, 1.0f, // top left
      0.5f,  0.5f,  0.0f, 0.0f, 1.0f, // top right
      0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, // bottom right

      -0.5f, 0.5f,  0.0f, 0.0f, 1.0f, // top left
      0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, // bottom right
      -0.5f, -0.5f, 0.0f, 0.0f, 1.0f  // bottom left
  };

  GLuint VAO, VBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  GLuint program = createProgram("shaders/vertex_shader.glsl",
                                 "shaders/fragment_shader.glsl");
  glUseProgram(program);

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glfwTerminate();
  return 0;
}
