#include "ChooseLobby.h"

ChooseLobby::ChooseLobby(GLFWwindow* window, nanogui::Screen* screen) {
	enum test_enum {
		Item1 = 0,
		Item2,
		Item3
	};

	bool bvar = true;
	int ivar = 12345678;
	double dvar = 3.1415926;
	float fvar = (float)dvar;
	test_enum enumval = Item2;
	nanogui::Color colval(0.5f, 0.5f, 0.7f, 1.f);

	// Create nanogui gui
	bool enabled = true;
	nanogui::FormHelper* gui = new nanogui::FormHelper(screen);
	nanogui::ref<nanogui::Window> nanoguiWindow = gui->addWindow(Eigen::Vector2i(20, 20), "");
	nanoguiWindow->setFixedSize(nanogui::Vector2i(500, 100));
	/*gui->addGroup("Basic types");
	gui->addVariable("bool", bvar)->setTooltip("Test tooltip.");
	gui->addVariable("string", ipAddress);

	gui->addGroup("Validating fields");
	gui->addVariable("int", ivar)->setSpinnable(true);
	gui->addVariable("float", fvar)->setTooltip("Test.");
	gui->addVariable("double", dvar)->setSpinnable(true);

	gui->addGroup("Complex types");
	gui->addVariable("Enumeration", enumval, enabled)->setItems({ "Item 1", "Item 2", "Item 3" });

	std::string var = "asdf";*/
	gui->addGroup("Select Lobby");

	nanogui::TextBox* textBox = new nanogui::TextBox(nanoguiWindow, "localhost");
	textBox->setEditable(true);
	//textBox->setFormat("^[a-z]+$");// accept lower-case letters only
	textBox->setCallback([](const std::string& value) {
		std::cout << "Current Automatic Value: " << value << std::endl;
		return true;// we are verifying with the regex specified in setFormat
	});
	textBox->setFixedSize(nanogui::Vector2i(200,30));
	text = textBox;
	gui->addWidget("Enter IP", textBox);
	gui->addButton("Enter Lobby", [textBox]() { std::cout << "Button pressed. " << textBox->value() << std::endl; });
	

	/*nanogui::Window* nanoguiWindow = new nanogui::Window(screen, "Enter Lobby");
	nanogui::TextBox* textBox = new nanogui::TextBox(nanoguiWindow, "initial");
	textBox->setEditable(true);
	textBox->setFormat("^[a-z]+$");// accept lower-case letters only
	textBox->setCallback([](const std::string& value) {
		std::cout << "Current Automatic Value: " << value << std::endl;
		return true;// we are verifying with the regex specified in setFormat
	});*/


	screen->setVisible(true);
	screen->performLayout();
	nanoguiWindow->center();
}