#ifndef _GAME_STATE_H
#define _GAME_STATE_H

#include "Player.hpp"
#include "Plant.hpp"
#include "Zombie.hpp"
#include "Tool.hpp"
#include "Tile.hpp"
#include "SeedShack.hpp"
#include "WaterTap.hpp"
#include "Message.hpp"
#include "Floor.hpp"
#include "GameStateLoader.hpp"

#include <cmath>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/serialization/vector.hpp>

class GameState {
public:
    GameState() : seedShack(nullptr), waterTap(nullptr) {}

    void loadFromConfigFile(std::string filename) {
        // load from file in here
        std::ifstream myfile;

        myfile.open(filename.c_str());

        if (!myfile.is_open()) {
            std::cerr << "Failed to open config file: " << filename << std::endl;
            return;
        }

        std::string line;
        std::string header;
        bool readingMap = false;

        // Run until end of file
        while (myfile.good()) {
            std::getline(myfile, line);

            // Skip empty lines and comments
            if (line.length() == 0 || line[0] == '#') {
                continue;
            }

            // Change the [header], call different functions depending on current header
            if (line[0] == '[') {
                header = line.substr(1, line.length() - 2);
            } else {
                size_t equalPos = line.find_first_of('=');
                std::string key = line.substr(0, equalPos);
                std::string value = line.substr(equalPos + 1);

                if (header == "Tools") {
                    GameStateLoader::initTools(key, value, tools, objectCount);
                } else if (header == "Floor") {
                    GameStateLoader::initFloor(key, value, floor, readingMap);
                } else if (header == "SeedShack") {
                    GameStateLoader::initGameObject(key, value, seedShack, objectCount);
                } else if (header == "WaterTap") {
                    GameStateLoader::initGameObject(key, value, waterTap, objectCount);
                }
            }
        }
        myfile.close();

        // Add tools, seed shack and water tap to game object id map
        for (Tool* tool : tools) {
            gameObjectMap[tool->objectId] = tool;
        }
        gameObjectMap[seedShack->objectId] = seedShack;
        gameObjectMap[waterTap->objectId] = waterTap;
    }

    void init(int tick_rate) {
        tickRate = tick_rate;
        floor = new Floor();
        seedShack = new SeedShack();
        waterTap = new WaterTap();
        //init();
	}

