////////////////////////////////////////
// Client.cpp
////////////////////////////////////////

#include "Client.h"
#include "NetworkClient.h"

////////////////////////////////////////////////////////////////////////////////

#define AUDIO_FILE_BGM "audio/animal\ dizhuing.wav"

////////////////////////////////////////////////////////////////////////////////
//Assimp Test
//TODO: to be removed

#include "AssimpModel.h"
#include "AnimatedAssimpModel.h"

AssimpModel* ourModel;

void loadAssimpModelTest() {
	ourModel = new AssimpModel("model/corn.fbx");
}

void renderAssimpModelTest(Camera* cam, uint shader) {
	glUseProgram(shader);

	// create a temp model mtx
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 1.75f, 0.0f));
	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, false, (float*)&model);
	glUniformMatrix4fv(glGetUniformLocation(shader, "projectView"), 1, false, (float*)&cam->GetViewProjectMtx());

	ourModel->draw(shader);

	glUseProgram(0);
}

////////////////////////////////////////////////////////////////////////////////

Client * Client::CLIENT;

Client::Client(GLFWwindow * window, int argc, char **argv) {
	windowHandle = window;

	// initalize mouse state
	leftDown=middleDown=rightDown=false;
	mouseX=mouseY=0;

	//set up keyboard tracker
	std::unordered_map<int, bool>* hi;
	hi = new std::unordered_map<int, bool>;
	keyPresses = hi;
	setupKeyboardPresses();

	// Initialize components
	program=new ShaderProgram("Model.glsl", ShaderProgram::eRender);

	glfwGetWindowSize(windowHandle, &winX, &winY);
	cam=new Camera;
	cam->SetAspect(float(winX)/float(winY));

	scene = Scene::scene0();

	setupAudio();

	//TODO: remove the test for assimp
	assimpProgram = new ShaderProgram("AssimpModel.glsl", ShaderProgram::eRender);
	loadAssimpModelTest();

	// Load network class
	setupNetwork();
}

////////////////////////////////////////////////////////////////////////////////

Client::~Client() {
	delete program;
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

		recieveState();

		// recieve the state from the server
		currentGameState = netClient->getCurrentState();

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

	scene->draw(cam->GetViewProjectMtx(), program->GetProgramID());

	//TODO: remove the Assimp test
	renderAssimpModelTest(cam, assimpProgram->GetProgramID());
	
	// Finish drawing scene
	glFinish();
	glfwSwapBuffers(windowHandle);
}

/* Ideally there would be a single networking object that we would pass our 
 events to and have it serializae and send them
 */
void Client::sendEvents()
{
}

/*Here we would poll the networking object and it would give us desearlized data in some format
*/
void Client::recieveState()
{
	scene->getState(keyPresses);
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


	if (action == GLFW_PRESS && keyPresses->count((char)key) > 0) {
		(*keyPresses)[key] = true;
	}

	if (action == GLFW_RELEASE && keyPresses->count((char)key) > 0) {
		(*keyPresses)[key] = false;
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

void Client::setupKeyboardPresses()
{
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
	netClient = new NetworkClient("localhost", "13");
	netClient->start();
}

////////////////////////////////////////////////////////////////////////////////
