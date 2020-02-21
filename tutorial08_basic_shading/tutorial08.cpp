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
#include "../common/objloader.hpp"

using namespace std;
using namespace glm;

static const int WINDOW_WIDTH = 1366;
static const int WINDOW_HEIGHT = 768;
static const char TITLE[50] = "Tutorial 07";

void addHints();
GLFWwindow *openWindow();
GLFWwindow *openWindow();
void createOpenGlContext(GLFWwindow *window);
void createWindow(GLFWwindow **window, GLuint *vertexArrayId);
void addInputs(GLFWwindow *window);
void defineVertexArray(GLuint *VertexArrayID);
GLuint createVertexBuffer(vector<vec3> vertices);
void createObjects(GLuint *programID, vector<GLuint> *verticesIds, vector<GLuint> *uvsIds, vector<GLuint> *normalsIds, vector<GLuint> *textureIds, vector<int> *qttObjectsFragments);
void loadShaders(GLuint *programID);
void extendVector(vector<GLfloat> *objectVertices, GLfloat *object, int qttPoints);
void swapBuffers(GLFWwindow *window);
void run(GLFWwindow *window, GLuint programID, vector<GLuint> verticesIds, vector<GLuint> uvsIds, vector<GLuint> normalsIds, vector<GLuint> textureIds, vector<int> qttObjectsFragments);
void draw(GLuint programID, vector<GLuint> mvpIds, vector<mat4> attribsMvp, vector<GLuint> verticesIds, vector<GLuint> uvsIds, vector<GLuint> normalsIds, vector<GLuint> textureIds, vector<int> qttObjectsFragments);
void drawObject(vector<GLuint> mvpIds, vector<mat4> attribsMvp, GLuint verticesId, GLuint uvsId, GLuint normalsIds, GLuint textureId, int qttObjectFragments, GLint attrShaderObject, GLint attrShaderTexture, GLint attrShaderUv);
GLuint createTexture();

int main()
{
  GLFWwindow *window;
  GLuint programID;
  GLuint vertexArrayID; // basis to use vertices (points of objects)
  vector<GLuint> verticesIds;
  vector<GLuint> uvsIds;
  vector<GLuint> normalsIds;
  vector<GLuint> textureIds;
  vector<int> qttObjectsFragments;

  createWindow(&window, &vertexArrayID);
  addInputs(window);
  loadShaders(&programID);

  createObjects(&programID, &verticesIds, &uvsIds, &normalsIds, &textureIds, &qttObjectsFragments);
  run(window, programID, verticesIds, uvsIds, normalsIds, textureIds, qttObjectsFragments);

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

template <class T>
GLuint createBuffer(vector<T> bufferData)
{

  GLuint bufferId;
  glGenBuffers(1, &bufferId);
  glBindBuffer(GL_ARRAY_BUFFER, bufferId);
  glBufferData(
      GL_ARRAY_BUFFER,
      bufferData.size() * sizeof(T),
      &bufferData[0], GL_STATIC_DRAW);
  return bufferId;
}

void createObjects(GLuint *programID, vector<GLuint> *verticesIds, vector<GLuint> *uvsIds,
                   vector<GLuint> *normalsIds, vector<GLuint> *textureIds,
                   vector<int> *qttObjectsFragments)
{
  // Cube
  std::vector<glm::vec3> vertices, normals;
  std::vector<glm::vec2> uvs;
  loadOBJ("./suzanne.obj", vertices, uvs, normals);
  qttObjectsFragments->push_back(vertices.size());
  // Obj
  verticesIds->push_back(createBuffer(vertices));
  uvsIds->push_back(createBuffer(uvs));
  normalsIds->push_back(createBuffer(normals));
  // Texture
  textureIds->push_back(createTexture());
  // textureIds->push_back(0);
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

vector<mat4> perspectiveControl(GLFWwindow *window)
{
  vector<mat4> attribsMVp;
  computeMatricesFromInputs(window, WINDOW_WIDTH, WINDOW_HEIGHT, false);
  mat4 ProjectionMatrix = getProjectionMatrix();
  mat4 ViewMatrix = getViewMatrix();
  mat4 ModelMatrix = mat4(1.0);
  mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

  attribsMVp.push_back(MVP);
  attribsMVp.push_back(ModelMatrix);
  attribsMVp.push_back(ViewMatrix);
  return attribsMVp;
}

void run(GLFWwindow *window, GLuint programID,
         vector<GLuint> verticesIds, vector<GLuint> uvsIds, vector<GLuint> normalsIds,
         vector<GLuint> textureIds, vector<int> qttObjectsFragments)
{
  vector<GLuint> mvpIds;
  GLuint mvpId, modelId, viewId;
  
  // Get a handle for our "MVP" uniform in vertex shader file
  // Only during the initialisation
  vector<mat4> attribsMvp;
  mvpId = glGetUniformLocation(programID, "MVP");
  modelId = glGetUniformLocation(programID, "M");
  viewId = glGetUniformLocation(programID, "V");
  
  mvpIds.push_back(mvpId);
  mvpIds.push_back(modelId);
  mvpIds.push_back(viewId);

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
    attribsMvp = perspectiveControl(window);
    draw(programID, mvpIds, attribsMvp, verticesIds, uvsIds, normalsIds, textureIds, qttObjectsFragments);
    swapBuffers(window);
  } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0);
}

