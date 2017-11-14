#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\quaternion.hpp>

#include <math.h>

#include "controls.h"

using namespace glm;

extern GLFWwindow* window;

mat4 ViewMatrix;
mat4 ProjectionMatrix;

mat4 getViewMatrix(){
	return ViewMatrix;
}
mat4 getProjectionMatrix(){
	return ProjectionMatrix;
}

vec3 position = vec3( 0, 0, 5 ); 
float horizontalAngle = 3.14f;
float verticalAngle = 0.0f;
float initialFoV = 45.0f;

float rotAngle = 180.0f;
vec3 offset = vec3(0, 0, 0);

float speed = 3.0f; // per second
float mouseSpeed = 0.005f;

void ComputeMatricesFromInputs(){

	static double lastTime = glfwGetTime(); // keep

	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// camera direction
	vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle), 
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);
	
	// view direction vector
	vec3 right = vec3(
		sin(horizontalAngle - 3.14f/2.0f), 
		0,
		cos(horizontalAngle - 3.14f/2.0f)
	);
	vec3 up = cross( right, direction );
	vec3 back = cross(up, right);

	float FoV = initialFoV + 20;

	position = vec3(0, 1, 0) +vec3(4 * cos(currentTime), 0, 4 * sin(currentTime));
	direction = vec3(0, 0, 0);
	
	// Projection matrix :	  Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = perspective(FoV, 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	ViewMatrix       = lookAt(
								position,           // Camera is here
								direction,			// looks here
								up                  // Head is up
						   );

	lastTime = currentTime;
}