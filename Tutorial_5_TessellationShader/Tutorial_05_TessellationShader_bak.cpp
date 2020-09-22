// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
//#include <AntTweakBar.h>

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


void FillGrid_Indexed_WithTangentSpace( int dwWidth, int dwLength,
	float fGridSizeX, float fGridSizeZ,std::vector<glm::vec3> & positions,std::vector<vec2> & uvs,
	std::vector<glm::vec3> & normals, std::vector<glm::vec3> & binormals, std::vector<glm::vec3> & tangents,
	std::vector<unsigned short> & indices)
{
	
	int   nNumVertex = (dwWidth + 1) * (dwLength + 1);
	int   nNumIndex = 3 * 2 * dwWidth * dwLength;
	float   fStepX = fGridSizeX / dwWidth;
	float   fStepZ = fGridSizeZ / dwLength;

	// Allocate memory for buffer of vertices in system memory
	//TANGENTSPACEVERTEX*    pVertexBuffer = new TANGENTSPACEVERTEX[nNumVertex];
	//TANGENTSPACEVERTEX*    pVertex = &pVertexBuffer[0];
	positions.resize(nNumVertex);
	uvs.resize(nNumVertex);
	normals.resize(nNumVertex);
	binormals.resize(nNumVertex);
	tangents.resize(nNumVertex);


	// Fill vertex buffer
	for (int i = 0; i <= dwLength; ++i)
	{
		for (int j = 0; j <= dwWidth; ++j)
		{
			int index = i * (dwWidth+1) + j;
			positions[index].x = -fGridSizeX / 2.0f + j*fStepX;
			positions[index].y = -8.0f;
			positions[index].z = fGridSizeZ / 2.0f - i*fStepZ;
			uvs[index].x = 0.0f + ((float)j / dwWidth);
			uvs[index].y = 0.0f + ((float)i / dwLength);

			// Flat grid so tangent space is very basic; with more complex geometry 
			// you would have to export calculated tangent space vectors
			normals[index].x = 0.0f;
			normals[index].y = 1.0f;
			normals[index].z = 0.0f;
			binormals[index].x = 0.0f;
			binormals[index].y = 0.0f;
			binormals[index].z = -1.0f;
			tangents[index].x = 1.0f;
			tangents[index].y = 0.0f;
			tangents[index].z = 0.0f;
		}
	}

	// Allocate memory for buffer of indices in system memory
	indices.resize(nNumIndex);
	unsigned short *    pIndex = &indices[0];

	// Fill index buffer
	for (int i = 0; i < dwLength; ++i)
	{
		for (int j = 0; j < dwWidth; ++j)
		{
			*pIndex++ = (unsigned short)(i    * (dwWidth + 1) + j);
			*pIndex++ = (unsigned short)(i    * (dwWidth + 1) + j + 1);
			*pIndex++ = (unsigned short)((i + 1) * (dwWidth + 1) + j);

			*pIndex++ = (unsigned short)((i + 1) * (dwWidth + 1) + j);
			*pIndex++ = (unsigned short)(i    * (dwWidth + 1) + j + 1);
			*pIndex++ = (unsigned short)((i + 1) * (dwWidth + 1) + j + 1);
		}
	}


}

int wireframe = 0;

