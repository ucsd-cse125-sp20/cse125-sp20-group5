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
        deltaTime = 1.0f / tickRate;
        floor = new Floor();
        seedShack = new SeedShack();
        waterTap = new WaterTap();
        init();
	}

    void init() {
        // Init the default state here
        // TODO: change them later
        //int NUM_OF_PLAYER = 3;
        //int NUM_OF_ZOMBIE = 3;

        // Init players
        /*
        for (int i = 0; i < NUM_OF_PLAYER; i++) {
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
        // Init Corn
        Position* cornPosition = new Position(5, 0, 5);
        Direction* cornDirection = new Direction(0.0);
        Animation* cornAnimation = new Animation(0, 0);
        TowerRange* cornRange = new TowerRange(3);
        float cornRadius = 1.0f;
        Plant* corn = new Plant(cornPosition, cornDirection, cornAnimation, objectCount, cornRadius, cornRange, Plant::PlantType::CORN, Plant::GrowStage::SEED);
        corn->growExpireTime = 2.0f;
        plants.push_back(corn);
        gameObjectMap[objectCount] = corn;
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
            case OPCODE_PLAYER_ACTION:
                player->shouldPerformAction = true;
                break;
            case OPCODE_PLAYER_INTERACT:
                player->shouldInteract = true;
                
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

            float x_offset = std::cos(player->direction->angle) * player->boundingBoxRadius;
            float z_offset = std::sin(player->direction->angle) * player->boundingBoxRadius;
            tool->position->x = player->position->x - x_offset;
            tool->position->y = player->position->y;
            tool->position->z = player->position->z + z_offset;
            tool->direction->angle = player->direction->angle;
            tool->heldBy = 0;
            tool->held = false;
            player->heldObject = 0;
            player->holding = false;
        }
    }

    void update() {
        playersPerformAction();
        playersInteract();
        updatePlants();
        updateZombies();
        updatePlayersPosition();
        tick++;
    }

    void playersPerformAction() {
        for (Player* player : players) {
            // Check if player pressed e during this tick
            if (!player->shouldPerformAction) {
                continue;
            }
            player->shouldPerformAction = false;

            // Check if player is holding something
            if (!player->holding) {
                // Check if player is interacting with seed plant
                continue;
            }
            /*
            std::vector<Tile*> nearTiles;
            for (int row = player->currRow - 1; row <= player->currRow + 1; row++) {
               for (int col = player->currCol - 1; col <= player->currCol + 1; col++) {
                   if (row >= 0 && row < floor->tiles.size() && col >= 0 && col < floor->tiles[0].size()) {
                       nearTiles.push_back(floor->tiles[row][col]);
                   }
               }
            }
            */

            Tool* tool = (Tool*)gameObjectMap[player->heldObject];
            switch (tool->toolType) {

            // WATER_CAN
            case Tool::ToolType::WATER_CAN: {
                if (player->collideWith(waterTap) && tool->remainingWater < tool->capacity) {
                    tool->remainingWater += deltaTime;
                    std::cout << "Current watering can remaining water: " << tool->remainingWater << std::endl;
                    break;
				}
                if (tool->remainingWater <= 0) {
                    std::cout << "Not enough to watering plants" << std::endl;
                    break;
                }
                Plant* currPlant = nullptr;
                float minDistance = std::numeric_limits<float>::max();
                for (Plant* plant : plants) {
                    float dist = player->distanceTo(plant);
                    std::cout << "Distance to plant is " << dist << std::endl;
                    if (dist < minDistance) {
                        currPlant = plant;
                        minDistance = dist;
                    }
                }

                // Water the nearest plant
                if (currPlant && player->collideWith(currPlant)) {
                    std::cout << "Watering plant at (" << currPlant->position->x << ", " << currPlant->position->z << ")" << std::endl;
                    if (currPlant->growStage != Plant::GrowStage::GROWN) {
                        currPlant->growProgressTime += deltaTime;
                        tool->remainingWater -= deltaTime;
                        std::cout << "Current plant growing progress: " << currPlant->growProgressTime << std::endl;
                        std::cout << "Current watering can remaining water: " << tool->remainingWater << std::endl;
                    }
                    else {
                        std::cout << "Plant is already grown" << std::endl;
                    }
                }
                break;
            }

            // PLOW
            case Tool::ToolType::PLOW:
                break;
            
            // TODO: need to generalize for all seeds
            // SEED_CORN
            case Tool::ToolType::SEED:
                // Get player's tile (only plant on non-zombie and plowed tiles)
                Tile* currTile = floor->tiles[player->currRow][player->currCol];
                if (currTile->tileType != Tile::TYPE_ZOMBIE && currTile->plantId == 0) {
                    player->holding = false;
                    player->heldObject = 0;

                    // Replace it with a plant
                    Position* plantPosition = new Position(currTile->position);
                    plantPosition->x += Tile::TILE_PAD_X;
                    plantPosition->z += Tile::TILE_PAD_Z;
                    Plant* plant = new Plant(
                        plantPosition,
                        new Direction(player->direction), // ??
                        new Animation(0, 0),
                        objectCount,
                        1.0f,
                        new TowerRange(3.0f),
                        tool->seedType,
                        Plant::GrowStage::SEED
                    );
                    plant->growExpireTime = 2.0f;
                    plant->attackPower = 50;
                    plant->currAttackTime = 0.0f;
                    plant->attackInterval = 1.0f;
                    plants.push_back(plant);
                    std::cout << "Seed planted" << std::endl;

                    // Delete the seed tool
                    auto it = std::find(tools.begin(), tools.end(), tool);
                    if (it != tools.end()) {
                        tools.erase(it);
                    }
                    delete tool;

				}
                break;
            }
        }
    }

    void playersInteract() {
        for (Player* player : players) {
            if (!player->shouldInteract) {
                continue;
            }
            player->shouldInteract = false;
            if (player->holding) {
                // TODO: facing direction check to use tool or drop the tool

                // drop tool
                Tool* tool = (Tool*)gameObjectMap[player->heldObject];
                float x_offset = std::cos(player->direction->angle) * player->boundingBoxRadius;
                float z_offset = std::sin(player->direction->angle) * player->boundingBoxRadius;
                tool->position->x = player->position->x - x_offset;
                tool->position->y = player->position->y;
                tool->position->z = player->position->z + z_offset;
                tool->direction->angle = player->direction->angle;
                tool->heldBy = 0;
                tool->held = false;
                player->heldObject = 0;
                player->holding = false;
            } else {
                // Loop through tools and check if player collides with them
                // Pick up the closest tool
                Tool* currTool = nullptr;
                float minDistance = std::numeric_limits<float>::max();
                for (Tool* tool : tools) {
                    float dist = player->distanceTo(tool);
                    std::cout << "Distance to tool is " << dist << std::endl;
                    if (dist < minDistance) {
                        currTool = tool;
                        minDistance = dist;
                    }
                }

                // Check distance to seedShack as well
                bool seedShackClosest = false;
                float dist = player->distanceTo(seedShack);
                std::cout << "Distance to seedShack is " << dist << std::endl;
                if (dist < minDistance) {
                    minDistance = dist;
                    seedShackClosest = true;
                }

                if (seedShackClosest && player->collideWith(seedShack)) {
                    // Create a seed (Tool)
                    Tool* seed = new Tool(
                        new Position(player->position),
                        new Direction(player->direction),
                        new Animation(),
                        objectCount,
                        0.25f,
                        Tool::ToolType::SEED,
                        player->objectId,
                        true
                    );
                    seed->seedType = seedShack->seedType;
                    gameObjectMap[objectCount++] = seed;
                    tools.push_back(seed);
                    player->holding = true;
                    player->heldObject = seed->objectId;
                    std::cout << "Pick up seed from seed shack at (" << seedShack->position->x << ", " << seedShack->position->z << ")" << std::endl;
                }
                else {
                    // interacting with tools
                    // Make sure tool is within collision range and is not held by others 
                    if (currTool && player->collideWith(currTool) && currTool->heldBy == 0) {
                        std::cout << "Pick up tool at (" << currTool->position->x << ", " << currTool->position->z << ")" << std::endl;
                        player->holding = true;
                        player->heldObject = currTool->objectId;
                        currTool->heldBy = player->objectId;
                        currTool->held = true;
                        currTool->direction->angle = player->direction->angle;
                    }
                }

            }
        }
    }

    void updatePlants() {
        for (Plant* plant : plants) {
            if (plant->growStage == Plant::GrowStage::GROWN) {
                // Grown stage
            }
            else {
                // Still Growing
                if (plant->growProgressTime >= plant->growExpireTime) {
                    std::cout << "Plant growth complete, going next stage" << std::endl;
                    plant->growStage++;
                    plant->growExpireTime = 2.0f;
                    plant->growProgressTime = 0.0f;
                }

            }

            // Attack zombies
            attackZombies(plant);

            // TODO: handle spawn bullets
        }
    }


    void updatePlayersPosition() {
        for (Player* player : players) {
            Position prevPos(player->position);
            // 1. Update position
            player->move(deltaTime);

            // 2. Check if collide with zombies
            for (Zombie* zombie : zombies) {
                if (player->collideWith(zombie)) {
                    std::cout << "Collide with zombie" << std::endl;
                    float dx = player->position->x - prevPos.x;
                    float dz = player->position->z - prevPos.z;
                    player->position->x = prevPos.x - 10 * dx;
                    player->position->z = prevPos.z - 10 * dz;
                    break;
                }
            }

            // 3. Check if collide with wall
            if (player->position->x < 0 || player->position->x > floor->tiles[0].size() * 1.0) {
                std::cout << "Collide with wall" << std::endl;
                player->position->x = prevPos.x;
                continue;
            }
            if (player->position->z < 0 || player->position->z > floor->tiles.size() * 1.0) {
                std::cout << "Collide with wall" << std::endl;
                player->position->z = prevPos.z;
                continue;
            }

            // 4. Check if collide with tools
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
            zombie->health = 100;
            zombie->maxHealth = 100;
            zombies.push_back(zombie);
		}

        for (auto i = std::begin(zombies); i != std::end(zombies); i++) {
            Zombie* zombie = (*i);
            float paddingZ = 0, paddingX = 0;

            // Move current zombie
            Direction* currDir = zombie->direction;
            if (currDir->directionEquals(Direction::DIRECTION_DOWN)) {
                zombie->position->z += Zombie::SPEED * deltaTime;
                paddingZ -= Tile::TILE_PAD_Z;
            } 
            else if (currDir->directionEquals(Direction::DIRECTION_RIGHT)) {
                zombie->position->x += Zombie::SPEED * deltaTime;
                paddingX -= Tile::TILE_PAD_X;
            } 
            else if (currDir->directionEquals(Direction::DIRECTION_UP)) {
                zombie->position->z -= Zombie::SPEED * deltaTime;
                paddingZ += Tile::TILE_PAD_Z;
            } 
            else if (currDir->directionEquals(Direction::DIRECTION_LEFT)) {
                zombie->position->x -= Zombie::SPEED * deltaTime;
                paddingX += Tile::TILE_PAD_X;
            }

            // Calculate the current effecting tile's row&column
            int row = zombie->position->z + paddingZ;
            int col = zombie->position->x + paddingX;
            Tile* currTile = floor->tiles[row][col];

            // Check if remove current zombie (final tile / hp = 0, ...)
            bool reachedFinalTile = row == floor->zombieFinalTileRow
                && col == floor->zombieFinalTileCol;

            if (reachedFinalTile || zombie->health <= 0) {
                i = zombies.erase(i);
                continue;
            }

            // Rotate current zombie
            if (currTile->tileType != Tile::TYPE_NORMAL)
                zombie->direction->angle = currTile->direction->angle;
        }
    }

    void attackZombies(Plant* plant) {
        if (plant->growStage != Plant::GrowStage::GROWN) {
            return;
        }

        if (plant->currAttackTime < plant->attackInterval) {
            plant->currAttackTime += deltaTime;
            return;
        }

        std::cout << "Perform attack to zombies" << std::endl;
        for (Zombie* zombie : zombies) {
            switch (plant->plantType) {
            case Plant::PlantType::CORN:
                //std::cout << "Distance to zombie is " << zombie->distanceTo(plant) << std::endl;
                if (zombie->distanceTo(plant) < plant->range->rangeDistance) {
                    zombie->health -= plant->attackPower;
                }
                break;
            }
        }
        plant->currAttackTime = 0.0f;
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
    long long tick;
    int tickRate;
    float deltaTime;
};
#endif
