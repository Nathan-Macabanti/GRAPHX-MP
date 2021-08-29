#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "main.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "glm/glm.hpp"
#include "obj_mesh.h";
#include "shader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "skybox.h"

struct mouseStuff {
	//Mouse Input
	int button;
	int state;

	double lastMouseX;
	double lastMouseY;
	double mouseX;
	double mouseY;
	double mouseOffsetX;
	double mouseOffsetY;
	bool firstMouse;
};

void updateMouseInput(GLFWwindow* window, mouseStuff* mouse, double dt) {
	glfwGetCursorPos(window, &mouse->mouseX, &mouse->mouseY);

	if (mouse->firstMouse) {
		mouse->lastMouseX = mouse->mouseX;
		mouse->lastMouseY = mouse->mouseY;
		mouse->firstMouse = false;
	}

	mouse->mouseOffsetX = (mouse->mouseX - mouse->lastMouseX) * dt;
	mouse->mouseOffsetY = (mouse->lastMouseY - mouse->mouseY) * dt;

	mouse->lastMouseX = mouse->mouseX;
	mouse->lastMouseY = mouse->mouseY;
}

void updateLight(GLFWwindow* window, glm::vec3* light, float dt) {
	float speed = 10;

	//TRANSLATE
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) { //BACKWARD
		light->z -= speed * dt;
		//std::cout << "W" << std::endl;
	}
	else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) { //BACKWARD
		light->z += speed * dt;
		//std::cout << "S" << std::endl;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) { //BACKWARD
		light->x -= speed * dt;
		//std::cout << "A" << std::endl;
	}
	else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) { //BACKWARD
		light->x += speed * dt;
		//std::cout << "D" << std::endl;
	}
	if (glfwGetKey(window, GLFW_KEY_PERIOD) == GLFW_PRESS) { //BACKWARD
		light->y -= speed * dt;
		//sca.y -= speed * dt;
	}
	else if (glfwGetKey(window, GLFW_KEY_COMMA) == GLFW_PRESS) { //BACKWARD
		light->y += speed * dt;
	}
}

void updateKeyInput(GLFWwindow* window, glm::vec3* cam, float dt) {
	float speed = 5;

	glm::vec3 Front = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

	//PROGRAM
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) { //FORWARD
		glfwWindowShouldClose(window);
	}

	//TRANSLATE
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { //BACKWARD
		*cam += (speed * dt) * Front;
		std::cout << "W" << std::endl;
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) { //BACKWARD
		*cam -= (speed * dt) * Front;
		std::cout << "S" << std::endl;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { //BACKWARD
		*cam -= glm::normalize(glm::cross(Front, Up)) * (speed * dt);
		std::cout << "A" << std::endl;
	}
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { //BACKWARD
		*cam += glm::normalize(glm::cross(Front, Up)) * (speed * dt);
		std::cout << "D" << std::endl;
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) { //BACKWARD
		*cam += Up * (speed * dt);
		//sca.y -= speed * dt;
	}
	else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) { //BACKWARD
		*cam -= Up * (speed * dt);
	}

	//ROTATE
	//rot.x += (mouse.mouseOffsetX * 180) / (atan(1) * 4);
	//rot.y += (mouse.mouseOffsetY * 180) / (atan(1) * 4);
}

int main() {
	stbi_set_flip_vertically_on_load(true);
#pragma region Initialization
	//Mouse Struct
	mouseStuff mouse;

	//initialize glfw
	if (glfwInit() != GLFW_TRUE) {
		fprintf(stderr, "Failed to initialized! \n");
		return -1;
	}

	// set opengl version to 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// create window 
	GLFWwindow* window;
	window = glfwCreateWindow(1024, 768, "Macabanti, Nathan", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to load window! \n");
		return -1;
	}
	glfwMakeContextCurrent(window);

	//initialize glew
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}
#pragma endregion


#pragma region Mesh Loading

	ObjData earth;
	LoadObjFile(&earth, "earth/Earth.obj");
	GLfloat bunnyOffsets[] = { 0.0f, 0.0f, 0.0f };
	LoadObjToMemory(
		&earth,
		1.0f,
		bunnyOffsets
	);

	ObjData sun;
	LoadObjFile(&sun, "earth/Earth.obj");
	GLfloat tourusOffsets[] = { 0.0f, 0.0f, 0.0f };
	LoadObjToMemory(
		&sun,
		1.0f,
		tourusOffsets
	);

	ObjData moon;
	LoadObjFile(&moon, "earth/Earth.obj");
	GLfloat moonOffsets[] = { 0.0f, 0.0f, 0.0f };
	LoadObjToMemory(
		&moon,
		1.0f,
		moonOffsets
	);

	//Load Skybox Model
	std::vector<std::string> faces{
		"right.png",
		"left.png",
		"bottom.png",
		"top.png",
		"front.png",
		"back.png",
	};
	SkyboxData skybox = LoadSkybox("Assets/skybox", faces);
#pragma endregion

