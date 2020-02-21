#ifndef CONTROLS_HPP
#define CONTROLS_HPP
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

using namespace glm;


void computeMatricesFromInputs(GLFWwindow *window, int width, int height, bool lookCenter);
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();

#endif