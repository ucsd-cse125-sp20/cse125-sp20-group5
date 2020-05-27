#include "Core.h"
#include "Client.h"
#include <iostream>
#include <nanogui/glutil.h>
#include <nanogui/button.h>
#include <nanogui/window.h>
#include <nanogui/screen.h>
#include <nanogui/layout.h>
#include <nanogui/label.h>
#include <nanogui/FormHelper.h>



// These are really HACKS to make glfw call member functions instead of static functions

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

static void sscroll(GLFWwindow* window, double xoffset, double yoffset) {
	if (Client::CLIENT) {
		Client::CLIENT->zoomScreen(window, xoffset, yoffset);
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
	GLFWwindow * windowHandle = glfwCreateWindow(1080, 720, "plant", NULL, NULL);
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

	// Background color
	glClearColor(0., 0., 0., 1.);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);


	nanogui::Screen* screen = new nanogui::Screen();
	screen->initialize(windowHandle, true);

	enum test_enum {
		Item1 = 0,
		Item2,
		Item3
	};

	bool bvar = true;
	int ivar = 12345678;
	double dvar = 3.1415926;
	float fvar = (float)dvar;
	std::string strval = "A string";
	test_enum enumval = Item2;
	nanogui::Color colval(0.5f, 0.5f, 0.7f, 1.f);

	// Create nanogui gui
	bool enabled = true;
	nanogui::FormHelper* gui = new nanogui::FormHelper(screen);
	nanogui::ref<nanogui::Window> nanoguiWindow = gui->addWindow(Eigen::Vector2i(10, 10), "Form helper example");
	gui->addGroup("Basic types");
	gui->addVariable("bool", bvar)->setTooltip("Test tooltip.");
	gui->addVariable("string", strval);

	gui->addGroup("Validating fields");
	gui->addVariable("int", ivar)->setSpinnable(true);
	gui->addVariable("float", fvar)->setTooltip("Test.");
	gui->addVariable("double", dvar)->setSpinnable(true);

	gui->addGroup("Complex types");
	gui->addVariable("Enumeration", enumval, enabled)->setItems({ "Item 1", "Item 2", "Item 3" });

	gui->addGroup("Other widgets");
	gui->addButton("A button", []() { std::cout << "Button pressed." << std::endl; })->setTooltip("Testing a much longer tooltip, that will wrap around to new lines multiple times.");;

	screen->setVisible(true);
	screen->performLayout();
	nanoguiWindow->center();



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
