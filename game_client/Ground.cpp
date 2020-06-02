#include "Ground.h"
#include "ClientParams.h"
#include <iostream>

Ground::Ground(ClientParams& config, int x, int y, float size,  int padX, int padY, uint shader, uint assimpShader) : config(config)
{
	tilesX = x;
	tilesY = y;
	this->padX = padX;
	this->padY = padY;
	totalX = (x + (2 * padX));
	totalY = (y + (2 * padY));
	tileSize = size;

	int gridsize = totalX * totalY;

	grid = new TILE_TYPE[gridsize];

	int hgridsize = x * y;
	highlighted_grid = new bool[hgridsize];
	// std::fill(highlighted_grid, highlighted_grid + hgridsize * sizeof(bool), false);
	for (int i = 0; i < hgridsize; i++) {
		highlighted_grid[i] = false;
	}

	for (int i = 0; i < NUM_TILES; i++) {
		Model * temp = new Model(shader);
		glm::vec3 color = getColor((TILE_TYPE)i);
		glm::vec3 min(0, 0, 0);
		glm::vec3 max(size, 0, size);
		temp->makeTile(min, max, color, getTexture((TILE_TYPE)i));

		tiles.push_back(temp);
	}

	// Make outline box
	Model* temp = new Model(shader);
	temp->makeOutline(1,1,0.05,glm::vec3(1.0,1.0,0.0));
	outlineBox = temp;

	setPadding(TILE_TYPE::BLANK);

	baseLayer = new Model(shader);
	baseLayer->makeTile(glm::vec3(0, -0.01f, 0), glm::vec3(x * size, -0.01f, y * size), 
		getColor(TILE_TYPE::BASE_LAYER), getTexture(Ground::TILE_TYPE::BASE_LAYER));

	tilled = new AssimpModel(DIRT_MODEL, assimpShader);
}

Ground::~Ground()
{
	delete grid;
	for (Model* tile : tiles) {
		delete tile;
	}
}

Ground::TILE_TYPE Ground::getLoc(int x, int y)
{
	return grid[((padX+x)*totalY) + y + padY];
}

void Ground::setLoc(int x, int y, TILE_TYPE type)
{
	grid[((padX + x) * totalY) + y + padY] = type;
}

SceneNode* Ground::createSceneNodes(uint objectId)
{
	return new SceneNode(this, std::string("ground"), objectId);
}

void Ground::update(SceneNode * node)
{
	
}

void Ground::draw(SceneNode& node, const glm::mat4& viewProjMtx)
{

	glm::mat4 handlePadding = node.transform;

	// not sure why this +1 is needed
	handlePadding[3][0] = handlePadding[3][0] - (tileSize * (padX));
	handlePadding[3][2] = handlePadding[3][2] - (tileSize * (padY));

	// the passed in location is the truw start of thgame board
	baseLayer->draw(node, viewProjMtx);

	glm::mat4 tileMat = handlePadding;
	for (int i = 0; i < totalX; i++) {
		tileMat[3][2] = handlePadding[3][2];
		for (int j = 0; j < totalY; j++) {
;
			SceneNode temp(NULL, std::string(""), 0);
			temp.transform = tileMat;
			// Don't draw the normal tiles to make it transparent. Display the base layer underneath
			if (grid[(i * totalY) + j] != Ground::TILE_TYPE::NORMAL && grid[(i * totalY) + j] != Ground::TILE_TYPE::BLANK) {
				if (grid[(i * totalY) + j] == Ground::TILE_TYPE::TILLED) {
					float offset = tileSize / 2.0;
					temp.transform = glm::translate(temp.transform, glm::vec3(offset, 0.0, offset));
					temp.transform[0][0] = config.dirtScaler;
					temp.transform[1][1] = config.dirtScaler;
					temp.transform[2][2] = config.dirtScaler;
					tilled->draw(temp, viewProjMtx);
				}
				else {
					tiles[(int)grid[i * totalY + j]]->draw(temp, viewProjMtx);
				}
			}
			tileMat[3][2] += tileSize;
		}
		tileMat[3][0] += tileSize;
	}
	// Draw outline boxes
	tileMat = node.transform;
	for (int i = 0; i < tilesX; i++) {
		tileMat[3][2] = node.transform[3][2];
		for (int j = 0; j < tilesY; j++) {
			if (highlighted_grid[i * tilesY + j]) {
				SceneNode temp(NULL, std::string(""), 0);
				temp.transform = tileMat;
				outlineBox->draw(temp, viewProjMtx);
				highlighted_grid[i * tilesY + j] = false;
			}
			tileMat[3][2] += tileSize;
		}
		tileMat[3][0] += tileSize;
	}
}

void Ground::setPadding(TILE_TYPE type) {
	for (int i = 0; i < totalX; i++) {
		for (int j = 0; j < totalY; j++) {
			grid[(i * totalY) + j] = type;
		}
	}
}

int Ground::getX() {
	return tilesX;
}

int Ground::getY() {
	return tilesY;
}

glm::vec3 Ground::getColor(TILE_TYPE type)
{
	switch (type) {
	case TILE_TYPE::PATH:
		return glm::vec3(1.0);
	case TILE_TYPE::TILLED:
		return glm::vec3(1.0);
	case TILE_TYPE::NORMAL:
		return glm::vec3(1.0);
	case TILE_TYPE::BLANK:
		return glm::vec3(1.0);
	case TILE_TYPE::BASE_LAYER:
		return glm::vec3(1.0);
	}
	return glm::vec3(0);
}

const char* Ground::getTexture(TILE_TYPE type)
{
	switch (type) {
	case TILE_TYPE::PATH:
		return PATH_TEXTURE;
	case TILE_TYPE::TILLED:
		return NO_TEXTURE;
	case TILE_TYPE::NORMAL:
		return NO_TEXTURE;
	case TILE_TYPE::BLANK:
		return NO_TEXTURE;
	case TILE_TYPE::BASE_LAYER:
		return BASE_TEXTURE;
	}
	return NULL;
}

Ground * Ground::ground0(ClientParams& config, uint shader)
{
	int x = 24;
	int y = 20;
	float size = 0.5;
	int padX = 2;
	int padY = 2;

	Ground * ground0 = new Ground(config, x, y, size, padX, padY, shader, shader);

	for (int i = 0; i < x; i++) {
		for (int j = 0; j < y; j++) {
			if (i < 2 || i > x-3 || j > y-3) {
				ground0->setLoc(i, j, TILE_TYPE::PATH);
			}
			else {
				ground0->setLoc(i,j,TILE_TYPE::NORMAL);
			}
		}
	}
	return ground0;
}

void Ground::highlightTile(int x, int y) {
	if (x == -1 || y == -1) return;
	highlighted_grid[x * tilesY + y] = true;
}
