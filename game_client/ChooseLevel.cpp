#include "ChooseLevel.h"

ChooseLevel::ChooseLevel(GLFWwindow* window, nanogui::Screen* screen, uint numLevels)
{
	numPlayers = 1;
	this->screen = screen;
	nanogui::Window* nanoguiWindow = new nanogui::Window(screen, "Choose Level");
	wind = nanoguiWindow;
	nanoguiWindow->setFixedSize(nanogui::Vector2i(200, 500));
	nanoguiWindow->setLayout(new nanogui::GroupLayout());

	nanogui::TextBox* textBox = new nanogui::TextBox(nanoguiWindow, "1");
	textBox->setEditable(false);
	textBox->setCallback([](const std::string& value) {
		return true;// we are verifying with the regex specified in setFormat
	});
	textBox->setFixedSize(nanogui::Vector2i(30, 30));
	textBox->setAlignment(nanogui::TextBox::Alignment::Center);
	text = textBox;


	for (uint i = 0; i < numLevels; i++) {
		nanogui::Button* level = new nanogui::Button(nanoguiWindow, std::string("Level ") + std::to_string(i));
		level->setCallback([textBox]() { std::cout << "Button pressed. " << textBox->value() << std::endl; });
		buttons.push_back(level);
	}



	screen->setVisible(true);
	screen->performLayout();
	nanoguiWindow->center();
}

int ChooseLevel::getLevel()
{
	for (int i = 0; i < buttons.size(); i++) {
		if (buttons[i]->pushed()) {
			return i;
		}
	}
	return -1;
}

void ChooseLevel::removeWindow()
{
	wind->setEnabled(false);
	wind->setVisible(false);
}

void ChooseLevel::setPlayers(uint numPlayers)
{
	this->numPlayers = numPlayers;
	text->setValue(std::to_string(numPlayers));
}
