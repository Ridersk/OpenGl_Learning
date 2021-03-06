#include <stdio.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

#include "controls.hpp"

using namespace std;

glm::mat4 ViewMatrix;
// glm::mat4 ViewMatrix = lookAt(
//     vec3(4, 3, 3), // Camera position (z, y, x)
//     vec3(3, 2, 2), // Camera focus (z, y, x)
//     vec3(0, 1, 0)  // Head is up
// );
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix()
{
  return ViewMatrix;
}
glm::mat4 getProjectionMatrix()
{
  return ProjectionMatrix;
}

float speed = 3.0f; // 3 units / second
float mouseSpeed = 0.005f;

// Fix position to look to the cube center
const static glm::vec3 CUBE_CENTER = vec3(5, 5, 5);
// initial position : on +z
glm::vec3 position = CUBE_CENTER;

// horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// vertical angle : 0, look at the horizon
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 45.0f;

#define lookObjectCenter(deltaTime) (CUBE_CENTER + vec3(2.0f * cos(deltaTime), 10.0f, 5.0f * sin(deltaTime)))


void computeMatricesFromInputs(GLFWwindow *window, int width, int height, bool lookCenter)
{
  // glfwGetTime is called only once, the first time this function is called
  static double lastTime = glfwGetTime();

  // Compute time difference between current and last frame
  double currentTime = glfwGetTime();
  float deltaTime = float(currentTime - lastTime);

  // Get mouse position
  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);

  // Reset mouse position for next frame
  glfwSetCursorPos(window, width / 2, height / 2);

  // Compute new orientation
  horizontalAngle += mouseSpeed * float(width / 2 - xpos);
  verticalAngle += mouseSpeed * float(height / 2 - ypos);

  // Direction: Spherical coordinates to Cartesian coordinates conversion
  glm::vec3 direction(
      cos(verticalAngle) * sin(horizontalAngle),
      sin(verticalAngle),
      cos(verticalAngle) * cos(horizontalAngle));

  // Right vector
  glm::vec3 right = glm::vec3(
      sin(horizontalAngle - 3.14f / 2.0f),
      0,
      cos(horizontalAngle - 3.14f / 2.0f));

  // Up vector : perpendicular to both direction and right
  glm::vec3 up = glm::cross(right, direction);

  // variable position to look the center of cube
  if (lookCenter) {
    position = lookObjectCenter(deltaTime);
  }

  // Move forward
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
  {
    position += direction * deltaTime * speed;
  }
  // Move backward
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
  {
    position -= direction * deltaTime * speed;
  }
  // Strafe right
  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
  {
    position += right * deltaTime * speed;
  }
  // Strafe left
  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
  {
    position -= right * deltaTime * speed;
  }

  float FoV = initialFoV; // - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

  // Projection matrix : 45&deg; Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
  ProjectionMatrix = glm::perspective(glm::radians(FoV), (float)width / (float)height, 0.1f, 100.0f);
  // Camera matrix
  ViewMatrix = glm::lookAt(
      position,             // Camera is here
      position + direction, // and looks here : at the same position, plus "direction"
      up                    // Head is up (set to 0,-1,0 to look upside-down)
  );

  // vec3 sum = position + direction;
  // cout << "Lookat: { [" << position[0] << endl;
  // printf("Lookat: { (%f, %f, %f), (%f, %f, %f), (%f, %f, %f)}\n", position[0], position[1], position[2],
  //        sum[0], sum[1], sum[2], up[0], up[1], up[2]);

  // For the next frame, the "last time" will be "now"
  lastTime = currentTime;
}
