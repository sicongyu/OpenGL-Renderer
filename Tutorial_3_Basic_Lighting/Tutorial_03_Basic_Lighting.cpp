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
#define USE_BLOOM 1

#define CUBEMAP_WIDTH 1024

#define SCR_WIDTH 1024
#define SCR_HEIGHT 768

GLFWwindow* window;

Mesh* cube_mesh;

Mesh* quad_mesh;

GLuint equirectangularToCubeProgramID;

unsigned int captureFBO, captureRBO;
unsigned int quadVAO, quadVBO;

enum Model {M1911, Cerberus};

static const glm::mat4 captureProjection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
static const glm::mat4 captureViews[] =
{
	glm::lookAt(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
	glm::lookAt(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
	glm::lookAt(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
	glm::lookAt(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f))
};

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

static const GLfloat quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	// positions		 // texCoords
	-1.0f,  1.0f,  0.0f, //0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, //0.0f, 0.0f,
	1.0f,  -1.0f,  0.0f, //1.0f, 0.0f,
				  		 //
	-1.0f,  1.0f,  0.0f, //0.0f, 1.0f,
	1.0f,  -1.0f,  0.0f, //1.0f, 0.0f,
	1.0f,   1.0f,  0.0f  //1.0f, 1.0f
};

static const GLfloat quadTexs[] = {
	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f,

	0.0f, 1.0f,
	1.0f, 0.0f,
	1.0f, 1.0f
};

void error_callback(int error, const char* description)
{
    puts(description);
}

void RenderQuad() {
	//glBindVertexArray(quadVAO);
	//glBindTexture(GL_TEXTURE_2D, textureColorbuffer);	// use the color attachment texture as the texture of the quad plane
	//glDrawArrays(GL_TRIANGLES, 0, 6);
	//printf("Start rendering quad.");
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, quad_mesh->pos_id);
	glVertexAttribPointer(
		0,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		3 * sizeof(float),                  // stride
		(void*)0            // array buffer offset
	);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, quad_mesh->uv_id);
	glVertexAttribPointer(
		1,                  // attribute
		2,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		2 * sizeof(float),                  // stride
		(void*)0            // array buffer offset
	);
	glDrawArrays(GL_TRIANGLES, 0, quad_mesh->vertex_size);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void RenderCube() {

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, cube_mesh->pos_id);
	glVertexAttribPointer(
		0,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	glDrawArrays(GL_TRIANGLES, 0, cube_mesh->vertex_size);
	glDisableVertexAttribArray(0);
}

unsigned int LoadImageAndConvertToCubemap(const char* path) {

	GLuint E2CViewMatrixID = glGetUniformLocation(equirectangularToCubeProgramID, "V");
	GLuint E2CProjectionMatrixID = glGetUniformLocation(equirectangularToCubeProgramID, "P");
	GLuint E2CEnvMapID = glGetUniformLocation(equirectangularToCubeProgramID, "equirectangularMap");

	// Load HDR Environment Mapping Image for IBL
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrComponents;
	float *data = stbi_loadf(path, &width, &height, &nrComponents, 0);
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
	}

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
	//glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

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
		RenderCube();
		//RenderCube1();
	}
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// configure the viewport to the original framebuffer's screen dimensions
	int scrWidth, scrHeight;
	glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
	glViewport(0, 0, scrWidth, scrHeight);

	return envCubemap;
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
	glfwWindowHint(GLFW_SAMPLES, 4); // Hint MSAA

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

	 // Explicitly declare enable MSAA
	glEnable(GL_MULTISAMPLE);


	// Cull triangles which normal is not towards the camera
	//glEnable(GL_CULL_FACE);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
#if USE_IBL
	GLuint programID = LoadShaders("shaders/myPBR.vertexshader",
		"shaders/IBL.fragmentshader");
#else
	GLuint programID = LoadShaders("shaders/myPBR.vertexshader", 
								"shaders/myPBR.fragmentshader");
#endif

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
#if USE_IBL
	GLuint IrradianceTexID = glGetUniformLocation(programID, "irradianceMap");
	GLuint SpecularTexID = glGetUniformLocation(programID, "prefilterMap");
	GLuint BRDFTexID = glGetUniformLocation(programID, "brdfLUT");
