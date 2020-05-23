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

class GameStateLoader {
public:
    static void initTools(std::string key, std::string value, std::vector<Tool*>& tools, unsigned int& objectCount) {
		if (key == "NumTools") {
			int numTools = std::stoi(value);
			for (int i = 0; i < numTools; i++) {
				Tool* tool = new Tool();

				// For tools, direction and animation always initializes to 0
				tool->direction = new Direction(0.0);
				tool->animation = new Animation(0, 0);
				tool->objectId = objectCount;
				tools.push_back(tool);
                objectCount++;
			}
		} else if (key == "ToolPositions") {
			// Positions are delimited by 2 commas
			size_t pos = 0;
			std::vector<std::string> stringPositions;
			while ((pos = value.find(",,")) != std::string::npos) {
				stringPositions.push_back(value.substr(1, pos - 2));
				value.erase(0, pos + 2);
			}
			stringPositions.push_back(value.substr(1, value.length() - 2));

			// For each position in the form of x,y,z
			// Assign position objects to each tool
			int i = 0;
			for (std::string stringPosition : stringPositions) {
				float xyz[3];
				while ((pos = stringPosition.find(",")) != std::string::npos) {
					xyz[i % 3] = std::stof(stringPosition.substr(0, pos));
					stringPosition.erase(0, pos + 1);
					i++;
				}
				xyz[2] = std::stof(stringPosition);
				tools[i / 3]->position = new Position(xyz[0], xyz[1], xyz[2]);
				i++;
			}

		} else if (key == "ToolRadius") {
			int i = 0;
			std::stringstream s_stream(value); //create string stream from the string
			while (s_stream.good()) {
				std::string substr;
				std::getline(s_stream, substr, ','); //get first string delimited by comma
				tools[i]->boundingBoxRadius = std::stof(substr);
				i++;
			}
		} else if (key == "ToolType") {
			int i = 0;
			std::stringstream s_stream(value); //create string stream from the string
			while (s_stream.good()) {
				std::string substr;
				std::getline(s_stream, substr, ','); //get first string delimited by comma
				switch (std::stoi(substr)) {
				case 0:
					tools[i]->toolType = Tool::ToolType::WATER_CAN;
					tools[i]->capacity = 3.0f;
					break;
				case 1:
					tools[i]->toolType = Tool::ToolType::PLOW;
					break;
				case 2:
					tools[i]->toolType = Tool::ToolType::SEED;
					break;
				}
				i++;
			}
		}
    }

	static void initFloor(std::string key, std::string value, Floor* floor, bool& readingMap) {
		if (key == "MapBegin") {
			readingMap = true;
		} else if (key == "MapEnd") {
			floor->boundMinX = 0;
			floor->boundMaxX = floor->tiles[0].size() * Floor::TILE_SIZE;
			floor->boundMinZ = 0;
			floor->boundMaxZ = floor->tiles.size() * Floor::TILE_SIZE;
			readingMap = false;
		} else if (readingMap) {
			// Parse the map
			std::vector<Tile*> row;
			int currCol = 0;
			for (char tileChar : key) {
				Tile* tile = new Tile();
				tile->position = new Position(currCol, 0, floor->tiles.size());

				// Assign direction and tile type depending on character
				switch (tileChar) {
					case 'U':
						tile->direction = new Direction(Direction::DIRECTION_UP);
						tile->tileType = Tile::TYPE_ZOMBIE;
						break;

					case 'R':
						tile->direction = new Direction(Direction::DIRECTION_RIGHT);
						tile->tileType = Tile::TYPE_ZOMBIE;
						break;

					case 'D':
						tile->direction = new Direction(Direction::DIRECTION_DOWN);
						tile->tileType = Tile::TYPE_ZOMBIE;
						break;

					case 'L':
						tile->direction = new Direction(Direction::DIRECTION_LEFT);
						tile->tileType = Tile::TYPE_ZOMBIE;
						break;
					case 'T':
						tile->tileType = Tile::TYPE_TILLED;
						break;
					case '*':
					default:
						tile->direction = new Direction(0.0);
						tile->tileType = Tile::TYPE_NORMAL;

				}
                // Set progress time
                tile->plowProgressTime = 0.0f;
				row.push_back(tile);
				currCol++;
			}
			floor->tiles.push_back(row);
		}
		else {
			if (key == "TileSize") {
				floor->tileSize = std::stof(value);
			} else if (key == "ZombieBaseTile") {
				std::string rowCol = value.substr(1, value.length() - 2);
				size_t commaPos = rowCol.find_first_of(',');

				// Now get row and col
				int row = std::stoi(rowCol.substr(0, commaPos));
				int col = std::stoi(rowCol.substr(commaPos + 1));
				floor->setZombieBaseTile(row, col);
			} else if (key == "ZombieFinalTile") {
				std::string rowCol = value.substr(1, value.length() - 2);
				size_t commaPos = rowCol.find_first_of(',');

				// Now get row and col
				floor->zombieFinalTileRow = std::stoi(rowCol.substr(0, commaPos));
				floor->zombieFinalTileCol = std::stoi(rowCol.substr(commaPos + 1));
			} else if (key == "PlowExpireTime") {
				floor->plowExpireTime = std::stof(value);
			}
		}
	}

	static void initGameObject(std::string key, std::string value, GameObject* obj, unsigned int& objectCount) {
		if (key == "SetObjectId") {
			obj->objectId = objectCount++;
		} else if (key == "Position") {
			std::string stringPos = value.substr(1, value.length() - 2);
			size_t firstCommaPos = stringPos.find_first_of(',');
			size_t lastCommaPos = stringPos.find_last_of(',');

			float x = std::stof(stringPos.substr(0, firstCommaPos));
			float y = std::stof(stringPos.substr(firstCommaPos + 1, lastCommaPos - firstCommaPos - 1));
			float z = std::stof(stringPos.substr(lastCommaPos + 1));
			obj->position = new Position(x, y, z);
		} else if (key == "Direction") {
			obj->direction = new Direction(std::stof(value));
		} else if (key == "Animation") {
			std::string stringAnim = value.substr(1, value.length() - 2);
			size_t commaPos = stringAnim.find_first_of(',');

			int animationType = std::stoi(stringAnim.substr(0, commaPos));
			float animationFrame = std::stof(stringAnim.substr(commaPos + 1));
			obj->animation = new Animation(animationType, animationFrame);
		} else if (key == "Radius") {
			obj->boundingBoxRadius = std::stof(value);
		} else if (key == "SeedType") {
			// SeedShack only
			SeedShack* seedShack = (SeedShack*)obj;
			if (value == "CORN") {
				seedShack->seedType = Plant::PlantType::CORN;
			} else if (value == "CACTUS") {
				seedShack->seedType = Plant::PlantType::CACTUS;
            }
		} else if (key == "MaxHealth") {
			HomeBase* homeBase = (HomeBase*)obj;
			homeBase->maxHealth = std::stoi(value);
			homeBase->health = homeBase->maxHealth;
		}
	}
};

#endif