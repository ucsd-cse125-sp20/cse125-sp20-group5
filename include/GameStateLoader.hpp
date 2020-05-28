#ifndef _GAME_STATE_LOADER_H
#define _GAME_STATE_LOADER_H

#include "Tool.hpp"
#include "Tile.hpp"
#include "GameObject.hpp"
#include "Message.hpp"
#include "Floor.hpp"
#include "Plant.hpp"
#include "HomeBase.hpp"

#include <fstream>
#include <sstream>
#include <string>

class GameState;

class GameStateLoader {
public:
	static void initTools(std::string key, std::string value, std::vector<Tool*>& tools, unsigned int& objectCount);

	static void initFloor(std::string key, std::string value, bool& readingMap, GameState* gameState);

	static void initGameObject(std::string key, std::string value, GameObject* obj, unsigned int& objectCount);
};

#endif