void draw(GLuint programID, vector<GLuint> mvpIds, vector<mat4> attribsMvp,
          vector<GLuint> verticesIds, vector<GLuint> uvsIds, vector<GLuint> normalsIds,
          vector<GLuint> textureIds, vector<int> qttObjectsFragments)
{
  GLint attrShaderObject = glGetAttribLocation(
      programID, "vertexPosition_modelspace");
  GLint attrShaderUv = glGetAttribLocation(
      programID, "vertexUV");
  GLuint attrShaderTexture = glGetUniformLocation(programID, "myTextureSampler");


  for (int objectId = 0; objectId < verticesIds.size(); objectId++)
  {
    drawObject(mvpIds, attribsMvp, verticesIds[objectId], uvsIds[objectId], normalsIds[objectId],
               textureIds[objectId], qttObjectsFragments[objectId],
               attrShaderObject, attrShaderTexture, attrShaderUv);
  }
}

void drawObject(vector<GLuint> mvpIds, vector<mat4> attribsMvp, GLuint verticesId, GLuint uvsId, GLuint normalsId,
                GLuint textureId, int qttObjectFragments, GLint attrShaderObject,
                GLint attrShaderTexture, GLint attrShaderUv)
{
  mat4 mvp = attribsMvp[0];
  mat4 modelMatrix = attribsMvp[1];
  mat4 viewMatrix = attribsMvp[2];

  GLuint mvpId = mvpIds[0];
  GLuint modelId = mvpIds[1];
  GLuint viewId = mvpIds[2];

  // use the perspective of the vertex shader
  glUniformMatrix4fv(mvpId, 1, GL_FALSE, &mvp[0][0]);
  glUniformMatrix4fv(modelId, 1, GL_FALSE, &modelMatrix[0][0]);
  glUniformMatrix4fv(viewId, 1, GL_FALSE, &viewMatrix[0][0]);


  // Bind our texture in Texture Unit 0
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textureId);
  // Set our "myTextureSampler" sampler to use Texture Unit 0
  glUniform1i(attrShaderTexture, 0);

  // Vertices
  glEnableVertexAttribArray(attrShaderObject);
  glBindBuffer(GL_ARRAY_BUFFER, verticesId);
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
  glBindBuffer(GL_ARRAY_BUFFER, uvsId);
  glVertexAttribPointer(
      attrShaderUv, // shader in  vertex shader file (layout(location = 1))
      2,            // dimensions
      GL_FLOAT,     // type
      GL_FALSE,     // normalization
      0,            // stride
      (void *)0     // array buffer offset
  );

  // Normals Buffer
  glEnableVertexAttribArray(2);
  glBindBuffer(GL_ARRAY_BUFFER, normalsId);
  glVertexAttribPointer(
      2,                // shader position
      3,                // dimensions
      GL_FLOAT,         // type
      GL_FALSE,         // normalization
      0,                // stride
      (void *)0         // array buffer offset
  );

  // Draw the fragments(triangles) !
  glDrawArrays(GL_TRIANGLES, 0, qttObjectFragments * 3);

  glDisableVertexAttribArray(attrShaderObject);
  glDisableVertexAttribArray(attrShaderUv);
  glDisableVertexAttribArray(2);
}

GLuint createTexture()
{
  // Open the file
  cout << "Loading texture" << endl;
  // GLuint textureId = loadBMPFile("./uvtemplate.bmp");
  GLuint textureId = loadDDS("./uvmap.DDS");

  return textureId;
}
