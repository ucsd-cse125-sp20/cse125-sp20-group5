////////////////////////////////////////
// Core.h
////////////////////////////////////////

#pragma once

// This file just has a bunch of common stuff used by all objects. It mainly just
// includes GL and some other standard headers.

// Set up GLM
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

// Set up GLEW
#define GLEW_STATIC
#pragma comment(lib, "glew32.lib")
#include "GL/glew.h"

// Set up GLEW
#include <GLFW/glfw3.h>

// get our constants
#include "Constants.h"

// Types
typedef unsigned int uint;

// STD library
#include <vector>
#include <ctype.h>
#include <chrono>
#include <ctime>  
