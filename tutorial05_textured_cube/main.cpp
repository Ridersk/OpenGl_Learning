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
static const char TITLE[50] = "Tutorial 01";

void addHints();
GLFWwindow *openWindow();
GLFWwindow *openWindow();
void createOpenGlContext(GLFWwindow *window);
void createWindow(GLFWwindow **window, GLuint *vertexArrayId);
void addInputs(GLFWwindow *window);
void defineVertexArray(GLuint *VertexArrayID);
GLuint createObjectBuffer(vector<GLfloat> bufferData);
GLuint createObjectTexture(int width, int height, unsigned char *data);
void createObjects(GLuint *programID, vector<GLuint> *mvpIds, vector<mat4> *mvps, vector<GLuint> *objectIds, vector<GLuint> *objectTextureIds, vector<int> *qttObjectsFragments);
void loadShaders(GLuint *programID);
void extendVector(vector<GLfloat> *objectVertices, GLfloat *object, int qttPoints);
void swapBuffers(GLFWwindow *window);
void run(GLFWwindow *window, GLuint programID, vector<GLuint> mvpIds, vector<mat4> mvps, vector<GLuint> objectIds, vector<GLuint> objectTextureIds, vector<int> qttObjectsFragments);
void draw(GLuint programID, vector<GLuint> mvpIds, vector<mat4> mvps, vector<GLuint> objectIds, vector<GLuint> objectTextureIds, vector<int> qttObjectsFragments);
void drawObject(GLuint mvpId, mat4 mvp, GLuint objectId, GLuint objectColorId, int qttObjectFragments, GLint posAttrShaderObject, GLint posAttrShaderColor);
GLuint loadBMPFile(const char *imagepath);
GLuint createCube(vector<int> *qttObjectsFragments);
GLuint createCubeTexture();
void addCubePerspective(GLuint *programID, vector<GLuint> *mvpIds, vector<mat4> *mvps);

int main()
{
  GLFWwindow *window;
  GLuint programID;
  GLuint vertexArrayID; // basis to use vertices (points of objects)
  vector<GLuint> mvpIds;
  vector<mat4> mvps;
  vector<GLuint> objectIds;
  vector<GLuint> objectTextureIds;
  vector<int> qttObjectsFragments;

  createWindow(&window, &vertexArrayID);
  addInputs(window);
  loadShaders(&programID);
  createObjects(&programID, &mvpIds, &mvps, &objectIds, &objectTextureIds, &qttObjectsFragments);
  run(window, programID, mvpIds, mvps, objectIds, objectTextureIds, qttObjectsFragments);
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

GLuint createObjectTexture(int width, int height, unsigned char *data)
{
  // Create texture
  GLuint textureId;
  glGenTextures(1, &textureId);
  glBindTexture(GL_TEXTURE_2D, textureId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,
               0, GL_BGR, GL_UNSIGNED_BYTE, data);
  glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                      GL_NEAREST);
  glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                      GL_NEAREST);
  return textureId;
}

void createObjects(GLuint *programID, vector<GLuint> *mvpIds, vector<mat4> *mvps, vector<GLuint> *objectIds,
                   vector<GLuint> *objectTextureIds, vector<int> *qttObjectsFragments)
{
  // Cube
  objectIds->push_back(createCube(qttObjectsFragments));
  createCubeTexture();
  addCubePerspective(programID, mvpIds, mvps);
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
         vector<GLuint> objectIds, vector<GLuint> objectTextureIds, vector<int> qttObjectsFragments)
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
    draw(programID, mvpIds, mvps, objectIds, objectTextureIds, qttObjectsFragments);
    swapBuffers(window);
  } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0);
}

void draw(GLuint programID, vector<GLuint> mvpIds, vector<mat4> mvps, vector<GLuint> objectIds,
          vector<GLuint> objectTextureIds, vector<int> qttObjectsFragments)
{
  GLint posAttrShaderObject = glGetAttribLocation(
      programID, "vertexPosition_modelspace");
  GLint posAttrShaderColor = glGetAttribLocation(
      programID, "vertexColor");

  for (int objectMvpId = 0; objectMvpId < mvpIds.size(); objectMvpId++)
  {
    drawObject(mvpIds[objectMvpId], mvps[objectMvpId], objectIds[objectMvpId], 1,
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

GLuint loadBMPFile(const char *imagepath)
{
  unsigned char header[54];
  unsigned int dataPos;
  unsigned int width, height;
  unsigned int imageSize;
  unsigned char *data;

  FILE *file = fopen(imagepath, "rb");
  if (!file)
  {
    printf("Image could not be opened\n");
    exit(0);
  }

  if (fread(header, 1, 54, file) != 54)
  { // If not 54 bytes read : problem
    printf("Not a correct BMP file\n");
    exit(0);
  }

  if (header[0] != 'B' || header[1] != 'M')
  {
    printf("Not a correct BMP file\n");
    exit(0);
  }

  // Read ints from the byte array
  dataPos = *(int *)&(header[0x0A]);
  imageSize = *(int *)&(header[0x22]);
  width = *(int *)&(header[0x12]);
  height = *(int *)&(header[0x16]);

  // Fix some infos
  if (imageSize == 0)
    imageSize = width * height * 3; // 3: RGB
  if (dataPos == 0)
    dataPos = 54; // 54: Size of header

  // Create a buffer
  data = new unsigned char[imageSize];

  // Read the actual data
  fread(data, 1, imageSize, file);

  fclose(file);

  return createObjectTexture(width, height, data);
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

GLuint createCubeTexture()
{
  // Open the file
  cout << "Loading texture" << endl;
  GLuint texture = loadBMPFile("./uvtemplate.bmp");

  return 1;
}
