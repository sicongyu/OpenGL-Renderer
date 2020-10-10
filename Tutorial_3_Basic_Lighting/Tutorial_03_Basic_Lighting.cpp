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

#include <common/stb_image.h>

#define PREVIEW_CUBE 0
#define USE_IBL 1

#define CUBEMAP_WIDTH 1024

GLFWwindow* window;

void error_callback(int error, const char* description)
{
    puts(description);
}


void RenderCube(Mesh& cube_mesh) {

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, cube_mesh.pos_id);
	glVertexAttribPointer(
		0,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	glDrawArrays(GL_TRIANGLES, 0, cube_mesh.vertex_size);
	glDisableVertexAttribArray(0);
}

unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void RenderCube1() {
	// initialize (if necessary)
	if (cubeVAO == 0)
	{
		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			// right face
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
			// bottom face
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			// top face
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
			 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// link vertex attributes
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
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
	glClearColor(0.227f, 0.227f, 0.227f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	//glDepthFunc(GL_LESS);
	glDepthFunc(GL_LEQUAL); // set depth function to less than AND equal for skybox depth trick.

	// Cull triangles which normal is not towards the camera
	//glEnable(GL_CULL_FACE);

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

	Mesh m1911("models/m1911/source/M1911.obj", true);
	m1911.LoadTexture(Diffuse,"models/m1911/textures/M1911Dis_Material_AlbedoTransparency.png");
	m1911.LoadTexture(Normal_Map, "models/m1911/textures/M1911Dis_Material_NormalOpenGL.png");
	m1911.LoadTexture(Displace, "models/m1911/textures/M1911Dis_Material_AO.png");
	m1911.LoadTexture(Roughness, "models/m1911/textures/M1911Dis_Material_Roughness.png");
	m1911.LoadTexture(Metallic, "models/m1911/textures/M1911Dis_Material_MetallicSmoothness.png");

	// Get a handle for our "LightPosition" uniform
	glUseProgram(programID);
	GLuint LightID = glGetUniformLocation(programID, "LightPos");
	GLuint CameraID = glGetUniformLocation(programID, "CameraPos");

#if PREVIEW_CUBE || USE_IBL
	GLuint equirectangularToCubeProgramID = LoadShaders("shaders/Cube.vertexshader", "shaders/Cube.fragmentshader");
	GLuint cubemapProgramID = LoadShaders("shaders/Cubemap.vertexshader", "shaders/Cubemap.fragmentshader");

	GLuint E2CViewMatrixID = glGetUniformLocation(equirectangularToCubeProgramID, "V");
	GLuint E2CProjectionMatrixID = glGetUniformLocation(equirectangularToCubeProgramID, "P");
	GLuint E2CEnvMapID = glGetUniformLocation(equirectangularToCubeProgramID, "equirectangularMap");

	GLuint CubemapViewMatrixID = glGetUniformLocation(cubemapProgramID, "V");
	GLuint CubemapProjectionMatrixID = glGetUniformLocation(cubemapProgramID, "P");
	GLuint CubemapEnvMapID = glGetUniformLocation(cubemapProgramID, "environmentMap");

	// Load HDR Environment Mapping Image for IBL
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrComponents;
	float *data = stbi_loadf("models/PaperMill_Ruins_E/PaperMill_E_3k.hdr", &width, &height, &nrComponents, 0);
	unsigned int hdrTexture;
	if (data)
	{
		glGenTextures(1, &hdrTexture);
		glBindTexture(GL_TEXTURE_2D, hdrTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		printf("Failed to load HDR image.");
		//std::cout << "Failed to load HDR image." << std::endl;
	}


	static const GLfloat g_vertex_buffer_data[] = {
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f
	};
	Mesh cube_mesh;
	cube_mesh.setAtrribute(Pos, (void *)g_vertex_buffer_data, sizeof(g_vertex_buffer_data));
	cube_mesh.vertex_size = sizeof(g_vertex_buffer_data) / sizeof(float) / 3;
	cube_mesh.face_size = cube_mesh.vertex_size / 3;
#endif

#if USE_IBL
	unsigned int captureFBO, captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, CUBEMAP_WIDTH, CUBEMAP_WIDTH);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

	unsigned int envCubemap;
	glGenTextures(1, &envCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	for (unsigned int i = 0; i < 6; i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, CUBEMAP_WIDTH, CUBEMAP_WIDTH, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Projection equirectangular map to six faces of cube
	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
	};

	// convert HDR equirectangular environment map to cubemap equivalent
	glUseProgram(equirectangularToCubeProgramID);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrTexture);
	glUniform1i(E2CEnvMapID, 0);

	glUniformMatrix4fv(E2CProjectionMatrixID, 1, GL_FALSE, &captureProjection[0][0]);

	glViewport(0, 0, CUBEMAP_WIDTH, CUBEMAP_WIDTH);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	for (unsigned int i = 0; i < 6; i++) {
		glUniformMatrix4fv(E2CViewMatrixID, 1, GL_FALSE, &captureViews[i][0][0]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		RenderCube(cube_mesh);
		//RenderCube1();
	}
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");

	//glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// configure the viewport to the original framebuffer's screen dimensions
	int scrWidth, scrHeight;
	glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
	glViewport(0, 0, scrWidth, scrHeight);

	// Initialize Cubemap Shader
	//glUseProgram(cubemapProgramID);
	//glUseProgram(cubemapProgramID);
	//glm::mat4 CubemapProjectionMatrix = getProjectionMatrix();
	//glUniformMatrix4fv(CubemapProjectionMatrixID, 1, GL_FALSE, &CubemapProjectionMatrix[0][0]);
#endif
	
	float angle = 0.0;
	float lastTime = 0.0f;
	float distance = 0.0f;

	auto process_inpute = [&lastTime](float & angle, float & distance, float curTime)
	{
		if (lastTime == 0.0f)
			lastTime = curTime;
		auto delta = curTime - lastTime;
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
			angle += 40.0f * delta;
		else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		{
			angle -= 40.0f * delta;
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
		//glPatchParameteri(GL_PATCH_VERTICES, 3);

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

		glm::vec3 lightPos = glm::vec3(0, 4, 4);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
		glm::vec3 cameraPos = getPosition();
		glUniform3f(CameraID, cameraPos.x, cameraPos.y, cameraPos.z);

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

		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, cur_mesh->tangent_id);
		glVertexAttribPointer(
			3,                                // attribute
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		glEnableVertexAttribArray(4);
		glBindBuffer(GL_ARRAY_BUFFER, cur_mesh->binormal_id);
		glVertexAttribPointer(
			4,                                // attribute
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Draw the triangles !

		//glDrawArrays(GL_TRIANGLES, 0, cur_mesh->vertex_size);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cur_mesh->index_id);
		glDrawElements(GL_TRIANGLES, cur_mesh->face_size * 3, GL_UNSIGNED_SHORT, (void *)0);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(3);
		glDisableVertexAttribArray(4);

#if PREVIEW_CUBE
		//  --- sky box pass ---
		glUseProgram(equirectangularToCubeProgramID);

		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, hdrTexture);
		glUniform1i(E2CEnvMapID, 5);

		glUniformMatrix4fv(E2CProjectionMatrixID, 1, GL_FALSE, &ProjectionMatrix[0][0]);
		glUniformMatrix4fv(E2CViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

		RenderCube(cube_mesh);
#endif

#if USE_IBL

		// convert HDR equirectangular environment map to cubemap equivalent
		glUseProgram(equirectangularToCubeProgramID);

		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, hdrTexture);
		glUniform1i(E2CEnvMapID, 5);

		//glUniformMatrix4fv(E2CProjectionMatrixID, 1, GL_FALSE, &captureProjection[0][0]);
		glUniformMatrix4fv(E2CProjectionMatrixID, 1, GL_FALSE, &ProjectionMatrix[0][0]);

		glViewport(0, 0, CUBEMAP_WIDTH, CUBEMAP_WIDTH);
		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		for (unsigned int i = 0; i < 6; i++) {
			glUniformMatrix4fv(E2CViewMatrixID, 1, GL_FALSE, &captureViews[i][0][0]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			RenderCube(cube_mesh);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// configure the viewport to the original framebuffer's screen dimensions
		int scrWidth, scrHeight;
		glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
		glViewport(0, 0, scrWidth, scrHeight);

		glUseProgram(cubemapProgramID);
		glUniformMatrix4fv(CubemapProjectionMatrixID, 1, GL_FALSE, &ProjectionMatrix[0][0]);
		glUniformMatrix4fv(CubemapViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
		glUniform1i(CubemapEnvMapID, 6);
		RenderCube(cube_mesh);
#endif

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
	glDeleteProgram(equirectangularToCubeProgramID);
	glDeleteProgram(cubemapProgramID);


	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}


