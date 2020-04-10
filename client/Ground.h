#pragma once
#include "core.h"
#include "Model.h"

class Ground
{
	enum class TILE_TYPE;

private:
	int tilesX;
	int tilesY;
	float tileSize;

	glm::vec3 position;
	glm::mat4 localMat;

	// this is a one dimensinal arrya that ill use as a two dimensional cuz c++ arrays are bad
	std::vector<Model*> tiles;
	TILE_TYPE* grid;


public:
	static enum class TILE_TYPE {PATH = 0, NORMAL = 1, TILLED = 2, WATER = 3};
	static const int NUM_TILES = 4;

	Ground(int x, int y, float size);
	~Ground();
	TILE_TYPE getLoc(int x, int y);
	void setLoc(int x, int y, TILE_TYPE type);
	void update();
	void draw(const glm::mat4& viewProjMtx, uint shader);

	static glm::vec3 getColor(TILE_TYPE type);
	static char* getTexture(TILE_TYPE type);

	static Ground * ground0();
};

