// Include GLFW
#include <GLFW/glfw3.h>
extern GLFWwindow* window;
// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include "controls.h"

float pi = 3.14159265359;
glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;
glm::vec3 Pos;
glm::mat4 ModelMatrix;
glm::mat4 model = glm::mat4(1.0f);
glm::mat4 rotMatrix = glm::mat4(1.0f);
glm::mat4 getViewMatrix(){
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix(){
	return ProjectionMatrix;
}
glm::mat4 getModelMatrix(){
	return ModelMatrix;
}

// Initial position : on +Z
glm::vec3 position = glm::vec3( 0, 0, 5 ); 

// Initial horizontal angle
float angle_z = 0.0f;
// Initial vertical angle
float angle_x = 0.0f;


float FoV = 45.0f;

float speed = 15.0f; // 15 units / second
float mouseSpeed = 0.001f;



void computeMatricesFromInputs(){


	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Reset mouse position for next frame
	glfwSetCursorPos(window, 1024/2, 768/2);

	// Compute new orientation
	angle_z = -mouseSpeed * float(1024/2 - xpos );
	angle_x = -mouseSpeed * float(768/2 - ypos );

	glm::vec3 myRotationAxis_x(1,0,0);
	glm::mat4 rotationMatrixX = glm::rotate(angle_x*180.0f/pi, myRotationAxis_x);
	glm::vec3 myRotationAxis_y(0,1,0);
	glm::mat4 rotationMatrixY = glm::rotate(angle_z*180.0f/pi, myRotationAxis_y);
	rotMatrix= rotationMatrixX*rotationMatrixY*rotMatrix;
	glm::vec3 direction(0.0f,0.0f,0.1f);  //Direction of translation
	// Move forward
	if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS){
		Pos -= (direction * deltaTime * speed);
		model = glm::translate(glm::mat4(), Pos);
	}
	// Move backward
	if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS){
		Pos += direction * deltaTime * speed;
		model = glm::translate(glm::mat4(), Pos);
	}
	ModelMatrix = model*rotMatrix;

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	ViewMatrix       = glm::lookAt(
								position,           // Camera is here
								glm::vec3(0,0,0), // and looks here : at the same position
								glm::vec3(0,1,0)             // Head is up (set to 0,-1,0 to look upside-down)
						   );

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}