    void init() {
        // Init the default state here
        // TODO: change them later
        //int NUM_OF_PLAYER = 3;
        //int NUM_OF_ZOMBIE = 3;

        // Init players
        /*for (int i = 0; i < NUM_OF_PLAYER; i++) {
            Position* playerPosition = new Position(i*3, 0, 0);
            Direction* playerDirection = new Direction(0.0);
            Animation* playerAnimation = new Animation(0, 0);
            Color* playerColor = new Color(100, 100, 100);
            float playerRadius = 1.0; // temp radius
            players.push_back(
                new Player(
                    playerPosition, playerDirection,
                    playerAnimation, objectCount, playerRadius, playerColor, i
                )
            );
            gameObjectMap[objectCount] = players[i];
            objectCount++;
        }*/

        // Init zombies
        /*for (int i = 0; i < NUM_OF_ZOMBIE; i++) {
            Position* zombiePosition = new Position(i*3, 0, 5.0);
            Direction* zombieDirection = new Direction(0.0);
            Animation* zombieAnimation = new Animation(0, 0);
            zombies.push_back(
                new Zombie(zombiePosition, zombieDirection, zombieAnimation, objectCount)
            );
            objectCount++;
        }*/

        // Init tools
        for (int i = 0; i < 2; i++) {
            Position* toolPosition = new Position(i, 0, i);
            Direction* toolDirection = new Direction(0.0);
            Animation* toolAnimation = new Animation(0, 0);
            float toolRadius = 1.0f;
            tools.push_back(new Tool(
                toolPosition, toolDirection, toolAnimation, objectCount, toolRadius, i, 0, false
            ));
            gameObjectMap[objectCount] = tools[i];
            objectCount++;
        }

        floor = new Floor();
        floor->init();

        // Init seed shack and water tap
        Position* seedShackPosition = new Position(3, 0, 3);
        Direction* seedShackDirection = new Direction(0);
        Animation* seedShackAnimation = new Animation(0, 0);
        float seedShackRadius = 1.0f;
        seedShack = new SeedShack(
            seedShackPosition, seedShackDirection, 
            seedShackAnimation, objectCount, seedShackRadius
        );
        gameObjectMap[objectCount] = seedShack;
        objectCount++;

        Position* waterTapPosition = new Position(2, 0, 2);
        Direction* waterTapDirection = new Direction(0);
        Animation* waterTapAnimation = new Animation(0, 0);
        float waterTapRadius = 1.0f;
        waterTap = new WaterTap(
            waterTapPosition, waterTapDirection, 
            waterTapAnimation, objectCount, waterTapRadius
        );
        gameObjectMap[objectCount] = waterTap;
        objectCount++;
    }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & players;
        ar & plants;
        ar & zombies;
        ar & tools;
        ar & floor;
        ar & seedShack;
        ar & waterTap;
    }

    ~GameState() {
        for (Player* player: players) {
            delete player;
		}

        for (Plant* plant: plants) {
            delete plant;
		}

        for (Zombie* zombie: zombies) {
            delete zombie;
        }

        for (Tool* tool: tools) {
            delete tool;
        }

        delete floor;
        delete seedShack;
        delete waterTap;
    }

    void updatePlayer(int opCode, Player* player) {
        //std::cout << "Before update angle = " << player->direction->angle << std::endl;
        switch (opCode) {
            case OPCODE_PLAYER_MOVE_FREEZE:
                player->moveState = Player::MoveState::FREEZE;
                break;
            case OPCODE_PLAYER_MOVE_DOWN:
                player->moveState = Player::MoveState::DOWN;
                break;
            case OPCODE_PLAYER_MOVE_LOWER_RIGHT:
                player->moveState = Player::MoveState::LOWER_RIGHT;
                break;
            case OPCODE_PLAYER_MOVE_RIGHT:
                player->moveState = Player::MoveState::RIGHT;
                break;
            case OPCODE_PLAYER_MOVE_UPPER_RIGHT:
                player->moveState = Player::MoveState::UPPER_RIGHT;
                break;
            case OPCODE_PLAYER_MOVE_UP:
                player->moveState = Player::MoveState::UP;
                break;
            case OPCODE_PLAYER_MOVE_UPPER_LEFT:
                player->moveState = Player::MoveState::UPPER_LEFT;
                break;
            case OPCODE_PLAYER_MOVE_LEFT:
                player->moveState = Player::MoveState::LEFT;
                break;
            case OPCODE_PLAYER_MOVE_LOWER_LEFT:
                player->moveState = Player::MoveState::LOWER_LEFT;
                break;
            case OPCODE_PLAYER_INTERACT:
                if (player->holding) {
                    // TODO: facing direction check to use tool or drop the tool

                    // Drop tool
                    Tool* tool = (Tool*)(gameObjectMap[player->heldObject]);
                    tool->position->x = player->position->x;
                    tool->position->y = player->position->y;
                    tool->position->z = player->position->z;
                    tool->heldBy = 0;
                    tool->held = false;
                    player->heldObject = 0;
                    player->holding = false;
                }
                else {
                    // Loop through tools and check if player collides with them
                    // Pick up the closest tool
                    Tool* currTool = nullptr;
                    float minDistance = std::numeric_limits<float>::max();
                    for (Tool* tool: tools) {
                        float dist = player->distanceTo(tool);
                        std::cout << "Distance to tool is " << dist << std::endl;
                        if (dist < minDistance) {
                            currTool = tool;
                            minDistance = dist;
                        }
                    }

                    // Make sure tool is within collision range and is not held by others 
                    if (currTool && player->collideWith(currTool) && currTool->heldBy == 0) {
                        std::cout << "Pick up tool at (" << currTool->position->x << ", " << currTool->position->z << ")" << std::endl;
                        player->holding = true;
                        player->heldObject = currTool->objectId;
                        currTool->heldBy = player->objectId;
                        currTool->held = true;
                    }
                }
                break;
        }
        //std::cout << "After update angle = " << player->direction->angle << std::endl;
    }

    void addPlayer(Player *player) {
        players.push_back(player);
    }

    void removePlayer(Player *player) {
        auto it = std::find(players.begin(), players.end(), player);
        if (it != players.end()) {
            players.erase(it);
        }

        // Drop tool if player disconnects
        if (player->heldObject != 0) {
            Tool* tool = (Tool*)(gameObjectMap[player->heldObject]);
            tool->position->x = player->position->x;
            tool->position->y = player->position->y;
            tool->position->z = player->position->z;
            tool->heldBy = 0;
            tool->held = false;
            player->heldObject = 0;
            player->holding = false;
        }
    }

    void update() {
        updatePlayers();
        updateZombies();
        tick++;
    }

    void updatePlayers() {
        for (Player* player : players) {
            // 1. Update position
            player->move();
        }
    }

    void updateZombies() {
        // Spawn zombie every second
        if (tick % tickRate == 0) {
            // TODO: Need to find out the position of the tile
            // Position right now uses indices
            Position* zombieBasePos = floor->zombieBaseTile->position;
            Direction* zombieBaseDir = floor->zombieBaseTile->direction;
            Position* zombiePosition = new Position(
                zombieBasePos->x + Tile::TILE_PAD_X,
                zombieBasePos->y,
                zombieBasePos->z + Tile::TILE_PAD_Z
            );
            Direction* zombieDirection = new Direction(
                zombieBaseDir->angle
            );
            Animation* zombieAnimation = new Animation(0, 0);
            float zombieRadius = 1.0f;
            Zombie* zombie = new Zombie(
                zombiePosition, zombieDirection,
                zombieAnimation, objectCount++, zombieRadius
            );
            zombies.push_back(zombie);
		}

        auto i = std::begin(zombies);
        while (i != std::end(zombies)) {
            Zombie* zombie = (*i);
            int row = zombie->position->z - Tile::TILE_PAD_Z;
            int col = zombie->position->x - Tile::TILE_PAD_X;

            // Check if remove current zombie (final tile / hp = 0, ...)
            if (row == floor->zombieFinalTileRow
                && col == floor->zombieFinalTileCol) {
                i = zombies.erase(i);
                continue;
            }

            // Move current zombie
            Tile* currTile = floor->tiles[row][col];
            Direction* currDir = zombie->direction;
            if (currDir->directionEquals(Direction::DIRECTION_DOWN)) {
                zombie->position->z += Zombie::STEP_SIZE;
            } else if (currDir->directionEquals(Direction::DIRECTION_RIGHT)) {
                zombie->position->x += Zombie::STEP_SIZE;
            } else if (currDir->directionEquals(Direction::DIRECTION_UP)) {
                zombie->position->z -= Zombie::STEP_SIZE;
            } else if (currDir->directionEquals(Direction::DIRECTION_LEFT)) {
                zombie->position->x -= Zombie::STEP_SIZE;
            }

            // Rotate current zombie
            zombie->direction->angle = currTile->direction->angle;

            i++;
        }
    }

    // We could use other data structures, for now use a list
    std::vector<Player*> players; // Up to 4 players?
    std::vector<Plant*> plants;
    std::vector<Zombie*> zombies;
    std::vector<Tool*> tools;

    Floor* floor;
    SeedShack* seedShack; // Assuming there's 1 place to get seeds
    WaterTap* waterTap;

    // GameObject Map
    std::unordered_map<unsigned int, GameObject*> gameObjectMap;

    // start of at 100 so we can portnetially reserve special constants
    unsigned int objectCount = 100;

    // tick count
    long long tick = 0;
    int tickRate;

};

#endif
