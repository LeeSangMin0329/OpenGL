#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include <iostream>
#include <stdlib.h>
#include <vector>

#include "shader.h"
#include "controls.h"
#include "objloader.h"

// extern API
#include "texture.hpp"
#include "vboindexer.hpp"

using namespace glm;
using namespace std;

GLFWwindow* window;

int main(void)
{
	// GLFW
	if( !glfwInit() )
	{
		cerr << "GLFW Init fail." << endl;
		exit(EXIT_FAILURE);
	}

	// window set & openGL context in
	glfwWindowHint(GLFW_SAMPLES, 4); // Anti-aliasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // version over 3.3 api
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // deprecated code not use.
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE); // resiable

	window = glfwCreateWindow( 1024, 768, "Chicago white sox", NULL, NULL);
	if( window == NULL ){
		cerr << "GLFW window open fail." << endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);

	// GLEW
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		cerr << "GLEW init fail." << endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// print spec
	cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
	cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
	cout << "Vendor: " << glGetString(GL_VENDOR) << endl;
	cout << "Renderer: " << glGetString(GL_RENDERER) << endl;
	
	// key set
	//glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	// hide mouse
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// event polling
    glfwPollEvents();
	// init mouse cursor
    glfwSetCursorPos(window, 1024/2, 768/2);

	// V-sync
	glfwSwapInterval(1);

	// set background
	glClearColor(0.0f, 0.0f, 0.6f, 0.0f);

	// Z-buffer
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	
	// VAO
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	
	// Shader
	GLuint programID = LoadShaders("shaderCode.vertexshader", "shaderCode.fragmentshader");

	// Transform matrix
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");

	// Texture
	GLuint Texture = loadDDS("sicagoTexture.dds");
	GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");

	// Load .obj
	vector<vec3> vertices;
	vector<vec2> uvs;
	vector<vec3> normals;
	bool res = LoadOBJ("sicago.obj", vertices, uvs, normals);

	// EBO
	vector<unsigned short> elements;
	vector<vec3> elementsVertices;
	vector<vec2> elementsUvs;
	vector<vec3> elementsNormals;
	indexVBO(vertices, uvs, normals, elements, elementsVertices, elementsUvs, elementsNormals);

	// VBO
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, elementsVertices.size() * sizeof(vec3), &elementsVertices[0], GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, elementsUvs.size() * sizeof(vec2), &elementsUvs[0], GL_STATIC_DRAW);

	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, elementsNormals.size() * sizeof(vec3), &elementsNormals[0], GL_STATIC_DRAW);

	// EBO
	GLuint elementbuffer;
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(unsigned short), &elements[0] , GL_STATIC_DRAW);

	glUseProgram(programID);

	// Lighting
	GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

	// Frame check
	double lastTime = glfwGetTime();
	int numOfFrames = 0;

	do{
		// Measure frame
		double currentTime = glfwGetTime();
		numOfFrames++;
		if ( currentTime - lastTime >= 1.0 ){
			cout << 1000.0 / double(numOfFrames) << " ms/frame " << numOfFrames << "fps" << endl;
			numOfFrames = 0;
			lastTime += 1.0;
		}

		// Clear screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// shader on
		glUseProgram(programID);

		// compute tramform matrix
		ComputeMatricesFromInputs();
		mat4 ProjectionMatrix = getProjectionMatrix();
		mat4 ViewMatrix = getViewMatrix();
		mat4 ModelMatrix = mat4(1.0); // normalize mat
		mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		// Send to Shader computed matrix
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

		// Send to Shader light position
		vec3 lightPos = vec3(4,4,4);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

		// Bind Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		// "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(TextureID, 0);
		
		// Attribute Location method is 3
		// (1) shader layout numbering
		//  2  shader link state use per-link glBindAttribLocation(program, index, variable name)
		//  3  shader link state auto assignment index. can see glGetAttribLocation(program, variable name)
		
		// 1st attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute loaction
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			sizeof(float) * 3,  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(
			1,                                // attribute
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			sizeof(float) * 2,                // stride
			(void*)0                          // array buffer offset
		);

		// 3rd attribute buffer : normals
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glVertexAttribPointer(
			2,                                // attribute
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			sizeof(float) * 3,                // stride
			(void*)0                          // array buffer offset
		);
		
		// Element buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

		// accel VAO use EBO
		glDrawElements(
			GL_TRIANGLES,		// mode
			elements.size(),	// count
			GL_UNSIGNED_SHORT,  // type
			(void*)0			// element array buffer offset
		);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Delete
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteBuffers(1, &elementbuffer);
	glDeleteProgram(programID);
	glDeleteTextures(1, &Texture);
	glDeleteVertexArrays(1, &VertexArrayID);

	// terminate GLFW
	glfwTerminate();

	return 0;
}