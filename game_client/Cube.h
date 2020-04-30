#pragma once

// Define the coordinates and indices needed to draw the cube. Note that it is not necessary
// to use a 2-dimensional array, since the layout in memory is the same as a 1-dimensional array.
// This just looks nicer since it's easy to tell what coordinates/indices belong where.
const GLfloat vertices[36][3] = {
	// Back face
	{ -1.0f,  1.0f, -1.0f}, {	-1.0f, -1.0f, -1.0f}, {1.0f, -1.0f, -1.0f }, {1.0f, -1.0f, -1.0f}, {1.0f,  1.0f, -1.0f}, {-1.0f,  1.0f, -1.0f},

	// Left face
	{-1.0f, -1.0f,  1.0f}, {-1.0f, -1.0f, -1.0f}, {-1.0f,  1.0f, -1.0f}, {-1.0f,  1.0f, -1.0f}, {-1.0f,  1.0f,  1.0f}, {-1.0f, -1.0f,  1.0f},

	// Right face
	{1.0f, -1.0f, -1.0f }, {1.0f, -1.0f,  1.0f}, {1.0f,  1.0f,  1.0f}, {1.0f,  1.0f,  1.0f}, {1.0f,  1.0f, -1.0f}, {1.0f, -1.0f, -1.0f},

	// Front face
	{-1.0f, -1.0f,  1.0f }, {-1.0f,  1.0f,  1.0f}, {1.0f,  1.0f,  1.0f}, {1.0f,  1.0f,  1.0f}, {1.0f, -1.0f,  1.0f}, {-1.0f, -1.0f,  1.0f},

	// Top face
	{-1.0f,  1.0f, -1.0f}, {1.0f,  1.0f, -1.0f}, {1.0f,  1.0f,  1.0f}, {1.0f,  1.0f,  1.0f}, {-1.0f,  1.0f,  1.0f}, {-1.0f,  1.0f, -1.0f},

	// Bottom face
	{-1.0f, -1.0f, -1.0f }, {-1.0f, -1.0f,  1.0f}, {1.0f, -1.0f, -1.0f}, {1.0f, -1.0f, -1.0f}, {-1.0f, -1.0f,  1.0f}, {1.0f, -1.0f,  1.0f}
};
