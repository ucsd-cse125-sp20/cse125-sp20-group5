#pragma once
#include "core.h"
#include "Model.h"
#include "AssimpModel.h"

class Ground : public Drawable
{
	enum class TILE_TYPE;

private:
	int tilesX, tilesY;
	int padX, padY;
	int totalX, totalY;

	float tileSize;

	// this is a one dimensinal arrya that ill use as a two dimensional cuz c++ arrays are bad
	TILE_TYPE* grid;
	bool* highlighted_grid;

	std::vector<Model*> tiles;
	AssimpModel* tilled;
	Model* baseLayer;
	Model* outlineBox;

public:
	static enum class TILE_TYPE { NORMAL = 0, PATH = 1, TILLED = 2, BLANK = 3, BASE_LAYER = 4};
	static const int NUM_TILES = 5;

	Ground(int x, int y, float size, int paddingX, int paddingY, uint shader, uint assimpShader);
	~Ground();
	TILE_TYPE getLoc(int x, int y);
	void setLoc(int x, int y, TILE_TYPE type);

	SceneNode* createSceneNodes(uint objectId) override;
	void update(SceneNode * node) override;
	void draw(SceneNode& node, const glm::mat4& viewProjMtx) override;

	void setPadding(TILE_TYPE type);

	int getX();
	int getY();

	static glm::vec3 getColor(TILE_TYPE type);
	static const char* getTexture(TILE_TYPE type);

	static Ground * ground0(uint shader);

	void highlightTile(int x, int y);
};

