// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>
#include <common/text2D.hpp>

#include <common/model.hpp>

GLFWwindow* window;

void error_callback(int error, const char* description)
{
    puts(description);
}

int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "Tutorial 03", NULL, NULL);
	if (window==NULL)
	{
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	glfwSetWindowTitle(window,"Tutorial 03");

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetCursorPos(window, 1024 / 2, 768 / 2);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("shaders/myPBR.vertexshader", 
								"shaders/myPBR.fragmentshader");

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");

	// Get a handle for our "myTextureSampler" uniform
	GLuint AlbedoTexID = glGetUniformLocation(programID, "albedoMap");
	GLuint NormalTexID = glGetUniformLocation(programID, "normalMap");
	GLuint MetallicTexID = glGetUniformLocation(programID, "metallicMap");
	GLuint RoughnessTexID = glGetUniformLocation(programID, "roughnessMap");
	GLuint AOTexID = glGetUniformLocation(programID, "AOMap");

	//Mesh monkey("models/suzanne.obj");
	//monkey.LoadTexture(Diffuse,"models/uvmap.DDS");

	Mesh m1911("models/m1911/source/M1911.obj");
	m1911.LoadTexture(Diffuse,"models/m1911/textures/M1911Dis_Material_AlbedoTransparency.png");
	m1911.LoadTexture(Normal_Map, "models/m1911/textures/M1911Dis_Material_NormalOpenGL.png");
	m1911.LoadTexture(Displace, "models/m1911/textures/M1911Dis_Material_AO.png");
	m1911.LoadTexture(Roughness, "models/m1911/textures/M1911Dis_Material_Roughness.png");
	m1911.LoadTexture(Metallic, "models/m1911/textures/M1911Dis_Material_MetallicSmoothness.png");

	// Get a handle for our "LightPosition" uniform
	glUseProgram(programID);
	GLuint LightID = glGetUniformLocation(programID, "LightPos");

	//auto f = [&ninja_head, &monkey](Mesh * & cur_mesh)
	//{
	//	if (glfwGetKey(window,GLFW_KEY_N) == GLFW_PRESS)
	//		cur_mesh = &ninja_head;
	//	else if (glfwGetKey(window,GLFW_KEY_C) == GLFW_PRESS)
	//	{
	//		cur_mesh = &monkey;
	//	}
	//};
	
	float angle = 0.0;
	float lastTime = 0.0f;
	float distance = 0.0f;

	auto process_inpute = [&lastTime](float & angle, float & distance, float curTime)
	{
		if (lastTime == 0.0f)
			lastTime = curTime;
		auto delta = curTime - lastTime;
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
			angle += 20.0f * delta;
		else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		{
			angle -= 20.0f * delta;
		}
		//if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		//	distance += 20.0f * delta;
		//else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		//{
		//	distance -= 20.0f * delta;
		//}

		lastTime = curTime;
	};

	Mesh * cur_mesh = &m1911;
	do{
		//f(cur_mesh);
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		// Compute the MVP matrix from keyboard and mouse input
		process_inpute(angle, distance, glfwGetTime());
		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();
		glm::mat4 ModelMatrix = glm::mat4(1.0);
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.001, 0.001, 0.001));
		//ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0, 0, distance));
		ModelMatrix = glm::rotate(ModelMatrix, -90.0f, glm::vec3(1, 0, 0));
		ModelMatrix = glm::rotate(ModelMatrix, angle, glm::vec3(0, 0, 1));
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

		glm::vec3 lightPos = glm::vec3(4, 4, 4);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

		// Bind our textures in Texture Unit 
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cur_mesh->diffuse_texture_id);
		glUniform1i(AlbedoTexID, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, cur_mesh->normal_texture_id);
		glUniform1i(NormalTexID, 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, cur_mesh->displace_texture_id);
		glUniform1i(AOTexID, 2);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, cur_mesh->roughness_texture_id);
		glUniform1i(RoughnessTexID, 3);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, cur_mesh->metallic_texture_id);
		glUniform1i(MetallicTexID, 4);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, cur_mesh->pos_id);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, cur_mesh->uv_id);
		glVertexAttribPointer(
			1,                                // attribute
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
			);

		// 3rd attribute buffer : normals
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, cur_mesh->normal_id);
		glVertexAttribPointer(
			2,                                // attribute
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
			);

		// Draw the triangles !
		glDrawArrays(GL_TRIANGLES, 0, cur_mesh->vertex_size);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		//printText2D("Arrow Key move camera ,LSHIFT Speed", 10, 10, 20);
		//printText2D("0 NinjaHead", 10, 50, 20);
		//printText2D("1 Cube", 10, 80, 20);
		// Swap buffers
		glfwSwapBuffers(window);
		// Poll for and process events
		glfwPollEvents();
		
		glfwSetErrorCallback(error_callback);
	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window,GLFW_KEY_ESCAPE) != GLFW_PRESS && 
		!glfwWindowShouldClose(window));

	// Cleanup VBO and shader

	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