/*
void TW_CALL SetAutoRotateCB(const void *value, void *clientData)
{
	wireframe = *(const int *)value;
	if (wireframe != 0)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}


//  Callback function called by the tweak bar to get the 'AutoRotate' value
void TW_CALL GetAutoRotateCB(void *value, void *clientData)
{
	(void)clientData; // unused
	*(int *)value = wireframe; // copy g_AutoRotate to value
}*/

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
	window = glfwCreateWindow(1024, 768, "Tutorial 05", NULL, NULL);
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

	glfwSetWindowTitle(window, "Tutorial 05");

	// Ensure we can capture the escape key being pressed below
	//	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	//	glfwSetCursorPos(window, 1024 / 2, 768 / 2);
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
	GLuint programID = LoadShaders("shaders/tessllation.vertexshader", "shaders/Tessllation.fragmentshader", NULL, "shaders/Tessllation.tess_control_shader", "shaders/Tessllation.tess_eval_shader");

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");
	GLuint camera_positionId = glGetUniformLocation(programID, "camera_position");
	GLuint tessId = glGetUniformLocation(programID, "tess");
	GLuint scaleID = glGetUniformLocation(programID, "scale");
	GLuint disID = glGetUniformLocation(programID, "dis");



	// Get a handle for our "myTextureSampler" uniform
	GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");
	GLuint normalMapId = glGetUniformLocation(programID, "myNormal");
	GLuint displaceMapID = glGetUniformLocation(programID, "mydisplace");

	// Read our .obj file
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> binormals;
	std::vector<glm::vec3> tangents;
	std::vector<unsigned short> indices;
	FillGrid_Indexed_WithTangentSpace(25, 25, 200.0f, 200.0f, vertices, uvs, normals, binormals, tangents, indices);
	//bool res = loadOBJ("suzanne.obj", vertices, uvs, normals);

	// Load it into a VBO
	// Load the texture

	Mesh grid;
	grid.setAtrribute(Pos, &vertices[0], vertices.size()* sizeof(glm::vec3));
	grid.setAtrribute(Uv, &uvs[0], uvs.size()* sizeof(glm::vec2));
	grid.setAtrribute(Normal, &normals[0], normals.size()* sizeof(glm::vec3));
	grid.setAtrribute(BiTangent, &binormals[0], binormals.size()* sizeof(glm::vec3));
	grid.setAtrribute(Tangent, &tangents[0], tangents.size()* sizeof(glm::vec3));
	grid.setAtrribute(Index, &indices[0], indices.size()* sizeof(unsigned short));
	grid.LoadTexture(Displace, "models/rock_height.bmp");

	grid.LoadTexture(Normal_Map, "models/rocks_NM_height.bmp");
	grid.LoadTexture(Diffuse, "models/rocks.bmp");

	grid.vertex_size = vertices.size();
	grid.face_size = indices.size() / 3;

	Mesh ninja_head("models/ninjaHead_Low_smooth_po.obj", true);

	ninja_head.LoadTexture(Displace, "models/ninja_head_DM.bmp");
	ninja_head.LoadTexture(Normal_Map, "models/ninja_head_NM.bmp");
	ninja_head.LoadTexture(Diffuse, "models/ninja_head_AO.bmp");

	// Get a handle for our "LightPosition" uniform
	glUseProgram(programID);
	GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
	//initText2D("Holstein.tga");
	float dis = 0.17585;
	float scale = 0.2;
	Mesh * cur_mesh = &ninja_head;

	Mesh * meshes[2] = { &ninja_head, &grid };
	float  scales[2] = { 0.2f, 5.0f };
	float  dises[2] = { 0.17585f, 0.0f };


	int tess = 5;
		

	//////////////////////////////////////////////////////////////////////////
	//gui
	// Initialize AntTweakBar
	/*
	TwWindowSize(1024, 768);
	TwInit(TW_OPENGL_CORE, NULL);

	// Create a tweak bar
	TwBar * bar = TwNewBar("TweakBar");
	TwDefine(" GLOBAL help='This example shows how to integrate AntTweakBar with GLFW and OpenGL. Array Key Move Camera' "); // Message added to the help bar.

	
	// Add 'speed' to 'bar': it is a modifable (RW) variable of type TW_TYPE_DOUBLE. Its key shortcuts are [s] and [S].
	TwAddVarRW(bar, "tess", TW_TYPE_INT32, &tess,
		" label='Tessfactor' min=1 max=15 step=1 keyIncr=A keyDecr=D help='Tessllation factor' ");

	int index = 0;

	TwEnumVal shape[2] = { { 0, "NinjaHead" }, {1,"Groud"} };
	TwType shapeType = TwDefineEnum("Model Type", shape, 2);
	TwAddVarRW(bar, "Model", shapeType, &index, " keyIncr='N' keyDecr='C' help='Change object shape.' ");
	
	TwAddVarCB(bar, "AutoRotate", TW_TYPE_BOOL32, SetAutoRotateCB, GetAutoRotateCB, NULL,
		" label='WireFrame' key=space help='Toggle WirFrame Model mode.' ");
	*/
	
	int index = 0;

	/*
	glfwSetMouseButtonCallback(window,(GLFWmousebuttonfun)TwEventMouseButtonGLFW);
	// - Directly redirect GLFW mouse position events to AntTweakBar
	glfwSetCursorPosCallback(window, (GLFWcursorposfun)TwEventMousePosGLFW);
	// - Directly redirect GLFW mouse wheel events to AntTweakBar
	glfwSetScrollCallback(window, (GLFWscrollfun)TwEventMouseWheelGLFW);
	// - Directly redirect GLFW key events to AntTweakBar
	glfwSetKeyCallback(window,(GLFWkeyfun)TwEventKeyGLFW);
	// - Directly redirect GLFW char events to AntTweakBar
	glfwSetCharCallback(window,(GLFWcharfun)TwEventCharGLFW);
	*/

	//////////////////////////////////////////////////////////////////////////
	
	do{
		cur_mesh = meshes[index];
		scale = scales[index];
		dis = dises[index];
		if (wireframe != 0)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//f(cur_mesh);
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPatchParameteri(GL_PATCH_VERTICES, 3);
		// Use our shader
		glUseProgram(programID);

		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		//input_process(tess);

		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix  = getViewMatrix();
		glm::mat4 ModelMatrix = glm::mat4(1.0);
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		//glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		//glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
		

		glm::vec3 lightPos = glm::vec3(4, 4, 4);
		glm::vec3 camera_pos = getPosition();
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(camera_positionId, camera_pos.x, camera_pos.y, camera_pos.z);
		
		glUniform1i(tessId, tess);
		glUniform1f(disID, dis);
		glUniform1f(scaleID, scale);
		

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cur_mesh->displace_texture_id);
		glUniform1i(displaceMapID, 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, cur_mesh->normal_texture_id);
		// Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(normalMapId, 1);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, cur_mesh->diffuse_texture_id);
		glUniform1i(TextureID, 2);

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
 		glBindBuffer(GL_ARRAY_BUFFER, cur_mesh->binormal_id);
 		glVertexAttribPointer(
 			3,                                // attribute
 			3,                                // size
 			GL_FLOAT,                         // type
 			GL_FALSE,                         // normalized?
 			0,                                // stride
 			(void*)0                          // array buffer offset
 			);
 
 		glEnableVertexAttribArray(4);
 		glBindBuffer(GL_ARRAY_BUFFER, cur_mesh->tangent_id);
 		glVertexAttribPointer(
 			4,                                // attribute
 			3,                                // size
 			GL_FLOAT,                         // type
 			GL_FALSE,                         // normalized?
 			0,                                // stride
 			(void*)0                          // array buffer offset
 			);


		// Draw the triangles !
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cur_mesh->index_id);

		glDrawElements(GL_PATCHES, cur_mesh->face_size * 3, GL_UNSIGNED_SHORT, (void *)0);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(3);
		glDisableVertexAttribArray(4);


		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//	printText2D("Arrow Key move camera", 10, 10, 20);
	//	char text[256];
	//	sprintf(text, "current_factor %d", tess);
	//	printText2D(text, 10, 40, 20);
	//	printText2D("+ add Tessfactor, - sub Tessfactor", 10, 70, 20);
	//	printText2D("Space Toggle WireFrame", 10, 100, 20);
	//	printText2D("0 NinjaHead", 10, 130, 20);
	//	printText2D("1 Stone", 10, 160, 20);

		//std::cout << getPosition().x << " " << getPosition().y <<" "<< getPosition().z << std::endl;
		//std::cout << getViewDir().x << " " << getViewDir().y << " " << getViewDir().z << std::endl;


		// Swap buffers
		//TwDraw();
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		!glfwWindowShouldClose(window));

	// Cleanup VBO and shader

	glDeleteProgram(programID);

	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	//TwTerminate();
	glfwTerminate();

	return 0;
}

