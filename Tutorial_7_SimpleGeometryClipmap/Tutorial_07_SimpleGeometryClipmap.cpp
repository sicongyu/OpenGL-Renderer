#include "Predefine.h"
#include "Terrain.h"
#include "Camera.h"
#include "Tree.h"
#include <AntTweakBar.h>
using namespace std;

GLFWwindow* window;

Camera camera;
Terrain terrain;
Tree tree;

bool rightButtonPressed = false;

void ErrorCallback(int error, const char* description) {
	puts(description);
}

void CursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
	static double oldX = 0, oldY = 0;
	static bool oldState = false;
	if (rightButtonPressed && oldState) {
		camera.Rotate(xpos - oldX, ypos - oldY);
		//vec3 t = camera.eulerAngle;
		//printf("%.2f %.2f %.2f\n", t.x, t.y, t.z);
	}
	oldX = xpos;
	oldY = ypos;
	oldState = rightButtonPressed;

	TwEventMousePosGLFW(window, xpos, ypos);
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (action == GLFW_PRESS) {
			rightButtonPressed = true;
		} else {
			rightButtonPressed = false;
		}
	}
	TwEventMouseButtonGLFW(window, button, action, mods);
}

void Draw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	terrain.Update(&camera);
	terrain.Draw();
	tree.Draw(&camera);
}

int wireframe = 0;
void TW_CALL SetAutoRotateCB(const void *value, void *clientData) {
	wireframe = *(const int *)value;
	if (wireframe != 0)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}


//  Callback function called by the tweak bar to get the 'AutoRotate' value
void TW_CALL GetAutoRotateCB(void *value, void *clientData) {
	(void)clientData; // unused
	*(int *)value = wireframe; // copy g_AutoRotate to value
}

int main() {
	// Initialise GLFW
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

	glfwWindowHint(GLFW_SAMPLES, 4);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1024, 768, "Tutorial 07", NULL, NULL);
	if (window == NULL) {
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

	glfwSetWindowTitle(window, "Tutorial 07");

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);	

	glClearDepth(1.0f);
	glClearColor(0.51f, 0.83f, 1.0f, 1.0f);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

	camera.Init(vec3(112000, 2100, 64100), vec3(112000, 2100, 64099), vec3(0, 1, 0));
	terrain.Init();
	tree.Init();

#ifndef WIN32
	TwWindowSize(1024 * 2, 768 * 2);
#else
	TwWindowSize(1024, 768);
#endif
	TwInit(TW_OPENGL_CORE, NULL);

	// Create a tweak bar
	TwBar * bar = TwNewBar("TweakBar");
	TwDefine(" GLOBAL help='This example shows how to integrate AntTweakBar with GLFW and OpenGL. Array Key Move Camera' "); // Message added to the help bar.

	TwAddVarRW(bar, "camera speed", TW_TYPE_FLOAT, &(camera.moveSpeed), "Camera Speed");
	TwAddVarCB(bar, "wireframe", TW_TYPE_BOOL32, SetAutoRotateCB, GetAutoRotateCB, NULL, "wireframe");

	// Set GLFW event callbacks
	// - Directly redirect GLFW mouse button events to AntTweakBar
	//glfwSetMouseButtonCallback(window, (GLFWmousebuttonfun)TwEventMouseButtonGLFW);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	// - Directly redirect GLFW mouse position events to AntTweakBar
	//glfwSetCursorPosCallback(window, (GLFWcursorposfun)TwEventMousePosGLFW);
	glfwSetCursorPosCallback(window, CursorPosCallback);
	// - Directly redirect GLFW mouse wheel events to AntTweakBar
	glfwSetScrollCallback(window, (GLFWscrollfun)TwEventMouseWheelGLFW);
	// - Directly redirect GLFW key events to AntTweakBar
	glfwSetKeyCallback(window, (GLFWkeyfun)TwEventKeyGLFW);
	// - Directly redirect GLFW char events to AntTweakBar
	glfwSetCharCallback(window, (GLFWcharfun)TwEventCharGLFW);

	float lastTime = glfwGetTime(), curTime;
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window)) {	
		curTime = glfwGetTime();
		camera.Move(window, curTime - lastTime);
		lastTime = curTime;

		if (glfwGetKey(window, GLFW_KEY_P)) {
			vec3 t = camera.GetPos();
			printf("Camera Position:%.2f %.2f %.2f\n", t.x, t.y, t.z);
			t = camera.GetDir();
			printf("Camera Direction:%.2f %.2f %.2f\n", t.x, t.y, t.z);
			t = camera.GetUp();
			printf("Camera Up:%.2f %.2f %.2f\n", t.x, t.y, t.z);
		}

		Draw();
		TwDraw();

		// Swap buffers
		glfwSwapBuffers(window);
		// Poll for and process events
		glfwPollEvents();

		glfwSetErrorCallback(ErrorCallback);
	}

	glfwTerminate();
    return 0;
}