#endif

	//Mesh monkey("models/suzanne.obj");
	//monkey.LoadTexture(Diffuse,"models/uvmap.DDS");


	Mesh* cur_mesh;
	Model cur_model = M1911;
	switch (cur_model)
	{
	case M1911: {
		Mesh* m1911 = new Mesh("models/m1911/source/M1911.obj", true);
		m1911->LoadTexture(Diffuse, "models/m1911/textures/M1911Dis_Material_AlbedoTransparency.png");
		m1911->LoadTexture(Normal_Map, "models/m1911/textures/M1911Dis_Material_NormalOpenGL.png");
		m1911->LoadTexture(Displace, "models/m1911/textures/M1911Dis_Material_AO.png");
		m1911->LoadTexture(Roughness, "models/m1911/textures/M1911Dis_Material_Roughness.png");
		m1911->LoadTexture(Metallic, "models/m1911/textures/M1911Dis_Material_MetallicSmoothness.png");
		cur_mesh = m1911;
		break;
		}
	case Cerberus: {
		Mesh* cerberus = new Mesh("models/Cerberus_by_Andrew_Maximov/Cerberus_LP.obj", true);
		cerberus->LoadTexture(Diffuse, "models/Cerberus_by_Andrew_Maximov/Textures/Cerberus_A.tga");
		cerberus->LoadTexture(Normal_Map, "models/Cerberus_by_Andrew_Maximov/Textures/Cerberus_N.tga");
		cerberus->LoadTexture(Displace, "models/Cerberus_by_Andrew_Maximov/Textures/RAW/Cerberus_AO.tga");
		cerberus->LoadTexture(Roughness, "models/Cerberus_by_Andrew_Maximov/Textures/Cerberus_R.tga");
		cerberus->LoadTexture(Metallic, "models/Cerberus_by_Andrew_Maximov/Textures/Cerberus_M.tga");
		cur_mesh = cerberus;
		break;
		}
	default:
		break;
	}

	// Get a handle for our "LightPosition" uniform
	glUseProgram(programID);
	GLuint LightID = glGetUniformLocation(programID, "LightPos");
	GLuint CameraID = glGetUniformLocation(programID, "CameraPos");

#if PREVIEW_CUBE || USE_IBL

	// create vertex data for cube rendering
	cube_mesh = new Mesh();
	cube_mesh->setAtrribute(Pos, (void *)g_vertex_buffer_data, sizeof(g_vertex_buffer_data));
	cube_mesh->vertex_size = sizeof(g_vertex_buffer_data) / sizeof(float) / 3;
	cube_mesh->face_size = cube_mesh->vertex_size / 3;

	equirectangularToCubeProgramID = LoadShaders("shaders/Cube.vertexshader", "shaders/Cube.fragmentshader");
#endif

