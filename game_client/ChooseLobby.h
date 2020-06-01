#pragma once
#include "Core.h"
#include <nanogui/button.h>
#include <nanogui/window.h>
#include <nanogui/screen.h>
#include <nanogui/layout.h>
#include <nanogui/label.h>
#include <nanogui/textbox.h>

class ChooseLobby {
public:
	static ChooseLobby* chooseLobby;

	ChooseLobby(GLFWwindow* window, nanogui::Screen* screen);
	const std::string getIpAddress();
	bool getButtonStatus();
	void removeWindow();

private:
	nanogui::Screen* screen;
	nanogui::Window* wind;
	nanogui::Button * enter;
	nanogui::TextBox * text;
	std::string ipAddress;
};

