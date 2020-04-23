////////////////////////////////////////
// Client.cpp
////////////////////////////////////////

#include "Client.h"
#include "NetworkClient.h"

Client * Client::CLIENT;

Client::Client(GLFWwindow * window, int argc, char **argv) {
	windowHandle = window;

	// initalize mouse state
	leftDown=middleDown=rightDown=false;
	mouseX=mouseY=0;

	// Initialize components
	glfwGetWindowSize(windowHandle, &winX, &winY);
	cam=new Camera;
	cam->SetAspect(float(winX)/float(winY));

	scene = new Scene();

	setupAudio();

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

		// recieve the state from the server
		currentGameState = netClient->getCurrentState();

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
	cam->Update();
	scene->update();
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

	scene->draw(cam->GetViewProjectMtx());
	// scene->draw(cam->GetViewProjectMtx(), program->GetProgramID());
	
	// Finish drawing scene
	glFinish();
	glfwSwapBuffers(windowHandle);
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
		case 0x1b:		// Escape
			quit();
			break;
		case 'r':
			reset();
			break;
		case GLFW_KEY_W: {
			std::string w("w");
			netClient->sendMessage(w);
			break;
		}
		case GLFW_KEY_A: {
			std::string a("a");
			netClient->sendMessage(a);
			break;
		}
		case GLFW_KEY_S: {
			std::string s("s");
			netClient->sendMessage(s);
			break;
		}
		case GLFW_KEY_D: {
			std::string d("d");
			netClient->sendMessage(d);
			break;
		}
		default:
			break;
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
	if (cam->GetFOV() >= 1.0f && cam->GetFOV() <= 45.0f)
		cam->SetFOV(cam->GetFOV() - yoffset);
	if (cam->GetFOV() <= 1.0f)
		cam->SetFOV(1.0f);
	if (cam->GetFOV() >= 45.0f)
		cam->SetFOV(45.0f);
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
	netClient = new NetworkClient("localhost", "13");
	netClient->start();
}

////////////////////////////////////////////////////////////////////////////////