#pragma region Shader Loading

	//Load Skybox shader
	GLuint skyboxShaderProgram = LoadShaders("Shaders/skybox_vertex.shader", "Shaders/skybox_fragment.shader");

	GLuint gouraudShaderProgram = LoadShaders("Shaders/gouraud_vertex.shader", "Shaders/gouraud_fragment.shader");

	//Load shaders
	GLuint shaderProgram = LoadShaders("Shaders/vertex.shader", "Shaders/fragment.shader");
	glUseProgram(shaderProgram);

	GLuint phongShaderProgram = LoadShaders("Shaders/phong_vertex.shader", "Shaders/phong_fragment.shader");
	GLuint colorLoc = glGetUniformLocation(shaderProgram, "u_color");
	glUniform3f(colorLoc, 1.0f, 1.0f, 1.0f);


	// initialize MVP
	GLuint modelTransformLoc = glGetUniformLocation(shaderProgram, "u_model");
	GLuint viewLoc = glGetUniformLocation(shaderProgram, "u_view");
	GLuint projectionLoc = glGetUniformLocation(shaderProgram, "u_projection");

	//initialize normal transformation
	//initialize normal transformation
	GLuint normalTransformLoc = glGetUniformLocation(shaderProgram, "u_normal");
	GLuint cameraPosLoc = glGetUniformLocation(shaderProgram, "u_camera_pos");
	GLuint ambientColorLoc = glGetUniformLocation(shaderProgram, "u_ambient_color");
	glUniform3f(ambientColorLoc, 0.1f, 0.1f, 0.1f);
	glm::mat4 trans = glm::mat4(1.0f); // identity
	glm::mat4 trans2 = glm::mat4(1.0f); // identity
	glm::mat4 trans3 = glm::mat4(1.0f); // identity
	glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(trans));
	glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(trans2));

	// define projection matrix
	glm::mat4 projection = glm::mat4(1.0f);
	//glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));


	//setup light shading
	glm::vec3 lightPos = glm::vec3(0.0f, 3.0f, 0.0f);
	GLuint lightPosLoc = glGetUniformLocation(shaderProgram, "u_light_pos");
	GLuint lightDirLoc = glGetUniformLocation(shaderProgram, "u_light_dir");
	glUniform3f(lightPosLoc, 0.0f, 0.0f, 0.0f);
	glUniform3f(lightDirLoc, 0.0f, 0.0f, 1.0f);


