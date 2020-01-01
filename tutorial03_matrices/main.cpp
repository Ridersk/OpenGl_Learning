#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>

#include "../common/shader.hpp"

using namespace std;
using namespace glm;

static const int WINDOW_WIDTH = 1366;
static const int WINDOW_HEIGHT = 768;
static const char TITLE[50] = "Tutorial 01";

// DEFINES

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

void addInputs(GLFWwindow *window)
{
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
}

void defineVertexArray(GLuint *VertexArrayID)
{
  glGenVertexArrays(1, VertexArrayID);
  glBindVertexArray(*VertexArrayID);
}

void createVertexBuffer(GLuint *vertexBuffer)
{
  static GLfloat g_vertex_buffer_data[] = {
      -1.0f,
      -1.0f,
      0.0f,
      1.0f,
      -1.0f,
      0.0f,
      0.0f,
      1.0f,
      0.0f,
  };

  cout << g_vertex_buffer_data << endl;

  glGenBuffers(1, vertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, *vertexBuffer);
  glBufferData(
      GL_ARRAY_BUFFER,
      sizeof(g_vertex_buffer_data),
      g_vertex_buffer_data, GL_STATIC_DRAW);
}

void createWindow(GLFWwindow **window, GLuint *vertices)
{
  GLuint VertexArrayID; // basis to use vertices
  GLuint vertexBuffer;

  addHints();
  *window = openWindow();
  createOpenGlContext(*window);
  defineVertexArray(&VertexArrayID);
  createVertexBuffer(&vertexBuffer);

  // static, change it late
  vertices[0] = vertexBuffer;
}

// RUN

void swapBuffers(GLFWwindow *window)
{
  glfwSwapBuffers(window);
  glfwPollEvents();
}

void draw(GLFWwindow *window, GLuint *vertices)
{
  // cout<< vertices[0]<<endl;
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vertices[0]);
  glVertexAttribPointer(
      0,        // shader in  MyVertexShader.lma (layout(location = 0))
      3,        // size
      GL_FLOAT, // type
      GL_FALSE, // normalization
      0,        // stride
      (void *)0 // array buffer offset
  );
  glDrawArrays(GL_TRIANGLES, 0, 3);
  glDisableVertexAttribArray(0);
}

void run(GLFWwindow *window, GLuint *vertices, GLuint programID, GLuint MatrixID, mat4 MVP)
{

  // Dark blue background
  glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

  do
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(programID); // use my program
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    draw(window, vertices);
    swapBuffers(window);
  } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0);
}

void loadShaders(GLuint *programID)
{
  *programID = LoadShaders("MyVertexShader.lvet", "MyFragmentShader.lfrag");
}

void setPerspective(GLuint *programID, GLuint *MatrixID, mat4 *mvp) {
  // Get a handle for our "MVP" uniform
  // Only during the initialisation
  *MatrixID = glGetUniformLocation(*programID, "MVP");

  // Screen Projection, 45° Field of View
  mat4 Projection = perspective(radians(45.0f), (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT, 0.1f, 100.0f);

  // View: Camera matrix
  mat4 View = lookAt(
    vec3(4, 3, 3), // Camera position
    vec3(0, 0, 0),
    vec3(0, 1, 0) // Head is up
  );

  // Model matrix
  mat4 Model = mat4(1.0f);

  // ModelViewProjection
  *mvp = Projection * View * Model;
}

int main()
{
  GLFWwindow *window;
  GLuint programID;
  GLuint MatrixId;
  GLuint vertices[5];
  mat4 mvp;

  glewExperimental = true;

  if (!glfwInit())
  {
    fprintf(stderr, "Failed to initialize GLFW\n");
  }

  createWindow(&window, vertices);
  addInputs(window);
  loadShaders(&programID);
  setPerspective(&programID, &MatrixId, &mvp);
  run(window, vertices, programID, MatrixId, mvp);
  return 0;
}
