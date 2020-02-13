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
#include "../common/texture.hpp"
#include "../common/controls.hpp"

using namespace std;
using namespace glm;

static const int WINDOW_WIDTH = 1366;
static const int WINDOW_HEIGHT = 768;
static const char TITLE[50] = "Tutorial 06";

void addHints();
GLFWwindow *openWindow();
GLFWwindow *openWindow();
void createOpenGlContext(GLFWwindow *window);
void createWindow(GLFWwindow **window, GLuint *vertexArrayId);
void addInputs(GLFWwindow *window);
void defineVertexArray(GLuint *VertexArrayID);
GLuint createObjectBuffer(vector<GLfloat> bufferData);
void createObjects(GLuint *programID, vector<GLuint> *objectIds, vector<GLuint> *objectTextureIds, vector<GLuint> *objectUvIds, vector<int> *qttObjectsFragments);
void loadShaders(GLuint *programID);
void extendVector(vector<GLfloat> *objectVertices, GLfloat *object, int qttPoints);
void swapBuffers(GLFWwindow *window);
void run(GLFWwindow *window, GLuint programID, GLuint mvpId, mat4 mvp, vector<GLuint> objectIds, vector<GLuint> objectTextureIds, vector<GLuint> objectUvIds, vector<int> qttObjectsFragments);
void draw(GLuint programID, GLuint mvpId, mat4 mvp, vector<GLuint> objectIds, vector<GLuint> objectTextureIds, vector<GLuint> objectUvIds, vector<int> qttObjectsFragments);
void drawObject(GLuint mvpId, mat4 mvp, GLuint objectId, GLuint objectTextureId, GLuint objectUvId, int qttObjectFragments, GLint attrShaderObject, GLint attrShaderTexture, GLint attrShaderUv);
GLuint createCube(vector<int> *qttObjectsFragments);
GLuint createCubeTexture();
GLuint createCubeUV();

int main()
{
  GLFWwindow *window;
  GLuint programID;
  GLuint vertexArrayID; // basis to use vertices (points of objects)
  GLuint mvpId;
  mat4 mvp;
  vector<GLuint> objectIds;
  vector<GLuint> objectTextureIds;
  vector<GLuint> objectUvIds;
  vector<int> qttObjectsFragments;
  


  createWindow(&window, &vertexArrayID);
  addInputs(window);
  loadShaders(&programID);
  // Get a handle for our "MVP" uniform in MyVertexShader.lvet
  // Only during the initialisation
  mvpId = glGetUniformLocation(programID, "MVP");
  createObjects(&programID, &objectIds, &objectTextureIds, &objectUvIds, &qttObjectsFragments);
  run(window, programID, mvpId, mvp, objectIds, objectTextureIds, objectUvIds, qttObjectsFragments);

  // Cleanup VBO and shader
  glDeleteProgram(programID);
  glDeleteVertexArrays(1, &vertexArrayID);

  // Close OpenGL window and terminate GLFW
  glfwTerminate();
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
  *programID = LoadShaders("TransformVertexShader.vertexshader", "TextureFragmentShader.fragmentshader");
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

void createObjects(GLuint *programID, vector<GLuint> *objectIds, vector<GLuint> *objectTextureIds,
                   vector<GLuint> *objectUvIds, vector<int> *qttObjectsFragments)
{
  // Cube
  objectIds->push_back(createCube(qttObjectsFragments));
  objectTextureIds->push_back(createCubeTexture());
  objectUvIds->push_back(createCubeUV());
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

mat4 perspectiveControl(GLFWwindow *window)
{
  computeMatricesFromInputs(window, WINDOW_WIDTH, WINDOW_HEIGHT);
  mat4 ProjectionMatrix = getProjectionMatrix();
  mat4 ViewMatrix = getViewMatrix();
  mat4 ModelMatrix = mat4(1.0);
  mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
  return MVP;
}

void run(GLFWwindow *window, GLuint programID, GLuint mvpId, mat4 mvp,
         vector<GLuint> objectIds, vector<GLuint> objectTextureIds, vector<GLuint> objectUvIds,
         vector<int> qttObjectsFragments)
{

  // Dark blue background
  glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

  // Enable depth test
  glEnable(GL_DEPTH_TEST);
  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS);
  // Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

  do
  {
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(programID); // use my program
    mvp = perspectiveControl(window);
    draw(programID, mvpId, mvp, objectIds, objectTextureIds, objectUvIds, qttObjectsFragments);
    swapBuffers(window);
  } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0);
}

void draw(GLuint programID, GLuint mvpId, mat4 mvp,
          vector<GLuint> objectIds, vector<GLuint> objectTextureIds,
          vector<GLuint> objectUvIds, vector<int> qttObjectsFragments)
{
  GLint attrShaderObject = glGetAttribLocation(
      programID, "vertexPosition_modelspace");
  GLint attrShaderUv = glGetAttribLocation(
      programID, "vertexUV");
  GLuint attrShaderTexture = glGetUniformLocation(programID, "myTextureSampler");

  for (int objectId = 0; objectId < objectIds.size(); objectId++)
  {
    drawObject(mvpId, mvp, objectIds[objectId],
               objectTextureIds[objectId], objectUvIds[objectId], qttObjectsFragments[objectId],
               attrShaderObject, attrShaderTexture, attrShaderUv);
  }
}

