#include <stdio.h>
#include <stdlib.h>
#include <tuple>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>

#include <glm/gtc/matrix_transform.hpp>

#include "../common/shader.hpp"

using namespace std;
using namespace glm;

static const int WINDOW_WIDTH = 1366;
static const int WINDOW_HEIGHT = 768;
static const char TITLE[50] = "Tutorial 04 - Exercise";

void addHints();
GLFWwindow *openWindow();
GLFWwindow *openWindow();
void createOpenGlContext(GLFWwindow *window);
void createWindow(GLFWwindow **window, GLuint *vertexArrayId);
void addInputs(GLFWwindow *window);
void defineVertexArray(GLuint *VertexArrayID);
void createObjects(GLuint *programID, vector<GLuint> *mvpIds, vector<mat4> *mvps, vector<GLuint> *objectIds, vector<GLuint> *objectColorIds, vector<int> *qttObjectsFragments);
GLuint createObjectBuffer(vector<GLfloat> bufferData);
void loadShaders(GLuint *programID);
void extendVector(vector<GLfloat> *objectVertices, GLfloat *object, int qttPoints);
void swapBuffers(GLFWwindow *window);
void run(GLFWwindow *window, GLuint programID, vector<GLuint> mvpIds, vector<mat4> mvps, vector<GLuint> objectIds, vector<GLuint> objectColorIds, vector<int> qttObjectsFragments);
void draw(GLuint programID, vector<GLuint> mvpIds, vector<mat4> mvps, vector<GLuint> objectIds, vector<GLuint> objectColorIds, vector<int> qttObjectsFragments);
void drawObject(GLuint mvpId, mat4 mvp, GLuint objectId, GLuint objectColorId, int qttObjectFragments, GLint posAttrShaderObject, GLint posAttrShaderColor);
GLuint createCube(vector<int> *qttObjectsFragments);
GLuint createCubeColor();
void addCubePerspective(GLuint *programID, vector<GLuint> *mvpIds, vector<mat4> *mvps);
GLuint createPyramid(vector<int> *qttObjectsFragments);
GLuint createPyramidColor();
void addPyramidPerspective(GLuint *programID, vector<GLuint> *mvpIds, vector<mat4> *mvps);

int main()
{
  GLFWwindow *window;
  GLuint programID;
  GLuint vertexArrayID; // basis to use vertices (points of objects)
  vector<GLuint> mvpIds;
  vector<mat4> mvps;
  vector<GLuint> objectIds;
  vector<GLuint> objectColorIds;
  vector<int> qttObjectsFragments;

  createWindow(&window, &vertexArrayID);
  addInputs(window);
  loadShaders(&programID);
  createObjects(&programID, &mvpIds, &mvps, &objectIds, &objectColorIds, &qttObjectsFragments);
  run(window, programID, mvpIds, mvps, objectIds, objectColorIds, qttObjectsFragments);
  return 0;
}

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

void createWindow(GLFWwindow **window, GLuint *vertexArrayId)
{
  glewExperimental = true;

  if (!glfwInit())
  {
    fprintf(stderr, "Failed to initialize GLFW\n");
  }

  addHints();
  *window = openWindow();
  createOpenGlContext(*window);
  defineVertexArray(vertexArrayId);
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

void loadShaders(GLuint *programID)
{
  *programID = LoadShaders("TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader");
}

void createObjects(GLuint *programID, vector<GLuint> *mvpIds, vector<mat4> *mvps, vector<GLuint> *objectIds,
                   vector<GLuint> *objectColorIds, vector<int> *qttObjectsFragments)
{
  // Cube
  objectIds->push_back(createCube(qttObjectsFragments));
  objectColorIds->push_back(createCubeColor());
  addCubePerspective(programID, mvpIds, mvps);

  // Pyramid
  objectIds->push_back(createPyramid(qttObjectsFragments));
  objectColorIds->push_back(createPyramidColor());
  addPyramidPerspective(programID, mvpIds, mvps);
}

GLuint createObjectBuffer(vector<GLfloat> bufferData)
{
  GLuint bufferId;
  glGenBuffers(1, &bufferId);
  glBindBuffer(GL_ARRAY_BUFFER, bufferId);
  glBufferData(
      GL_ARRAY_BUFFER,
      bufferData.size() * sizeof(GLfloat),
      static_cast<void *>(bufferData.data()), GL_STATIC_DRAW);
  return bufferId;
}

void extendVector(vector<GLfloat> *objectVertices, GLfloat *object, int qttPoints)
{
  int dimensions = 3;
  for (int i = 0; i < dimensions * qttPoints; i++)
  {
    objectVertices->push_back(object[i]);
  }
}

void swapBuffers(GLFWwindow *window)
{
  glfwSwapBuffers(window);
  glfwPollEvents();
}

void run(GLFWwindow *window, GLuint programID, vector<GLuint> mvpIds, vector<mat4> mvps,
         vector<GLuint> objectIds, vector<GLuint> objectColorIds, vector<int> qttObjectsFragments)
{

  // Dark blue background
  glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

  // Enable depth test
  glEnable(GL_DEPTH_TEST);
  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS);

  do
  {
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(programID); // use my program
    draw(programID, mvpIds, mvps, objectIds, objectColorIds, qttObjectsFragments);
    swapBuffers(window);
  } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0);
}

