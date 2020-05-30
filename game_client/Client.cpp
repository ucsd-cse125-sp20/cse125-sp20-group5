////////////////////////////////////////
// Client.cpp
////////////////////////////////////////

#include "Client.h"
#include "NetworkClient.h"
#include "Message.hpp"


// static
Client * Client::CLIENT;
int Client::winX;
int Client::winY;

Client::Client(GLFWwindow * window, nanogui::Screen  *screen, int argc, char **argv) {
	windowHandle = window;

	// nangui stuff
	this->screen = screen;
	this->startPage = new ChooseLobby(window, screen);

	// initalize mouse state
	leftDown=middleDown=rightDown=false;
	mouseX=mouseY=0;

	// initialize keyboard state
	setupKeyboardPresses();

	// Initialize components
	glfwGetWindowSize(windowHandle, &winX, &winY);
	cam=new Camera;
	cam->SetAspect(float(winX)/float(winY));

	scene = new Scene(); 
	scene->setupDirectionalLighting(cam->GetPosition());

	setupAudio();

	state = ClientState::GETIP;

	// Load network class
	setupNetwork();
}

////////////////////////////////////////////////////////////////////////////////

Client::~Client() {
	delete cam;
	delete scene;
	delete netClient;

	aEngine.Shutdown();

	glFinish();
	glfwDestroyWindow(windowHandle);
}

////////////////////////////////////////////////////////////////////////////////

void Client::loop() {
	while (!glfwWindowShouldClose(windowHandle)) {
		//std::cout << "main looop" << std::endl;
		sendKeyboardEvents();

		// recieve the state from the server
		currentGameState = netClient->getCurrentState();
		//std::cout << currentGameState << std::endl;

		scene->setState(currentGameState);

		// Update the components in the world
		// calculate matrices for rendering
		update();

		// Tell redraw the scene
		draw();

		// process new input events
		glfwPollEvents();
	}
}

void Client::update() {
	if (state == ClientState::PLAYING) {
		cam->Update();
		if (currentGameState != nullptr) {
			scene->update();
		}
	}
	else if (state == ClientState::GETIP) {
		
	}
	// Maybe show a loading screen or something if gameState is nullptr (not yet received)?
}

////////////////////////////////////////////////////////////////////////////////

void Client::reset() {
	cam->Reset();
	cam->SetAspect(float(winX)/float(winY));

}

void Client::draw() {
	// Begin drawing scene
	glViewport(0, 0, winX, winY);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//scene->draw(cam->GetViewProjectMtx());
	screen->drawContents();
	screen->drawWidgets();

	// Finish drawing scene
	glFinish();
	glfwSwapBuffers(windowHandle);
}

/* Ideally there would be a single networking object that we would pass our 
 events to and have it serializae and send them
 */
void Client::sendKeyboardEvents()
{
	if ((*keyPresses)[GLFW_KEY_W] && (*keyPresses)[GLFW_KEY_A]) {
		netClient->sendMessage(OPCODE_PLAYER_MOVE_UPPER_LEFT);
	}
	else if ((*keyPresses)[GLFW_KEY_W] && (*keyPresses)[GLFW_KEY_D]) {
		netClient->sendMessage(OPCODE_PLAYER_MOVE_UPPER_RIGHT);
	}
	else if ((*keyPresses)[GLFW_KEY_S] && (*keyPresses)[GLFW_KEY_A]) {
		netClient->sendMessage(OPCODE_PLAYER_MOVE_LOWER_LEFT);
	}
	else if ((*keyPresses)[GLFW_KEY_S] && (*keyPresses)[GLFW_KEY_D]) {
		netClient->sendMessage(OPCODE_PLAYER_MOVE_LOWER_RIGHT);
	}
	else if ((*keyPresses)[GLFW_KEY_W]) {
        netClient->sendMessage(OPCODE_PLAYER_MOVE_UP);
	}
	else if ((*keyPresses)[GLFW_KEY_A]) {
        netClient->sendMessage(OPCODE_PLAYER_MOVE_LEFT);
	}
	else if ((*keyPresses)[GLFW_KEY_S]) {
        netClient->sendMessage(OPCODE_PLAYER_MOVE_DOWN);
	}
	else if ((*keyPresses)[GLFW_KEY_D]) {
		netClient->sendMessage(OPCODE_PLAYER_MOVE_RIGHT);
	}
	else {
		netClient->sendMessage(OPCODE_PLAYER_MOVE_FREEZE);
	}

	if ((*keyPresses)[GLFW_KEY_E]) {
		netClient->sendMessage(OPCODE_PLAYER_ACTION);
	}

	/* For testing */
	if ((*keyPresses)[GLFW_KEY_1]) {
		int player1_numAnim = 2;
		Animation* player1_anim = currentGameState->players[0]->animation;
		player1_anim->animationType = (player1_anim->animationType + 1) % player1_numAnim;
	}
	if ((*keyPresses)[GLFW_KEY_2]) {
		//scene->healthBar->updateBar(std::fmod(scene->healthBar->filledFraction + 0.3, 1.0));
	}
}

