#pragma once
#include "Core.h"
#include <nanogui/button.h>
#include <nanogui/window.h>
#include <nanogui/screen.h>
#include <nanogui/layout.h>
#include <nanogui/label.h>
#include <nanogui/FormHelper.h>

class ChooseLobby {
public:
	static ChooseLobby* chooseLobby;

	ChooseLobby(GLFWwindow* window, nanogui::Screen* screen);

private:
	std::string ipAddress;
	bool enteringLobby;
	bool connectionMade;
};

