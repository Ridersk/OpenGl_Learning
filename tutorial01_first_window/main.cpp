#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>

using namespace std;
using namespace glm;

static const int WINDOW_WIDTH = 1024;
static const int WINDOW_HEIGHT = 768;
static const char TITLE[50] = "Tutorial 01";

void addHints()
{
  glfwWindowHint(GLFW_SAMPLES, 4);               // 4x antialiasing
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGl 3.3
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // MacOS compatibility
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

GLFWwindow *openWindow()
{
  GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, TITLE, NULL, NULL);

  if (window == NULL)
  {
    fprintf(stderr, "Failed to open GLFW window");
    glfwTerminate();
    exit(1);
  }

  return window;
}

void createOpenGlContext(GLFWwindow *window)
{
  glfwMakeContextCurrent(window);
  glewExperimental = true;

  if (glewInit() != GLEW_OK)
  {
    fprintf(stderr, "Failed to initialize GLEW\n");
    exit(1);
  }
}

void createWindow(GLFWwindow **window)
{
  addHints();
  *window = openWindow();
  createOpenGlContext(*window);
}

void addInputs(GLFWwindow *window)
{
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
}

void swapBuffers(GLFWwindow *window)
{
  glfwSwapBuffers(window);
  glfwPollEvents();
}

void draw(GLFWwindow *window)
{
}

void run(GLFWwindow *window)
{
  do
  {
    glClear(GL_COLOR_BUFFER_BIT);
    draw(window);
    swapBuffers(window);
  } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0);
}

int main()
{
  GLFWwindow *window;
  glewExperimental = true;

  if (!glfwInit())
  {
    fprintf(stderr, "Failed to initialize GLFW\n");
  }

  createWindow(&window);
  cout << window << endl;
  addInputs(window);
  run(window);
  return 0;
}