#pragma endregion

	// set bg color to green
	glClearColor(0.5f, 0.5f, 0.5f, 0.0f);

	// var for rotations
	float rotFactor = 0.0f;
	float speed = 0.1f;
	float currentTime = glfwGetTime();
	float prevTime = 0.0f;
	float deltaTime = 0.0f;

	//depth testing
	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_ALWAYS); // set the depth test function

	//face culling
	glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK); // set which face to cull
	//glFrontFace(GL_CCW); // set the front face orientation

	glm::vec3 cameraPos = glm::vec3(0.0f, 3.0f, 10.0f);
	glm::vec3 position(0.f);
	glm::vec3 rotation(0.f);
	glm::vec3 scale(1.f);

	while (!glfwWindowShouldClose(window)) {

#pragma region Viewport
		float ratio;
		int width, height;

		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;

		glViewport(0, 0, width, height);

#pragma endregion

#pragma region Projection
		// Orthopgraphic projection but make units same as pixels. origin is lower left of window
		// projection = glm::ortho(0.0f, (GLfloat)width, 0.0f, (GLfloat)height, 0.1f, 10.0f); // when using this scale objects really high at pixel unity size

		// Orthographic with stretching
		//projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 10.0f);

		// Orthographic with corection for stretching, resize window to see difference with previous example
		//projection = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 0.1f, 10.0f);

		// Perspective Projection
		projection = glm::perspective(glm::radians(90.0f), ratio, 0.1f, 10.0f),

			// Set projection matrix in shader
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
#pragma endregion

#pragma region View
		glm::mat4 view = glm::lookAt(
			cameraPos,
			//glm::vec3(0.5f, 0.0f, -1.0f),
			glm::vec3(0.0f, 0.0f, 3.0f),
			glm::vec3(0.0f, 1.0f, -1.0f)
		);
		glUniform3f(cameraPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
#pragma endregion

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//toggle to render wit GL_FILL or GL_LINE
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

#pragma region Draw

		DrawSkybox(skybox, skyboxShaderProgram, view, projection);

		//draw bunny
		glBindVertexArray(earth.vaoId);
		glUseProgram(shaderProgram);

		// transforms
		// incerement rotation by deltaTime
		currentTime = glfwGetTime();
		deltaTime = currentTime - prevTime;
		rotFactor += deltaTime * 360.0f * speed;
		if (rotFactor > 360.0f) {
			rotFactor -= 360.0f;
		}
		prevTime = currentTime;

		trans = glm::mat4(1.0f); // identity
		//trans = glm::rotate(trans, glm::radians(rotFactor), glm::vec3(0.0f, 1.0f, 0.0f)); // matrix * rotation_matrix
		trans = glm::mat4(1.f);
		trans = glm::translate(trans, position);
		trans = glm::rotate(trans, glm::radians(rotation.x), glm::vec3(1.f, 0.f, 0.f));
		trans = glm::rotate(trans, glm::radians(rotFactor), glm::vec3(0.f, 1.f, 0.f));
		trans = glm::rotate(trans, glm::radians(rotation.z), glm::vec3(0.f, 0.f, 1.f));
		trans = glm::scale(trans, scale);
		//send to shader

		glm::mat4 normalTrans = glm::transpose(glm::inverse(trans));
		glUniformMatrix4fv(normalTransformLoc, 1, GL_FALSE, glm::value_ptr(normalTrans));
		glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(trans));

		glActiveTexture(GL_TEXTURE0);
		GLuint earthTexture = earth.textures[earth.materials[0].diffuse_texname];
		glBindTexture(GL_TEXTURE_2D, 1);

		//drawearth
		glDrawElements(GL_TRIANGLES, earth.numFaces, GL_UNSIGNED_INT, (void*)0);
		 
		//unbindtexture after rendering
		glBindTexture(GL_TEXTURE_2D, 0);

		//draw tourus
		glBindVertexArray(sun.vaoId);
		glUseProgram(gouraudShaderProgram);

		//glDrawElements(GL_TRIANGLES, sun.numFaces, GL_UNSIGNED_INT, (void*)0);
		normalTransformLoc = glGetUniformLocation(gouraudShaderProgram, "u_normal");
		cameraPosLoc = glGetUniformLocation(gouraudShaderProgram, "u_camera_pos");
		ambientColorLoc = glGetUniformLocation(gouraudShaderProgram, "u_ambient_color");
		trans2 = glm::mat4(1.0f); // identity
		//trans = glm::rotate(trans, glm::radians(rotFactor), glm::vec3(0.0f, 1.0f, 0.0f)); // matrix * rotation_matrix
		trans2 = glm::translate(trans2, glm::vec3(4.f, 1.f, 5.f));
		trans2 = glm::rotate(trans2, glm::radians(rotation.x), glm::vec3(1.f, 0.f, 0.f));
		trans2 = glm::rotate(trans2, glm::radians(rotFactor), glm::vec3(0.f, 1.f, 0.f));
		trans2 = glm::rotate(trans2, glm::radians(rotation.z), glm::vec3(0.f, 0.f, 1.0));
		trans2 = glm::scale(trans2, glm::vec3(0.25f, 0.25f, 0.25f));

		normalTrans = glm::transpose(glm::inverse(trans2));
		glUniformMatrix4fv(normalTransformLoc, 1, GL_FALSE, glm::value_ptr(normalTrans));
		glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(trans2));

		glActiveTexture(GL_TEXTURE0);
		GLuint sunTexture = sun.textures[sun.materials[0].diffuse_texname];
		glBindTexture(GL_TEXTURE_2D, 2);

		//drawearth
		glDrawElements(GL_TRIANGLES, sun.numFaces, GL_UNSIGNED_INT, (void*)0);

		//unbindtexture after rendering
		glBindTexture(GL_TEXTURE_2D, 0);

		//draw tourus
		glBindVertexArray(moon.vaoId);

		//glDrawElements(GL_TRIANGLES, sun.numFaces, GL_UNSIGNED_INT, (void*)0);

		trans3 = glm::mat4(1.0f); // identity
		//trans = glm::rotate(trans, glm::radians(rotFactor), glm::vec3(0.0f, 1.0f, 0.0f)); // matrix * rotation_matrix
		trans3 = glm::translate(trans3, glm::vec3(3.f, 1.f, 5.f));
		//trans3 = glm::rotate(trans3, glm::radians(rotation.x), glm::vec3(1.f, 0.f, 0.f));
		trans3 = glm::rotate(trans3, glm::radians(rotFactor), glm::vec3(0.f, 1.f, 0.f));
		//trans3 = glm::rotate(trans3, glm::radians(rotation.z), glm::vec3(0.f, 0.f, 1.0));
		trans3 = glm::scale(trans3, glm::vec3(0.05f, 0.05f, 0.05f));

		normalTrans = glm::transpose(glm::inverse(trans2));
		glUniformMatrix4fv(normalTransformLoc, 1, GL_FALSE, glm::value_ptr(normalTrans));
		glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(trans3));

		glActiveTexture(GL_TEXTURE0);
		GLuint moonTexture = moon.textures[moon.materials[0].diffuse_texname];
		glBindTexture(GL_TEXTURE_2D, 3);

		//drawearth
		glDrawElements(GL_TRIANGLES, moon.numFaces, GL_UNSIGNED_INT, (void*)0);

		//unbindtexture after rendering
		glBindTexture(GL_TEXTURE_2D, 0);
		//--- stop drawing here ---
#pragma endregion

		glfwSwapBuffers(window);
		//listen for glfw input events
		glfwPollEvents(); 

		updateKeyInput(window, &cameraPos, deltaTime);
		updateMouseInput(window, &mouse, deltaTime);
		updateLight(window, &lightPos, deltaTime);
	}
	return 0;
}