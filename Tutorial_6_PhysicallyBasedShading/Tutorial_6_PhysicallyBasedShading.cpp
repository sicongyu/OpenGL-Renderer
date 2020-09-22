// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <AntTweakBar.h>

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

extern glm::vec3 position;
extern float horizontalAngle;

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
	glfwWindowHint(GLFW_SAMPLES, 4);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1024, 768, "Tutorial 03", NULL, NULL);
	if (window == NULL)
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

	glfwSetWindowTitle(window, "Tutorial 03");

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
	//glEnable(GL_CULL_FACE);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("shaders/pbr.vertexshader",
		"shaders/pbr.fragmentshader");
	GLuint skyboxProgramID = LoadShaders("shaders/skybox.vertexshader",
		"shaders/skybox.fragmentshader");

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");
	GLuint camera_positionId = glGetUniformLocation(programID, "camera_position");
	// for pbr
	GLuint envMapID = glGetUniformLocation(programID, "env_map");
	GLuint envMapFilteredID = glGetUniformLocation(programID, "env_map_filtered");
	GLuint IntegrateBRDFSamplerID = glGetUniformLocation(programID, "IntegrateBRDFSampler");
	GLuint roughnessID = glGetUniformLocation(programID, "roughness");
	GLuint indexID = glGetUniformLocation(programID, "modelIndex");

	// Get a handle for our "myTextureSampler" uniform
	GLuint diffuseID = glGetUniformLocation(programID, "diffuseTex");
	GLuint roughnessTexID = glGetUniformLocation(programID, "roughnessTex");
	GLuint metallicID = glGetUniformLocation(programID, "metallicTex");

	// sky box uniforms
	GLuint skyBoxID = glGetUniformLocation(skyboxProgramID, "env_map");
	GLuint viewProjInvMatId = glGetUniformLocation(skyboxProgramID, "view_proj_inv_mat");

	// --- quad ---
	Mesh quad("models/quad.obj");

	// --- monkey ---
	Mesh monkey("models/suzanne.obj");
	monkey.LoadTexture(Diffuse, "models/uvmap.DDS");

	// --- ninja ---
	Mesh ninja_head("models/ninjaHead_Low_smooth_po.obj");
	ninja_head.LoadTexture(Diffuse, "models/ninja_head_AO.bmp");

	// --- shaver ---
	Mesh shaver("models/Electric_shaver_GEO.obj");
	shaver.LoadTexture(Diffuse, "models/color.jpg");
	shaver.LoadTexture(Roughness, "models/roughness.jpg");
	shaver.LoadTexture(Metallic, "models/metallic.jpg");

	// --- sphere ---
	Mesh sphere("models/sphere0.obj");

	// --- M1911 ---
	Mesh pistol("models/m1911/source/M1911.obj");
	pistol.LoadTexture(Diffuse, "models/m1911/textures/M1911Dis_Material_AO.png");
	pistol.LoadTexture(Roughness, "models/m1911/textures/M1911Dis_Material_Roughness");
	pistol.LoadTexture(Metallic, "models/m1911/textures/M1911Dis_Material_MetallicSmoothness");

	Mesh* meshes[5] = { &shaver, &ninja_head,  &monkey, &sphere, &pistol};

	// load pbr resources
	vector<string> paths = GenCubemapList("models/pbr/pisa_c0", ".hdr");
	GLuint env_map = CreateTextureCubeMap_MipMap(paths);
	// env map filtered
	paths = GenCubemapList("models/pbr/filter/pisa_d_c0", ".hdr");
	int w, h;
	GLuint env_map_filtered = CreateTextureCubeMap(paths, w, h);
	GLuint integrateBRDF = loadTGA("models/pbr/convolution_spec.tga");

	// Get a handle for our "LightPosition" uniform
	glUseProgram(programID);
	GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

	int meshIdx = 0;
	auto f = [&meshIdx, meshes](Mesh * & cur_mesh)
	{
		if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
			meshIdx = (meshIdx + 1) % 3;
		else if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
			meshIdx = (meshIdx + 2) % 3;
		cur_mesh = meshes[meshIdx];
	};

	float roughness = 0.5f;
	float lastTime = 0.0f;
	auto process_inpute = [&lastTime](float & roughness, float curTime)
	{
		if (lastTime == 0.0f)
			lastTime = curTime;
		auto delta = curTime - lastTime;
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
			roughness += 0.01f;
			if (roughness > 0.8f)
				roughness = 0.8f;
		}
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
			roughness -= 0.01f;
			if (roughness < 0.0f)
				roughness = 0.0f;
		}

		lastTime = curTime;
	};

	//////////////////////////////////////////////////////////////////////////
	//gui
	// Initialize AntTweakBar
	#ifndef WIN32
		TwWindowSize(1024 * 2, 768 * 2);
	#else
		TwWindowSize(1024, 768);
	#endif
		TwInit(TW_OPENGL_CORE, NULL);

		// Create a tweak bar
		TwBar * bar = TwNewBar("TweakBar");
		TwDefine(" GLOBAL help='This example shows how to integrate AntTweakBar with GLFW and OpenGL. Array Key Move Camera' "); // Message added to the help bar.

		int index = 4;

		TwEnumVal shape[5] = { { 0,"shaver" }, { 1, "NinjaHead" },{2, "monkey"}, {3, "sphere"}, {4, "Pistol"} };
		TwType shapeType = TwDefineEnum("Model Type", shape, 5);
		TwAddVarRW(bar, "Model", shapeType, &index, " keyIncr='N' keyDecr='C' help='Change object shape.' ");

			// Set GLFW event callbacks
			// - Redirect window size changes to the callback function WindowSizeCB
			//glfwSetWindowSizeCallback(window, (GLFWwindowsizefun)WindowSizeCB);
			// - Directly redirect GLFW mouse button events to AntTweakBar
		glfwSetMouseButtonCallback(window, (GLFWmousebuttonfun)TwEventMouseButtonGLFW);
		// - Directly redirect GLFW mouse position events to AntTweakBar
		glfwSetCursorPosCallback(window, (GLFWcursorposfun)TwEventMousePosGLFW);
		// - Directly redirect GLFW mouse wheel events to AntTweakBar
		glfwSetScrollCallback(window, (GLFWscrollfun)TwEventMouseWheelGLFW);
		// - Directly redirect GLFW key events to AntTweakBar
		glfwSetKeyCallback(window, (GLFWkeyfun)TwEventKeyGLFW);
		// - Directly redirect GLFW char events to AntTweakBar
		glfwSetCharCallback(window, (GLFWcharfun)TwEventCharGLFW);

	Mesh * cur_mesh = &ninja_head;
	horizontalAngle = 0.0f;
	position = glm::vec3(0, 0, -15);
	do {
		cur_mesh = meshes[index];
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glEnable(GL_STENCIL_TEST);
		

		// Use our shader
		glUseProgram(programID);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);

		// Compute the MVP matrix from keyboard and mouse input

		process_inpute(roughness, glfwGetTime());
		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();
		glm::mat4 ModelMatrix = glm::rotate(glm::mat4(1.0), 180.0f, glm::vec3(0, 1, 0));
		glm::mat4 ShaverTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(0, -5, -1));
		glm::mat4 PistolTranslate = glm::scale(glm::mat4(1.0), glm::vec3(0.005, 0.005, 0.005));
		PistolTranslate = glm::rotate(PistolTranslate, -90.0f, glm::vec3(1, 0, 0));
		if (index == 0) // shaver
			ModelMatrix *= ShaverTranslate;
		else if (index == 4)
			ModelMatrix *= PistolTranslate;
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

		glm::vec3 lightPos = glm::vec3(4, 4, 4);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
		glm::vec3 camera_pos = getPosition();
		glUniform3f(camera_positionId, camera_pos.x, camera_pos.y, camera_pos.z);
		float r = roughness;
		glUniform1f(roughnessID, r);
		glUniform1i(indexID, index);

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cur_mesh->diffuse_texture_id);
		glUniform1i(diffuseID, 0);
		// for pbr
		// env map 
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, env_map);
		glUniform1i(envMapID, 1);
		// env map filtered
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_CUBE_MAP, env_map_filtered);
		glUniform1i(envMapFilteredID, 2);
		// brdf
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, integrateBRDF);
		glUniform1i(IntegrateBRDFSamplerID, 3);
		// roughenss & metallic
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, cur_mesh->roughness_texture_id);
		glUniform1i(roughnessTexID, 4);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, cur_mesh->metallic_texture_id);
		glUniform1i(metallicID, 5);

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
		if (index == 3)
		{
			for (int i = 0; i < 8; ++i)
			{ 
				auto newModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.3));
				newModelMatrix = glm::translate(newModelMatrix, glm::vec3( i*2.5 - 8 ,0,0));
				glm::mat4 MVP = ProjectionMatrix * ViewMatrix * newModelMatrix;
				r = i * 1.0f / 8;
				glUniform1f(roughnessID, r);
				glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
				glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &newModelMatrix[0][0]);
				glDrawArrays(GL_TRIANGLES, 0, cur_mesh->vertex_size);
			}
			
		}
		else
			glDrawArrays(GL_TRIANGLES, 0, cur_mesh->vertex_size);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		
		// --- sky box pass ---
		glUseProgram(skyboxProgramID);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glStencilFunc(GL_EQUAL, 0, 0xFF);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, env_map);
		glUniform1i(skyBoxID, 1);

		glm::mat3 viewRotate3X3 = glm::mat3(ViewMatrix);
		glm::mat4 viewRotate4X4 = glm::mat4(viewRotate3X3);
		glm::mat4 VP_Inverse = ProjectionMatrix * viewRotate4X4;
		VP_Inverse = glm::inverse(VP_Inverse);
		glUniformMatrix4fv(viewProjInvMatId, 1, GL_FALSE, &VP_Inverse[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, quad.pos_id);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		glDrawArrays(GL_TRIANGLES, 0, quad.vertex_size);
		glDisableVertexAttribArray(0);

		// Swap buffers
		TwDraw();
		glfwSwapBuffers(window);
		// Poll for and process events
		glfwPollEvents();

		glfwSetErrorCallback(error_callback);
	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		!glfwWindowShouldClose(window));

	// Cleanup VBO and shader

	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}


