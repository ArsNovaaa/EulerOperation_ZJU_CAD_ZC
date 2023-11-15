#include <GLFW/glfw3.h>
  
#include "common/controls.h"

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix() {
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix() {
	return ProjectionMatrix;
}

// position
glm::vec3 position = glm::vec3(0, 0, 5);
// horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// vertical angle : 0, look at the horizon
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 45.0f;

float speed = 3.0f; // 3 units / second

float mouseSpeed = 0.005f;



glm::mat4 getView(GLFWwindow* const window,bool& canMouseRotate)
{
    static double lastTime = glfwGetTime();
    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);

   
    if (canMouseRotate)
    {
        horizontalAngle -= mouseSpeed * ImGui::GetIO().MouseDelta.x;
        verticalAngle -= mouseSpeed * ImGui::GetIO().MouseDelta.y;
    }
    

    // Direction : Spherical coordinates to Cartesian coordinates conversion
    glm::vec3 direction(
        cos(verticalAngle) * sin(horizontalAngle),
        sin(verticalAngle),
        cos(verticalAngle) * cos(horizontalAngle));

    // Right vector
    glm::vec3 right = glm::vec3(
        sin(horizontalAngle - 3.14f / 2.0f),
        0,
        cos(horizontalAngle - 3.14f / 2.0f));

    // Up vector
    glm::vec3 up = glm::cross(right, direction);

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

    float FoV = initialFoV;

    glm::mat4 ViewMatrix = glm::lookAt(
        position,             // Camera is here
        position + direction, // and looks here : at the same position, plus "direction"
        up                    // Head is up (set to 0,-1,0 to look upside-down)
    );

    lastTime = currentTime;
    return ViewMatrix;
}