void drawObject(GLuint mvpId, mat4 mvp, GLuint objectId, GLuint objectTextureId, GLuint objectUvId,
                int qttObjectFragments, GLint attrShaderObject, GLint attrShaderTexture,
                GLint attrShaderUv)
{
  // use the perspective of the vertex shader
  glUniformMatrix4fv(mvpId, 1, GL_FALSE, &mvp[0][0]);

  // Bind our texture in Texture Unit 0
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, objectTextureId);
  // Set our "myTextureSampler" sampler to use Texture Unit 0
  glUniform1i(attrShaderTexture, 0);

  // object vertex
  glEnableVertexAttribArray(attrShaderObject);
  glBindBuffer(GL_ARRAY_BUFFER, objectId);
  glVertexAttribPointer(
      attrShaderObject, // shader in vertex shader file (layout(location = 0))
      3,                // dimensions
      GL_FLOAT,         // type
      GL_FALSE,         // normalization
      0,                // stride
      (void *)0         // array buffer offset
  );

  // UV Buffer
  glEnableVertexAttribArray(attrShaderUv);
  glBindBuffer(GL_ARRAY_BUFFER, objectUvId);
  glVertexAttribPointer(
      attrShaderUv, // shader in  MyVertexShader.lma (layout(location = 0))
      2,            // dimensions
      GL_FLOAT,     // type
      GL_FALSE,     // normalization
      0,            // stride
      (void *)0     // array buffer offset
  );

  // Draw the fragments(triangles) !
  glDrawArrays(GL_TRIANGLES, 0, qttObjectFragments * 3);

  glDisableVertexAttribArray(attrShaderObject);
  glDisableVertexAttribArray(attrShaderUv);
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

GLuint createCubeTexture()
{
  // Open the file
  cout << "Loading texture" << endl;
  // GLuint textureId = loadBMPFile("./uvtemplate.bmp");
  GLuint textureId = loadDDS("./uvtemplate.DDS");

  return textureId;
}

GLuint createCubeUV()
{
  vector<GLfloat> objectTextureData;

  GLfloat colorTriangle1[] = {
      0.000059f, 1.0f - 0.000004f,
      0.000103f, 1.0f - 0.336048f,
      0.335973f, 1.0f - 0.335903f};
  GLfloat colorTriangle2[] = {
      1.000023f, 1.0f - 0.000013f,
      0.667979f, 1.0f - 0.335851f,
      0.999958f, 1.0f - 0.336064f};
  GLfloat colorTriangle3[] = {
      0.667979f, 1.0f - 0.335851f,
      0.336024f, 1.0f - 0.671877f,
      0.667969f, 1.0f - 0.671889f};
  GLfloat colorTriangle4[] = {
      1.000023f, 1.0f - 0.000013f,
      0.668104f, 1.0f - 0.000013f,
      0.667979f, 1.0f - 0.335851f};
  GLfloat colorTriangle5[] = {
      0.000059f, 1.0f - 0.000004f,
      0.335973f, 1.0f - 0.335903f,
      0.336098f, 1.0f - 0.000071f};
  GLfloat colorTriangle6[] = {
      0.667979f, 1.0f - 0.335851f,
      0.335973f, 1.0f - 0.335903f,
      0.336024f, 1.0f - 0.671877f};
  GLfloat colorTriangle7[] = {
      1.000004f, 1.0f - 0.671847f,
      0.999958f, 1.0f - 0.336064f,
      0.667979f, 1.0f - 0.335851f};
  GLfloat colorTriangle8[] = {
      0.668104f, 1.0f - 0.000013f,
      0.335973f, 1.0f - 0.335903f,
      0.667979f, 1.0f - 0.335851f};
  GLfloat colorTriangle9[] = {
      0.335973f, 1.0f - 0.335903f,
      0.668104f, 1.0f - 0.000013f,
      0.336098f, 1.0f - 0.000071f};
  GLfloat colorTriangle10[] = {
      0.000103f, 1.0f - 0.336048f,
      0.000004f, 1.0f - 0.671870f,
      0.336024f, 1.0f - 0.671877f};
  GLfloat colorTriangle11[] = {
      0.000103f, 1.0f - 0.336048f,
      0.336024f, 1.0f - 0.671877f,
      0.335973f, 1.0f - 0.335903f};
  GLfloat colorTriangle12[] = {
      0.667969f, 1.0f - 0.671889f,
      1.000004f, 1.0f - 0.671847f,
      0.667979f, 1.0f - 0.335851f};

  extendVector(&objectTextureData, colorTriangle1, 2);
  extendVector(&objectTextureData, colorTriangle2, 2);
  extendVector(&objectTextureData, colorTriangle3, 2);
  extendVector(&objectTextureData, colorTriangle4, 2);
  extendVector(&objectTextureData, colorTriangle5, 2);
  extendVector(&objectTextureData, colorTriangle6, 2);
  extendVector(&objectTextureData, colorTriangle7, 2);
  extendVector(&objectTextureData, colorTriangle8, 2);
  extendVector(&objectTextureData, colorTriangle9, 2);
  extendVector(&objectTextureData, colorTriangle10, 2);
  extendVector(&objectTextureData, colorTriangle11, 2);
  extendVector(&objectTextureData, colorTriangle12, 2);

  return (createObjectBuffer(objectTextureData));
}