void draw(GLuint programID, vector<GLuint> mvpIds, vector<mat4> mvps, vector<GLuint> objectIds,
          vector<GLuint> objectColorIds, vector<int> qttObjectsFragments)
{
  GLint posAttrShaderObject = glGetAttribLocation(
      programID, "vertexPosition_modelspace");
  GLint posAttrShaderColor = glGetAttribLocation(
      programID, "vertexColor");

  for (int objectMvpId = 0; objectMvpId < mvpIds.size(); objectMvpId++)
  {
    drawObject(mvpIds[objectMvpId], mvps[objectMvpId], objectIds[objectMvpId], objectColorIds[objectMvpId],
               qttObjectsFragments[objectMvpId], posAttrShaderObject, posAttrShaderColor);
  }
}

void drawObject(GLuint mvpId, mat4 mvp, GLuint objectId, GLuint objectColorId, int qttObjectFragments,
                GLint posAttrShaderObject, GLint posAttrShaderColor)
{
  // use the perspective of the vertex shader
  glUniformMatrix4fv(mvpId, 1, GL_FALSE, &mvp[0][0]);

  // object vertex
  glEnableVertexAttribArray(posAttrShaderObject);
  glBindBuffer(GL_ARRAY_BUFFER, objectId);
  glVertexAttribPointer(
      posAttrShaderObject, // shader in vertex shader file (layout(location = 0))
      3,                   // dimensions
      GL_FLOAT,            // type
      GL_FALSE,            // normalization
      0,                   // stride
      (void *)0            // array buffer offset
  );

  // color object
  glEnableVertexAttribArray(posAttrShaderColor);
  glBindBuffer(GL_ARRAY_BUFFER, objectColorId);
  glVertexAttribPointer(
      posAttrShaderColor, // shader in  MyVertexShader.lma (layout(location = 0))
      3,                  // dimensions
      GL_FLOAT,           // type
      GL_FALSE,           // normalization
      0,                  // stride
      (void *)0           // array buffer offset
  );

  // Draw the fragments(triangles) !
  glDrawArrays(GL_TRIANGLES, 0, qttObjectFragments * 3);

  glDisableVertexAttribArray(posAttrShaderObject);
  glDisableVertexAttribArray(posAttrShaderColor);
}

