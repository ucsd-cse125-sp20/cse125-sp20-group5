#include "core.h"
#include "Client.h"
#include <iostream>

// These are really HACKS to make glut call member functions instead of static functions

static void skeyboard(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (Client::CLIENT != NULL) {
		Client::CLIENT->keyboard(window, key, scancode, action, mods);
	}
}

static void smouseButton(GLFWwindow* window, int btn, int action, int mods) {
	if (Client::CLIENT) {
		Client::CLIENT->mouseButton(window, btn, action, mods);
	}
}

static void smouseMotion(GLFWwindow* window, double x, double y) {
	if (Client::CLIENT) {
		Client::CLIENT->mouseMotion(window, x, y);
	}
}

static void sresize(GLFWwindow* window, int width, int height) {
	if (Client::CLIENT) {
		Client::CLIENT->resize(window, width, height);
	}
}



////////////////////////////////////////////////////////////////////////////////

void glfwErrorCallback(int error, const char* description)
{
	fputs(description, stderr);
}

/*
right now the main sets up teh graphics and windows then starts the client

the networking people need to figure out where to put the networking code
ideally it coudl fit into a entirely seperate class 
*/
int main(int argc, char** argv) {
	// set error callback first incase even initalization fails

	glfwSetErrorCallback(glfwErrorCallback);

	// initlize glfw
	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}

	// Create the window
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	GLFWwindow * windowHandle = glfwCreateWindow(1000, 600, "plant", NULL, NULL);
	glfwSetWindowPos(windowHandle, 100, 100);
	if (!windowHandle)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	// doe some basic setup
	glfwMakeContextCurrent(windowHandle);
	glfwSwapInterval(1);

	// Background color
	glClearColor(0., 0., 0., 1.);

	// Set Callbacks
	glfwSetKeyCallback(windowHandle, skeyboard);
	glfwSetMouseButtonCallback(windowHandle, smouseButton);
	glfwSetCursorPosCallback(windowHandle, smouseMotion);
	glfwSetWindowSizeCallback(windowHandle, sresize);


	// Initialize GLEW
	glewInit();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// make the client
	Client::CLIENT = new Client(windowHandle, argc, argv);
	
	// start the game loop
	Client::CLIENT->loop();


	delete Client::CLIENT;

	// close down glfw
	glfwTerminate();
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