#if USE_IBL

	GLuint cubemapProgramID = LoadShaders("shaders/Cubemap.vertexshader", "shaders/Cubemap.fragmentshader");

	GLuint CubemapViewMatrixID = glGetUniformLocation(cubemapProgramID, "V");
	GLuint CubemapProjectionMatrixID = glGetUniformLocation(cubemapProgramID, "P");
	GLuint CubemapEnvMapID = glGetUniformLocation(cubemapProgramID, "environmentMap");

	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	//unsigned int EnvCubemap = LoadImageAndConvertToCubemap("models/Old_Industrial_Hall/fin4_Bg.jpg");
	unsigned int EnvCubemap = LoadImageAndConvertToCubemap("models/PaperMill_Ruins_E/PaperMill_E_8k.jpg");

	//unsigned int IrradianceMap = LoadImageAndConvertToCubemap("models/Old_Industrial_Hall/fin4_Env.hdr");
	unsigned int IrradianceMap = LoadImageAndConvertToCubemap("models/PaperMill_Ruins_E/PaperMill_E_Env.hdr");

	unsigned int ReflectionMap = LoadImageAndConvertToCubemap("models/PaperMill_Ruins_E/PaperMill_E_3k.hdr");

	GLuint integrateBRDF = loadTGA("models/pbr/convolution_spec.tga");


	// Prefilter Specular Map
	GLuint prefilterProgramID = LoadShaders("shaders/Cube.vertexshader", "shaders/Specular.fragmentshader");

	GLuint PrefilterViewMatrixID = glGetUniformLocation(prefilterProgramID, "V");
	GLuint PrefilterProjectionMatrixID = glGetUniformLocation(prefilterProgramID, "P");
	GLuint PrefilterEnvMapID = glGetUniformLocation(prefilterProgramID, "environmentMap");
	GLuint PrefilterRoughnessID = glGetUniformLocation(prefilterProgramID, "roughness");
	
	unsigned int prefilterMap;
	glGenTextures(1, &prefilterMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	glUseProgram(prefilterProgramID);
	glUniformMatrix4fv(PrefilterProjectionMatrixID, 1, GL_FALSE, &captureProjection[0][0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, ReflectionMap);
	glUniform1i(PrefilterEnvMapID, 0);


	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	unsigned int maxMipLevels = 5;
	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{
		// reisze framebuffer according to mip-level size.
		unsigned int mipWidth = 128 * std::pow(0.5, mip);
		unsigned int mipHeight = 128 * std::pow(0.5, mip);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		glUniform1f(PrefilterRoughnessID, roughness);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glUniformMatrix4fv(PrefilterViewMatrixID, 1, GL_FALSE, &captureViews[i][0][0]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			RenderCube();
		}
	}

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	int scrWidth, scrHeight;
	glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
	glViewport(0, 0, scrWidth, scrHeight);


#endif

#if USE_BLOOM

	// create VAO for quad rendering
	quad_mesh = new Mesh();
	quad_mesh->setAtrribute(Pos, (void *)quadVertices, sizeof(quadVertices));
	quad_mesh->setAtrribute(Uv, (void *)quadTexs, sizeof(quadTexs));
	quad_mesh->vertex_size = sizeof(quadVertices) / sizeof(float) / 3;
	quad_mesh->face_size = 2;
	printf("Quad VAO data generated.\n");

	GLuint screenProgramID = LoadShaders("shaders/screen.vertexshader", "shaders/screen.fragmentshader");
	GLuint screenTexID = glGetUniformLocation(screenProgramID, "screenTexture");

	printf("Bloom shader started.\n");

	GLuint hdrFBO;
	glGenFramebuffers(1, &hdrFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

	printf("Frame buffer generated.\n");

	//GLuint colorBuffer;
	//glGenTextures(1, &colorBuffer);
	//glBindTexture(GL_TEXTURE_2D, colorBuffer);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glBindTexture(GL_TEXTURE_2D, 0);

	//glFramebufferTexture2D(
	//	GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0
	//);

	printf("Colorbuffer generated.\n");

	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
	// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");

	printf("Render buffer generated. Frame buffer complete.\n");

	GLuint colorBuffers[2];
	glGenTextures(2, colorBuffers);
	for (GLuint i = 0; i < 2; i++) {
		glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0
		);
	}
	GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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


	do{
		//f(cur_mesh);
#if USE_BLOOM
		glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
		glEnable(GL_DEPTH_TEST);
#endif
		//glDrawElements(GL_TRIANGLES, cur_mesh->face_size * 3, GL_UNSIGNED_SHORT, (void *)0);

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

#if USE_IBL

		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_CUBE_MAP, IrradianceMap);
		glUniform1i(IrradianceTexID, 5);

		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
		glUniform1i(SpecularTexID, 6);

		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, integrateBRDF);
		glUniform1i(BRDFTexID, 7);

#endif // USE_IBL


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

		glUseProgram(cubemapProgramID);
		glUniformMatrix4fv(CubemapProjectionMatrixID, 1, GL_FALSE, &ProjectionMatrix[0][0]);
		glUniformMatrix4fv(CubemapViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, EnvCubemap);
		//glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
		glUniform1i(CubemapEnvMapID, 0);
		RenderCube();
#endif

#if USE_BLOOM
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_STENCIL_TEST);
		glUseProgram(screenProgramID);
		//glBindVertexArray(quadVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorBuffers[1]);
		//glBindTexture(GL_TEXTURE_2D, colorBuffer);
		glUniform1i(screenTexID, 0);
		RenderQuad();
		//glDrawArrays(GL_TRIANGLES, 0, 6);

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
	glDeleteProgram(prefilterProgramID);
	glDeleteProgram(screenProgramID);

	delete cur_mesh;
	delete cube_mesh;
	delete quad_mesh;

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}


