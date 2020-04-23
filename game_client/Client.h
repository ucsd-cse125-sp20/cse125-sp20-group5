////////////////////////////////////////
// Tester.h
////////////////////////////////////////

#pragma once

#include "core.h"
#include "Shader.h"
#include "Camera.h"
#include "SpinningCube.h"
#include "box.h"
#include "Scene.h"
#include "AudioEngine.h"
#include "NetworkClient.h"
#include <vector>
#include <iostream>
#include <unordered_map>

////////////////////////////////////////////////////////////////////////////////
/*
Currently the Client class handles the I/O as well as the game loop
*/
class Client {
public:
	static Client * CLIENT;

	Client(GLFWwindow * window, int argc, char** argv);
	~Client();

	void loop(); // Loop will be the idling function of glut
	void update(); // recalcuate the matrices
	void draw(); // renders everything

	void reset(); // reset scene ?
	void quit(); // close window exit progmra ?
	void setupAudio(); // setup audio engine
	void setupNetwork(); // setup network client

	// Event handlers
	void resize(GLFWwindow* window, int width, int height);
	void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods);
	void mouseButton(GLFWwindow* window, int btn, int action, int mods);
	void mouseMotion(GLFWwindow* window, double x, double y);

private:
	// Window management
	int winX, winY;

	// Input
	bool leftDown, middleDown, rightDown;
	int mouseX, mouseY;

	// Keyboard sensers
	const int keys[5] = {GLFW_KEY_W, GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D, GLFW_KEY_SPACE};

	GLFWwindow* windowHandle;
	// Components
	// ShaderProgram *program;
	// ShaderProgram *assimpProgram;
	
	Scene * scene;
	Camera * cam;
	CAudioEngine aEngine;

	std::vector<box> boxes;

	NetworkClient *netClient;
	GameState* currentGameState;
};

////////////////////////////////////////////////////////////////////////////////
