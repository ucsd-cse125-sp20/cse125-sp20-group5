#include "ChooseLobby.h"

ChooseLobby::ChooseLobby(GLFWwindow* window, nanogui::Screen* screen) {

	// Create nanogui gui
	bool enabled = true;
	this->screen = screen;

	nanogui::Window * nanoguiWindow = new nanogui::Window(screen, "Choose Lobby");
	wind = nanoguiWindow;
	nanoguiWindow->setFixedSize(nanogui::Vector2i(500, 200));
	nanoguiWindow->setLayout(new nanogui::GroupLayout());


	nanogui::TextBox* textBox = new nanogui::TextBox(nanoguiWindow, "localhost");
	textBox->setEditable(true);
	textBox->setCallback([](const std::string& value) {
		return true;// we are verifying with the regex specified in setFormat
	});
	textBox->setFixedSize(nanogui::Vector2i(200,30));
	textBox->setAlignment(nanogui::TextBox::Alignment::Center);
	text = textBox;

	nanogui::Button* enterButton = new nanogui::Button(nanoguiWindow, "Enter Lobby");
	enterButton->setCallback([textBox]() { std::cout << "Button pressed. " << textBox->value() << std::endl; });
	enter = enterButton;

	screen->setVisible(true);
	screen->performLayout();
	nanoguiWindow->center();
}



const std::string ChooseLobby::getIpAddress()
{
	return text->value();
}

bool ChooseLobby::getButtonStatus()
{
	return enter->pushed();
}

void ChooseLobby::removeWindow()
{
	wind->setEnabled(false);
	wind->setVisible(false);
	//screen->removeChild(wind);
}
