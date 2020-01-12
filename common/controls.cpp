#include <stdio.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include "controls.hpp"

using namespace std;

static const float SPEED = 3.0f; // 3 units / second
static const float MOUSE_SPEED = 0.005f;

glm::vec3 position = glm::vec3(0, 0, 5);
// horizontal angle : toward -Z
float horixontalAngle = 3.14f;
// vertical angle : 0, look at the horizon
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 45.0f;

void computeMatricesFromInputs(GLFWwindow *window, int width, int height)
{
  // glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

  // Get mouse position
  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);

  // Reset mouse position for next frame
  glfwSetCursorPos(window, width/2, height/2);
}