// Data Objects
GLuint createCube(vector<int> *qttObjectsFragments)
{
  vector<GLfloat> objectVerticesData;

  GLfloat triangle1[] = {
      -1.0f, -1.0f, 0.0f,
      1.0f, -1.0f, 0.0f,
      0.0f, 1.0f, 0.0f};
  GLfloat triangle2[] = {
      1.0f, 1.0f, -1.0f,
      -1.0f, -1.0f, -1.0f,
      -1.0f, 1.0f, -1.0f};
  GLfloat triangle3[] = {
      1.0f, -1.0f, 1.0f,
      -1.0f, -1.0f, -1.0f,
      1.0f, -1.0f, -1.0f};
  GLfloat triangle4[] = {
      1.0f, 1.0f, -1.0f,
      1.0f, -1.0f, -1.0f,
      -1.0f, -1.0f, -1.0f};
  GLfloat triangle5[] = {
      -1.0f, -1.0f, -1.0f,
      -1.0f, 1.0f, 1.0f,
      -1.0f, 1.0f, -1.0f};
  GLfloat triangle6[] = {
      1.0f, -1.0f, 1.0f,
      -1.0f, -1.0f, 1.0f,
      -1.0f, -1.0f, -1.0f};
  GLfloat triangle7[] = {
      -1.0f, 1.0f, 1.0f,
      -1.0f, -1.0f, 1.0f,
      1.0f, -1.0f, 1.0f};
  GLfloat triangle8[] = {
      1.0f, 1.0f, 1.0f,
      1.0f, -1.0f, -1.0f,
      1.0f, 1.0f, -1.0f};
  GLfloat triangle9[] = {
      1.0f, -1.0f, -1.0f,
      1.0f, 1.0f, 1.0f,
      1.0f, -1.0f, 1.0f};
  GLfloat triangle10[] = {
      1.0f, 1.0f, 1.0f,
      1.0f, 1.0f, -1.0f,
      -1.0f, 1.0f, -1.0f};
  GLfloat triangle11[] = {
      1.0f, 1.0f, 1.0f,
      -1.0f, 1.0f, -1.0f,
      -1.0f, 1.0f, 1.0f};
  GLfloat triangle12[] = {
      1.0f, 1.0f, 1.0f,
      -1.0f, 1.0f, 1.0f,
      1.0f, -1.0f, 1.0f};

  extendVector(&objectVerticesData, triangle1, 3);
  extendVector(&objectVerticesData, triangle2, 3);
  extendVector(&objectVerticesData, triangle3, 3);
  extendVector(&objectVerticesData, triangle4, 3);
  extendVector(&objectVerticesData, triangle5, 3);
  extendVector(&objectVerticesData, triangle6, 3);
  extendVector(&objectVerticesData, triangle7, 3);
  extendVector(&objectVerticesData, triangle8, 3);
  extendVector(&objectVerticesData, triangle9, 3);
  extendVector(&objectVerticesData, triangle10, 3);
  extendVector(&objectVerticesData, triangle11, 3);
  extendVector(&objectVerticesData, triangle12, 3);

  qttObjectsFragments->push_back(objectVerticesData.size());

  return (createObjectBuffer(objectVerticesData));
}

GLuint createPyramid(vector<int> *qttObjectsFragments)
{
  vector<GLfloat> objectVerticesData;

  GLfloat triangle1[] = {
      -1.0f, 0.0f, 0.0f,
      1.0f, 0.0f, 0.0f,
      0.0f, 1.0f, -0.5f};
  GLfloat triangle2[] = {
      -1.0f, 0.0f, 0.0f,
      -1.0f, 0.0f, -1.0f,
      0.0f, 1.0f, -0.5f};
  GLfloat triangle3[] = {
      1.0f, 0.0f, 0.0f,
      1.0f, 0.0f, -1.0f,
      0.0f, 1.0f, -0.5f};
  GLfloat triangle4[] = {
      -1.0f, 0.0f, -1.0f,
      1.0f, 0.0f, -1.0f,
      0.0f, 1.0f, -0.5f};
  GLfloat triangle5[] = {
      -1.0f, 0.0f, 0.0f,
      1.0f, 0.0f, 0.0f,
      -1.0f, 0.0f, -1.0f};
  GLfloat triangle6[] = {
      1.0f, 0.0f, 0.0f,
      1.0f, 0.0f, -1.0f,
      -1.0f, 0.0f, -1.0f};

  extendVector(&objectVerticesData, triangle1, 3);
  extendVector(&objectVerticesData, triangle2, 3);
  extendVector(&objectVerticesData, triangle3, 3);
  extendVector(&objectVerticesData, triangle4, 3);
  extendVector(&objectVerticesData, triangle5, 3);
  extendVector(&objectVerticesData, triangle6, 3);

  qttObjectsFragments->push_back(objectVerticesData.size());

  return (createObjectBuffer(objectVerticesData));
}

