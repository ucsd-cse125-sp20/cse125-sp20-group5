#include "Core.h"
#include "Client.h"
#include <iostream>

#include <nanogui/glutil.h>


// These are really HACKS to make glfw call member functions instead of static functions

static void skeyboard(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (Client::CLIENT != NULL) {
		Client::CLIENT->keyboard(window, key, scancode, action, mods);
		Client::CLIENT->screen->keyCallbackEvent(key, scancode, action, mods);
	}
}

static void smouseButton(GLFWwindow* window, int btn, int action, int mods) {
	if (Client::CLIENT) {
		Client::CLIENT->mouseButton(window, btn, action, mods);
		Client::CLIENT->screen->mouseButtonCallbackEvent(btn, action, mods);
	}
}

static void smouseMotion(GLFWwindow* window, double x, double y) {
	if (Client::CLIENT) {
		Client::CLIENT->mouseMotion(window, x, y);
		Client::CLIENT->screen->cursorPosCallbackEvent(x, y);
	}
}

static void sresize(GLFWwindow* window, int width, int height) {
	if (Client::CLIENT) {
		Client::CLIENT->resize(window, width, height);
	}
}

static void sscroll(GLFWwindow* window, double xoffset, double yoffset) {
	if (Client::CLIENT) {
		Client::CLIENT->zoomScreen(window, xoffset, yoffset);
		Client::CLIENT->screen->scrollCallbackEvent(xoffset, yoffset);
	}
}

static void sChar(GLFWwindow* window, unsigned int codepoint) {
	if (Client::CLIENT) {
		Client::CLIENT->screen->charCallbackEvent(codepoint);
	}
}

static void sSetDrop(GLFWwindow* window, int count, const char** filenames) {
	if (Client::CLIENT) {
		Client::CLIENT->screen->dropCallbackEvent(count, filenames);
	}
}

static void sSetFrameBufSize(GLFWwindow* window, int width, int height) {
	if (Client::CLIENT) {
		Client::CLIENT->screen->resizeCallbackEvent(width, height);
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
	GLFWwindow * windowHandle = glfwCreateWindow(1440, 720, "plant", NULL, NULL);
	glfwSetWindowPos(windowHandle, 100, 100);
	if (!windowHandle)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	// doe some basic setup
	glfwMakeContextCurrent(windowHandle);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		throw std::runtime_error("Could not initialize GLAD!");
	glGetError(); // pull and ignore unhandled errors like GL_INVALID_ENUM
	glfwSwapInterval(1);


	int width, height;
	glfwGetFramebufferSize(windowHandle, &width, &height);
	glViewport(0, 0, width, height);
	glfwSwapInterval(0);
	glfwSwapBuffers(windowHandle);

	// Set Callbacks
	glfwSetKeyCallback(windowHandle, skeyboard);
	glfwSetMouseButtonCallback(windowHandle, smouseButton);
	glfwSetCursorPosCallback(windowHandle, smouseMotion);
	glfwSetWindowSizeCallback(windowHandle, sresize);
	glfwSetScrollCallback(windowHandle, sscroll);
	glfwSetCharCallback(windowHandle, sChar);
	glfwSetFramebufferSizeCallback(windowHandle, sSetFrameBufSize);
	glfwSetDropCallback(windowHandle, sSetDrop);

	// Background color
	glClearColor(0., 0., 0., 1.);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);


	nanogui::Screen* screen = new nanogui::Screen();
	screen->initialize(windowHandle, true);

	// make the client
	Client::CLIENT = new Client(windowHandle, screen, argc, argv);
	
	// start the game loop
	Client::CLIENT->loop();


	delete Client::CLIENT;

	// close down glfw
	glfwTerminate();
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
