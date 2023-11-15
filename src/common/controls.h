#pragma once
#ifndef CONTROLS_HPP
#define CONTROLS_HPP
#include"glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include"imgui.h"
glm::mat4 getView(GLFWwindow* const window,bool& canMouseRotate);
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();

#endif