GLuint createCubeColor()
{
  vector<GLfloat> objectColorData;

  GLfloat colorTriangle1[] = {
      0.583f, 0.771f, 0.014f,
      0.609f, 0.115f, 0.436f,
      0.327f, 0.483f, 0.844f};
  GLfloat colorTriangle2[] = {
      0.822f, 0.569f, 0.201f,
      0.435f, 0.602f, 0.223f,
      0.310f, 0.747f, 0.185f};
  GLfloat colorTriangle3[] = {
      0.597f, 0.770f, 0.761f,
      0.559f, 0.436f, 0.730f,
      0.359f, 0.583f, 0.152f};
  GLfloat colorTriangle4[] = {
      0.483f, 0.596f, 0.789f,
      0.559f, 0.861f, 0.639f,
      0.195f, 0.548f, 0.859f};
  GLfloat colorTriangle5[] = {
      0.014f, 0.184f, 0.576f,
      0.771f, 0.328f, 0.970f,
      0.406f, 0.615f, 0.116f};
  GLfloat colorTriangle6[] = {
      0.676f, 0.977f, 0.133f,
      0.971f, 0.572f, 0.833f,
      0.140f, 0.616f, 0.489f};
  GLfloat colorTriangle7[] = {
      0.997f, 0.513f, 0.064f,
      0.945f, 0.719f, 0.592f,
      0.543f, 0.021f, 0.978f};
  GLfloat colorTriangle8[] = {
      0.279f, 0.317f, 0.505f,
      0.167f, 0.620f, 0.077f,
      0.347f, 0.857f, 0.137f};
  GLfloat colorTriangle9[] = {
      0.055f, 0.953f, 0.042f,
      0.714f, 0.505f, 0.345f,
      0.783f, 0.290f, 0.734f};
  GLfloat colorTriangle10[] = {
      0.722f, 0.645f, 0.174f,
      0.302f, 0.455f, 0.848f,
      0.225f, 0.587f, 0.040f};
  GLfloat colorTriangle11[] = {
      0.517f, 0.713f, 0.338f,
      0.053f, 0.959f, 0.120f,
      0.393f, 0.621f, 0.362f};
  GLfloat colorTriangle12[] = {
      0.673f, 0.211f, 0.457f,
      0.820f, 0.883f, 0.371f,
      0.982f, 0.099f, 0.879f};

  extendVector(&objectColorData, colorTriangle1, 3);
  extendVector(&objectColorData, colorTriangle2, 3);
  extendVector(&objectColorData, colorTriangle3, 3);
  extendVector(&objectColorData, colorTriangle4, 3);
  extendVector(&objectColorData, colorTriangle5, 3);
  extendVector(&objectColorData, colorTriangle6, 3);
  extendVector(&objectColorData, colorTriangle7, 3);
  extendVector(&objectColorData, colorTriangle8, 3);
  extendVector(&objectColorData, colorTriangle9, 3);
  extendVector(&objectColorData, colorTriangle10, 3);
  extendVector(&objectColorData, colorTriangle11, 3);
  extendVector(&objectColorData, colorTriangle12, 3);

  return (createObjectBuffer(objectColorData));
}

GLuint createPyramidColor()
{
  int qttFragments = 6;
  vector<GLfloat> objectColorData;

  for (int i = 0; i < qttFragments * 3; i++)
  {
    objectColorData.push_back(0.5f); // Red
    objectColorData.push_back(0.0f); // Green
    objectColorData.push_back(0.0f); // Blue
  }

  return (createObjectBuffer(objectColorData));
}

void addCubePerspective(GLuint *programID, vector<GLuint> *mvpIds, vector<mat4> *mvps)
{
  // Get a handle for our "MVP" uniform in MyVertexShader.lvet
  // Only during the initialisation
  mvpIds->push_back(glGetUniformLocation(*programID, "MVP"));

  // Screen Projection, 45° Field of View
  mat4 Projection = perspective(radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

  // View: Camera matrix
  mat4 View = lookAt(
      vec3(10, 3, 3), // Camera position (z, y, x)
      vec3(0, 0, 0),  // Camera focus (z, y, x)
      vec3(0, 1, 0)   // Head is up
  );

  // Model matrix
  mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
  mat4 Model = translationMatrix * mat4(1.0f); // model matix can to be Translation * Rotation * Scale

  // ModelViewProjection
  mat4 mvpgenerated = Projection * View * Model;
  mvps->push_back(mvpgenerated);
}

void addPyramidPerspective(GLuint *programID, vector<GLuint> *mvpIds, vector<mat4> *mvps)
{
  // Get a handle for our "MVP" uniform in MyVertexShader.lvet
  // Only during the initialisation
  mvpIds->push_back(glGetUniformLocation(*programID, "MVP"));

  // Screen Projection, 45° Field of View
  mat4 Projection = perspective(radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

  // View: Camera matrix
  mat4 View = lookAt(
      vec3(10, 3, 3), // Camera position (z, y, x)
      vec3(0, 0, 0),  // Camera focus (z, y, x)
      vec3(0, 1, 0)   // Head is up
  );

  // Model matrix
  mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 0.0f, 0.0f));
  mat4 Model = translationMatrix * mat4(1.0f); // model matix can to be Translation * Rotation * Scale

  // ModelViewProjection
  mat4 mvpgenerated = Projection * View * Model;
  mvps->push_back(mvpgenerated);
}