////////////////////////////////////////////////////////////////////////////////

void Client::quit() {
	glFinish();
	glfwDestroyWindow(windowHandle);
	exit(0);
}

////////////////////////////////////////////////////////////////////////////////

void Client::resize(GLFWwindow* window, int width, int height) {
	winX = width;
	winY = height;
	cam->SetAspect(float(winX)/float(winY));
}

////////////////////////////////////////////////////////////////////////////////

void Client::keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	switch (key) {
		case GLFW_KEY_ESCAPE:		// Escape
			quit();
			break;
		case GLFW_KEY_R:
			reset();
			break;
		// TODO: Used to test firing particles
		case GLFW_KEY_P:
			if (action == GLFW_PRESS) {
				scene->toggleWater();
			}
		default:
			break;	
	}

	if (action == GLFW_PRESS && keyPresses->count((char)key) > 0) {
		(*keyPresses)[key] = true;
	}

	if (action == GLFW_RELEASE && keyPresses->count((char)key) > 0) {
		(*keyPresses)[key] = false;
		if (key == GLFW_KEY_SPACE) {
			netClient->sendMessage(OPCODE_PLAYER_INTERACT);
		}
	}

	
}

////////////////////////////////////////////////////////////////////////////////

void Client::mouseButton(GLFWwindow* window,int btn,int action,int mods) {
	if(btn== GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS) {
			leftDown = true;
		}
		else if (action == GLFW_RELEASE) {
			leftDown = false;
		}
	}
	else if(btn== GLFW_MOUSE_BUTTON_MIDDLE) {
		if (action == GLFW_PRESS) {
			middleDown = true;
		}
		else if (action == GLFW_RELEASE) {
			middleDown = false;
		}
	}
	else if(btn== GLFW_MOUSE_BUTTON_RIGHT) {
		if (action == GLFW_PRESS) {
			rightDown = true;
		}
		else if (action == GLFW_RELEASE) {
			rightDown = false;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

void Client::mouseMotion(GLFWwindow* window, double nx, double ny) {
	int maxDelta=100;
	int dx = glm::clamp((int)nx - mouseX,-maxDelta,maxDelta);
	int dy = glm::clamp(-((int)ny - mouseY),-maxDelta,maxDelta);

	mouseX = nx;
	mouseY = ny;

	// Move camera
	// NOTE: this should really be part of Camera::Update()
	if(leftDown) {
		const float rate=1.0f;
		cam->SetAzimuth(cam->GetAzimuth()+dx*rate);
		cam->SetIncline(glm::clamp(cam->GetIncline()-dy*rate,-90.0f,90.0f));
	}
	if(rightDown) {
		const float rate=0.005f;
		float dist=glm::clamp(cam->GetDistance()*(1.0f-dx*rate),0.01f,1000.0f);
		cam->SetDistance(dist);
	}
}

////////////////////////////////////////////////////////////////////////////////

void Client::zoomScreen(GLFWwindow* window, double xoffset, double yoffset) {
	float lowerBound = 0.10f;
	float upperBound = 75.0f;
	if (cam->GetFOV() >= lowerBound && cam->GetFOV() <= upperBound)
		cam->SetFOV(cam->GetFOV() - yoffset);
	if (cam->GetFOV() <= lowerBound)
		cam->SetFOV(lowerBound);
	if (cam->GetFOV() >= upperBound)
		cam->SetFOV(upperBound);
}

////////////////////////////////////////////////////////////////////////////////

void Client::setupKeyboardPresses()
{
	keyPresses = new std::unordered_map<int, bool>;
	for (int c : keys) {
		(*keyPresses)[c] = false;
	}
}

////////////////////////////////////////////////////////////////////////////////

void Client::setupAudio() {
	aEngine.Init();

	aEngine.LoadSound(AUDIO_FILE_BGM, false, true);
	//aEngine.LoadSound("hold-weapon.mp3", true);
	//aEngine.LoadSound("weapon-collide.mp3", true);
	//aEngine.LoadSound("scream.mp3", true);

	aEngine.PlaySounds(AUDIO_FILE_BGM, glm::vec3(0), aEngine.VolumeTodB(0.02f));

}

////////////////////////////////////////////////////////////////////////////////
void Client::setupNetwork() {
	netClient = new NetworkClient("localhost", "10032");
	netClient->start();
}

////////////////////////////////////////////////////////////////////////////////
