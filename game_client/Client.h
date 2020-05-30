////////////////////////////////////////
// Tester.h
////////////////////////////////////////

#pragma once

#include "core.h"
#include "Shader.h"
#include "Camera.h"
#include "Scene.h"
#include "ChooseLobby.h"
#include "AudioEngine.h"
#include "NetworkClient.h"
#include <vector>
#include <iostream>
#include <unordered_map>

#include <nanogui/screen.h>
////////////////////////////////////////////////////////////////////////////////
/*
Currently the Client class handles the I/O as well as the game loop
*/
enum class ClientState {GETIP, CHOOSELEVEL, PLAYING};

class Client {
public:
	nanogui::Screen* screen;
	static Client * CLIENT;

	Client(GLFWwindow * window, nanogui::Screen * screen, int argc, char** argv);
	~Client();

	void loop(); // Loop will be the idling function of glut
	void update(); // recalcuate the matrices
	void draw(); // renders everything
	void sendKeyboardEvents(); // Send keyboard events

	void reset(); // reset scene ?
	void quit(); // close window exit progmra ?
	void setupAudio(); // setup audio engine
	void setupNetwork(); // setup network client
	void setupKeyboardPresses(); // setup keyboard handler

	// Event handlers
	void resize(GLFWwindow* window, int width, int height);
	void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods);
	void mouseButton(GLFWwindow* window, int btn, int action, int mods);
	void mouseMotion(GLFWwindow* window, double x, double y);
	void zoomScreen(GLFWwindow* window, double xoffset, double yoffset);

	// getter
	static int getWinX() { return winX; }
	static int getWinY() { return winY; }

private:
	// Window management
	static int winX, winY;

	ClientState state;

	// Input
	bool leftDown, middleDown, rightDown;
	int mouseX, mouseY;

	// Keyboard sensers
	const int keys[5] = {GLFW_KEY_W, GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D, GLFW_KEY_SPACE};

	GLFWwindow* windowHandle;

	// Components	
	Scene * scene;
	ChooseLobby* startPage;
	Camera * cam;
	CAudioEngine aEngine;
	std::unordered_map<int, bool>* keyPresses;

	NetworkClient *netClient;
	GameState* currentGameState;
};

////////////////////////////////////////////////////////////////////////////////
