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

void ChooseLevel::readInImages(GLuint * imageID, const char * texturefile) {
	//store texture buffer
	glBindTexture(GL_TEXTURE_2D, *imageID);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// load and generate the texture
	int width, height, nrChannels;
	unsigned char* data = stbi_load(texturefile, &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		//glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture " << texturefile << std::endl;
	}
	stbi_image_free(data);
}