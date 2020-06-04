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
#include "HomeBase.hpp"
#include "ZombieWaveManager.h"
#include "CactusBullet.hpp"
#include "ServerParams.h"
#include "Obstacle.hpp"

#include <cmath>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/serialization/vector.hpp>

class GameState {
public:
    GameState() : waterTap(nullptr) {}

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
                    GameStateLoader::initFloor(key, value, readingMap, this);
                } else if (header == "SeedShack1") {
                    GameStateLoader::initGameObject(key, value, seedShacks[0], objectCount);
                } else if (header == "SeedShack2") {
                    GameStateLoader::initGameObject(key, value, seedShacks[1], objectCount);
                } else if (header == "WaterTap") {
                    GameStateLoader::initGameObject(key, value, waterTap, objectCount);
                } else if (header == "HomeBase") {
                    GameStateLoader::initGameObject(key, value, homeBase, objectCount);
                }
            }
        }
        myfile.close();

        // Initialize HomeBase position using the zombie final tile
        Tile* baseTile = floor->tiles[floor->zombieFinalTileRow][floor->zombieFinalTileCol];
        homeBase->position = new Position(baseTile->getCenterPosition());
        homeBase->direction = new Direction(baseTile->direction->getOppositeDirection());
        homeBase->animation = new Animation(0, 0);


        initUnPlowableTiles();

        // Add tools, seed shack and water tap to game object id map
        for (Tool* tool : tools) {
            gameObjectMap[tool->objectId] = tool;
        }

        for (SeedShack* seedShack: seedShacks) {
            gameObjectMap[seedShack->objectId] = seedShack;
		}
        gameObjectMap[waterTap->objectId] = waterTap;
        gameObjectMap[homeBase->objectId] = homeBase;
    }

    void init(ServerParams& config) {
        this->config = config;
        totalWaveNum = config.totalWaveNum;
        tickRate = config.tickrate;
        deltaTime = 1.0f / tickRate;
        isGameOver = false;
        floor = new Floor();

        // TODO: refactor loader code, just assume 2 for now
        for (unsigned int i = 0; i < 2; i++) {
            seedShacks.push_back(new SeedShack());
        }

        waterTap = new WaterTap();
        homeBase = new HomeBase();
        zombieWaveManager = new ZombieWaveManager(this);
	}


    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & players;
        ar & plants;
        ar & zombies;
        ar & tools;
        ar & floor;
        ar & seedShacks;
        ar & bullets;
        ar & obstacles;
        ar & waterTap;
        ar & homeBase;
        ar & waveNum;
        ar & totalWaveNum;
        ar & numZombiesKilled;
        ar & isGameOver;
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

        for (SeedShack* seedShack: seedShacks) {
            delete seedShack;
        }

        for (CactusBullet* bullet: bullets) {
            delete bullet;
        }

        for (Obstacle* obstacle: obstacles) {
            delete obstacle;
        }

        for (Position* initPos : playerInitPositions) {
            delete initPos;
        }

        delete floor;
        delete waterTap;
        delete homeBase;
    }

    void updatePlayer(int opCode, Player* player) {
        //std::cout << "Before update angle = " << player->direction->angle << std::endl;
        if (player->isDead) {
            player->moveState = Player::MoveState::FREEZE;
            player->shouldPerformAction = false;
            player->shouldInteract = false;

            if (player->holding) {
                dropTool(player);
			}
            return;
        }
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
            case OPCODE_PLAYER_START_ACTION:
                player->shouldPerformAction = true;
                break;
            case OPCODE_PLAYER_END_ACTION:
                player->shouldPerformAction = false;
                break;
            case OPCODE_PLAYER_INTERACT:
                player->shouldInteract = true;
                break;
        }
        //std::cout << "After update angle = " << player->direction->angle << std::endl;
    }

    void addPlayer(Player *player) {
        Position* spawnPos = new Position(playerInitPositions[player->playerId % config.maxNumPlayers]);
        player->position = spawnPos;
        player->currRow = spawnPos->z / Floor::TILE_SIZE;
        player->currCol = spawnPos->x / Floor::TILE_SIZE;
        players.push_back(player);
    }

    void addPlayerBeforeStart(Player* player) {
        tempPlayers.push_back(player);
    }

    void setInitPlayerPositions() {
        for (Player* player : tempPlayers) {
            Position* spawnPos = new Position(playerInitPositions[player->playerId % config.maxNumPlayers]);
            player->position = spawnPos;
            player->currRow = spawnPos->z / Floor::TILE_SIZE;
            player->currCol = spawnPos->x / Floor::TILE_SIZE;
            players.push_back(player);
        }
        tempPlayers.clear();
    }

    void removePlayer(Player *player) {
        auto it = std::find(players.begin(), players.end(), player);
        if (it != players.end()) {
            players.erase(it);
        }

        // Drop tool if player disconnects
        if (player->heldObject != 0) {
            dropTool(player);
        }
    }

    void removeTempPlayer(Player* player) {
        auto it = std::find(tempPlayers.begin(), tempPlayers.end(), player);
        if (it != tempPlayers.end()) {
            tempPlayers.erase(it);
        }
    }

    void update() {
        tick++;
        currentTime += deltaTime;
        if (checkIsGameOver()) {
            std::cout << "Game Over" << std::endl;
            isGameOver = true;
            return;
        }
        playersPerformAction();
        playersInteract();
        updatePlants();
        updateTools();
        updateBullets();
        updateZombies();
        updatePlayersPosition();
        updatePlayersHighlight();
    }

    void playersPerformAction() {
        //Reset watering state
        waterTap->animation->animationType = WaterTap::WaterTapAnimation::IDLE;
        for (Player* player : players) {
            // Reset animation to MOVE
            player->animation->animationType = Player::PlayerAnimation::MOVE;

            // Check if player pressed e during this tick
            if (!player->shouldPerformAction) {
                /*
                if (waterTap->playerIdInUse == player->objectId) {
                    waterTap->playerIdInUse = 0;
				}*/
                if (player->highlightTileRow != -1 && player->highlightTileCol != -1) {
                    Tile* currTile = floor->tiles[player->highlightTileRow][player->highlightTileCol];
                    if (player->holding) {
                        Tool* tool = (Tool*)gameObjectMap[player->heldObject];
                        switch (tool->toolType) {
                        case Tool::ToolType::PLOW:
                            currTile->plowProgressTime = 0;
                            break;
                        case Tool::ToolType::PESTICIDE: {
                            Plant* plant = (Plant*) gameObjectMap[currTile->plantId];
                            if (plant) {
                                plant->currSprayTime = 0;
                            }
                            else {
                                std::cerr << "Cannot use PESTICIDE because tile's plantId = 0" << std::endl;
                            }
                            break;
                        }
                        case Tool::ToolType::FERTILIZER: {
                            Plant* plant = (Plant*) gameObjectMap[currTile->plantId];
                            if (plant) {
                                plant->currFertilizeTime = 0;
                            }
                            else {
                                std::cerr << "Cannot use FERTILIZER because tile's plantId = 0" << std::endl;
                            }
                            break;
                        }
                        }
                    }

                }
                continue;
            }

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
                // Check if player is highlighting water tap
                /*
                if (waterTap->playerIdInUse == 0) {
                    waterTap->playerIdInUse = player->objectId;
                }*/

                if (player->highlightObjectId == waterTap->objectId && tool->remainingWater < tool->capacity) {
                    tool->remainingWater += deltaTime;
                    waterTap->animation->animationType = WaterTap::WaterTapAnimation::WATER;
                    //std::cout << "Current watering can remaining water: " << tool->remainingWater << std::endl;
                    player->animation->animationType = Player::PlayerAnimation::WATER;
                    break;
                }

                if (tool->remainingWater <= 0) {
                    //std::cout << "Not enough to watering plants" << std::endl;
                    break;
                }

                // Here if player holds watering can, nonzero object ids must refer to Plant
                if (player->highlightObjectId != 0) {
                    Plant* currPlant = (Plant*)gameObjectMap[player->highlightObjectId];
                    if (currPlant->growStage != Plant::GrowStage::GROWN) {
                        if (currPlant->cooldownTime <= 0) {
                            currPlant->growProgressTime += deltaTime;
                            tool->remainingWater -= deltaTime;
                            //std::cout << "Watering plant at (" << currPlant->position->x << ", " << currPlant->position->z << ")" << std::endl;
                            //std::cout << "Current plant growing progress: " << currPlant->growProgressTime << std::endl;
                            //std::cout << "Current watering can remaining water: " << tool->remainingWater << std::endl;
                            player->animation->animationType = Player::PlayerAnimation::WATER;
                        }
                        else {
                            //std::cout << "Plant growing in cooldown. Cannot water" << std::endl;
                        }
                    }
                    else {
                        //std::cout << "Plant is already grown" << std::endl;
                    }
                }
                break;
            }

            // PLOW
            case Tool::ToolType::PLOW:
                // Attempt to plow if a tile is highlighted
                if (player->highlightTileRow != -1 && player->highlightTileCol != -1) {
                    Tile* currTile = floor->tiles[player->highlightTileRow][player->highlightTileCol];
                    if (currTile->plowProgressTime < floor->plowExpireTime) {
                        currTile->plowProgressTime += deltaTime;
                        //std::cout << "Current tile plowing progress: " << currTile->plowProgressTime << std::endl;
                        player->animation->animationType = Player::PlayerAnimation::PLOUGH;
                    }
                    else {
                        currTile->tileType = Tile::TYPE_TILLED;
                        currTile->canPlow = false;
                        //std::cout << "Tile is plowed" << std::endl;
                    }
                }
                break;
            
            // TODO: need to generalize for all seeds
            // SEED_CORN
            case Tool::ToolType::SEED:
                // Attempt to plant seed if tile is highlighted
                if (player->highlightTileRow != -1 && player->highlightTileCol != -1) {
                    Tile* currTile = floor->tiles[player->highlightTileRow][player->highlightTileCol];

                    player->holding = false;
                    player->heldObject = 0;

                    // Replace seed with a plant
                    Plant* plant = Plant::buildPlant(config, tool->seedType);

                    // If player plant, then transfer player from seed to plant
                    if (plant->plantType == Plant::PlantType::PLAYER) {
                        plant->playerPlant = tool->playerPlant;
					}

                    // set plant position, direction, and object id manually
                    plant->position = new Position(currTile->getCenterPosition());
                    plant->direction = new Direction(player->direction);
                    plant->objectId = objectCount++;

                    plants.push_back(plant);
                    gameObjectMap[plant->objectId] = plant;
                    currTile->plantId = plant->objectId;
                    //std::cout << "Seed planted" << std::endl;

                    // Delete the seed tool
                    auto it = std::find(tools.begin(), tools.end(), tool);
                    if (it != tools.end()) {
                        tools.erase(it);
                    }
                    delete tool;

                }
                break;

            case Tool::ToolType::PESTICIDE:
                if (player->highlightObjectId != 0) {
                    Plant* plant = (Plant*)gameObjectMap[player->highlightObjectId];

                    // assuming theres bugs on the plants
                    if (plant->aliveTime >= plant->activeTime) {
                        //std::cout << "Current plant spraying progress: " << plant->currSprayTime << std::endl;
                        plant->currSprayTime += deltaTime;
                        plant->aliveTime -= deltaTime;
                        player->animation->animationType = Player::PlayerAnimation::WATER;

                        if (plant->currSprayTime >= plant->pesticideSprayTime) {
                            plant->currSprayTime = 0.0f;
                            plant->aliveTime = 0.0f;
                            plant->isAttackedByBugs = false;
                        }
                    }
                }
                break;

            case Tool::ToolType::FERTILIZER:
                if (player->highlightObjectId != 0) {
                    Plant* plant = (Plant*)gameObjectMap[player->highlightObjectId];

                    // Fertilizer done, reset time and increase attack
                    if (plant->currFertilizeTime >= plant->fertilizerCompleteTime) {
                        //std::cout << "Fertilize Done " << std::endl;
                        plant->currFertilizeTime = 0.0f;
                        tool->fertilizerCurrTime = 0.0f;

                        // decrease attack interval
                        plant->attackInterval -= plant->deltaAttackInterval;
                        if (plant->attackInterval <= plant->minAttackInterval) {
                            plant->attackInterval = plant->minAttackInterval;
                        }

                        // increase attack power
                        plant->attackPower += plant->deltaAttack;
                        plant->level++;
                    }

                    // if cooldown is not active, fertilize plant
                    if (tool->fertilizerCurrTime >= tool->fertilizerCooldownTime) {
                        //std::cout << "Current plant fertilizing progress: " << plant->currFertilizeTime << std::endl;
                        plant->currFertilizeTime += deltaTime;
                        player->animation->animationType = Player::PlayerAnimation::WATER;
                    }
                    else {
                        //std::cout << "Cannot fertilize since fertilizer is in cooldown: " << tool->fertilizerCurrTime << std::endl;
                    }
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
                dropTool(player);
            }
            else if (player->highlightObjectId != 0) {
                // Get seed if highlighted id is seedshack
                SeedShack* highlightedShack = nullptr;
                for (SeedShack* seedShack : seedShacks) {
                    if (player->highlightObjectId == seedShack->objectId) {
                        highlightedShack = seedShack;
                        break;
					}
                }

                if (highlightedShack) {
                    Tool* seed = new Tool(
                        new Position(player->position),
                        new Direction(player->direction),
                        new Animation(),
                        objectCount,
                        config.seedBagRadius,
                        Tool::ToolType::SEED,
                        player->objectId,
                        true
                    );
                    seed->seedType = highlightedShack->seedType;
                    gameObjectMap[objectCount++] = seed;
                    tools.push_back(seed);
                    player->holding = true;
                    player->heldObject = seed->objectId;
                    //std::cout << "Pick up seed from seed shack at (" << highlightedShack->position->x << ", " << highlightedShack->position->z << ")" << std::endl;
                }
                else {
                    Tool* currTool = (Tool*)gameObjectMap[player->highlightObjectId];
                    //std::cout << "Pick up tool at (" << currTool->position->x << ", " << currTool->position->z << ")" << std::endl;
                    player->holding = true;
                    player->heldObject = currTool->objectId;
                    currTool->heldBy = player->objectId;
                    currTool->held = true;
                    currTool->direction->angle = player->direction->angle;
                }

                // Reset highlight id when a tool is picked up
                player->highlightObjectId = 0;
            }
        }
    }

    void updatePlants() {
        for (auto it = std::begin(plants); it != std::end(plants);) {
            Plant* plant = *it;
            if (plant->growStage == Plant::GrowStage::GROWN) {
                // Grown stage
                if (plant->aliveTime >= plant->deathTime) {
                    // Plant is dead, get rid of it
                    Tile* tile = getCurrentTile(plant);
                    tile->canPlow = true;
                    tile->tileType = Tile::TYPE_NORMAL;
                    tile->plantId = 0;
                    it = plants.erase(it);
                    continue;
                }

                if (plant->aliveTime < plant->activeTime) {
                    // Attack zombies
                    plantAttack(plant);
                }
                else {
                    plant->currAttackTime = 0.0f;
                    //std::cout << "Bugs Attacking plant! Use pesticide!!!" << std::endl;
                    plant->isAttackedByBugs = true;
                }

                plant->aliveTime += deltaTime;
            }
            else {
                // Still Growing
                if (plant->growProgressTime >= plant->growExpireTime) {
                    std::cout << "Plant growth complete, going next stage" << std::endl;
                    plant->growStage++;
                    plant->growProgressTime = 0.0f;
                    plant->cooldownTime = plant->coolDownExpireTime;
                }
                if (plant->cooldownTime > 0) {
                    plant->cooldownTime -= deltaTime;
                }

                // Handle the newly grown player plant within the same tick
                if (plant->growStage == Plant::GrowStage::GROWN 
                    && plant->plantType == Plant::PlantType::PLAYER) {
                    // If plant is a dplayer plant, revive player
                    Player* revivedPlayer = plant->playerPlant;
                    revivedPlayer->animation->animationType = Player::PlayerAnimation::IDLE;
                    revivedPlayer->isDead = false;
                    revivedPlayer->health = revivedPlayer->maxHealth;
                    revivedPlayer->position->x = plant->position->x;
                    revivedPlayer->position->y = plant->position->y;
                    revivedPlayer->position->z = plant->position->z;
                    it = plants.erase(it);

                    // Restore tile info
                    Tile* tile = getCurrentTile(plant);
                    tile->canPlow = true;
                    tile->tileType = Tile::TYPE_NORMAL;
                    tile->plantId = 0;
                    continue;
                }
            }

            // TODO: handle spawn bullets

            it++;
        }
    }


    void updateTools() {
        for (Tool* tool: tools) {
            switch (tool->toolType) {
            case Tool::ToolType::FERTILIZER:
                tool->fertilizerCurrTime += deltaTime;
                break;
            }
        }
    }

    void updatePlayersPosition() {
        for (Player* player : players) {
            if (player->isDead) {
                continue;
			}

            Position prevPos(player->position);
            // 1. Update position
            movePlayer(player);

            // 2. Check if collide with zombies
            bool collideWithZombie = false;
            for (Zombie* zombie : zombies) {
                if (player->collideWith(zombie)) {
                    if (player->invincibleTime <= 0) {
                        player->health--;
                        if (player->health <= 0) {
                            // set player to "dead"
                            player->isDead = true;

                            // spawn a seed bag with type PLAYER
                            Tool* playerSeed = new Tool(
                                new Position(player->position),
                                new Direction(player->direction),
                                new Animation(),
                                objectCount++,
                                config.seedBagRadius,
                                Tool::ToolType::SEED,
                                0,
                                false
                            );
                            playerSeed->seedType = Plant::PlantType::PLAYER;
                            playerSeed->playerPlant = player;
                            gameObjectMap[playerSeed->objectId] = playerSeed;
                            tools.push_back(playerSeed);

                            // Drop the tool if holding any
                            if (player->holding) {
                                dropTool(player);
                            }

                            break;
                        } 
                        
                        // player position bounce back
                        std::cout << "Collide with zombie, current health = " << player->health << std::endl;
                        float dir = player->direction->getOppositeDirection();
                        float dz = std::cos(dir);
                        float dx = std::sin(dir);

                        // Check x axis hit boundary
                        float playerToBoundX;
                        if (dx >= 0) {
                            // get distance from player to the bottom boundary
                            playerToBoundX = floor->boundMaxX - player->position->x;
                        }
                        else {
                            // get distance from player to the top boundary
                            playerToBoundX = floor->boundMinX - player->position->x;
                        }

                        // Check z axis hit boundary
                        float playerToBoundZ;
                        if (dz >= 0) {
                            // get distance from player to the bottom boundary
                            playerToBoundZ = floor->boundMaxZ - player->position->z;
                        }
                        else {
                            // get distance from player to the top boundary
                            playerToBoundZ = floor->boundMinZ - player->position->z;
                        }

                        //std::cout << "Player to z boundary is " << playerToBoundZ << std::endl;
                        //std::cout << "cos(dir) is " << dz << std::endl;
                        //std::cout << "Player to x boundary is " << playerToBoundX << std::endl;
                        //std::cout << "sin(dir) is " << dx << std::endl;

                        float knockBackDist = config.playerKnockBackMaxDistance;
                        if (playerToBoundZ / dz < knockBackDist) {
                            knockBackDist = playerToBoundZ / dz;
                        }
                        if (playerToBoundX / dx < knockBackDist) {
                            knockBackDist = playerToBoundX / dx;
                        }

                        player->position->x = prevPos.x + knockBackDist * dx;
                        player->position->z = prevPos.z + knockBackDist * dz;

                        // Look for nearest non-zombie tile
                        int nextRow = player->position->z / Floor::TILE_SIZE;
                        int nextCol = player->position->x / Floor::TILE_SIZE;

                        std::vector<Tile*> nearTiles;
                        for (int row = nextRow - 1; row <= nextRow + 1; row++) {
                            for (int col = nextCol - 1; col <= nextCol + 1; col++) {
                                if (row >= 0 && row < floor->tiles.size() && col >= 0 && col < floor->tiles[0].size()) {
                                    nearTiles.push_back(floor->tiles[row][col]);
                                }
                            }
                        }

                        float minDistanceToTile = std::numeric_limits<float>::max();
                        Tile* translateTile = nullptr;
                        for (Tile* tile : nearTiles) {
                            Position tilePos = tile->getCenterPosition();
                            float tileDist = player->distanceTo(tilePos);
                            if (tileDist < minDistanceToTile && tile->tileType != Tile::TYPE_ZOMBIE) {
                                minDistanceToTile = tileDist;
                                translateTile = tile;
                            }
                        }

                        if (translateTile) {
                            Position translateTilePos = translateTile->getCenterPosition();
                            player->position->x = translateTilePos.x;
                            player->position->z = translateTilePos.z;

                        }


                        player->invincibleTime = config.playerRespawnInvincibleTime;
                    }
                    else {
                        // In invincible mode player stays in previous position when collide with zombie
                        player->position->x = prevPos.x;
                        player->position->z = prevPos.z;
                    }

                    break;
                }
            }

            // 3. Check if collide with wall
            if (player->position->x < 0 || player->position->x > floor->tiles[0].size() * 1.0) {
                //std::cout << "Collide with wall" << std::endl;
                player->position->x = prevPos.x;
            }
            if (player->position->z < 0 || player->position->z > floor->tiles.size() * 1.0) {
                //std::cout << "Collide with wall" << std::endl;
                player->position->z = prevPos.z;
            }

            if (player->invincibleTime > 0) {
                player->invincibleTime -= deltaTime;
            }

            // 4. Check if collide with players
            for (Player* otherPlayer : players) {
                if (otherPlayer == player || otherPlayer->isDead) {
                    continue;
                }

                if (player->collideWith(otherPlayer)) {
                    player->position->x = prevPos.x;
                    player->position->z = prevPos.z;
                    break;
                }
            }

            // 5. Check if collide with plants
            for (Plant* plant : plants) {
                if (player->collideWith(plant)) {
                    collisionResponse(player, plant, prevPos);
                    break;
                }
            }

            // 6. Check if collide with seedshacks
            for (SeedShack* seedShack : seedShacks) {
                if (player->collideWith(seedShack)) {
                    collisionResponse(player, seedShack, prevPos);
                    break;
                }
            }

            // 7. Check if collide with watertap
            if (player->collideWith(waterTap)) {
                collisionResponse(player, waterTap, prevPos);
            }

            // 8. Check if collide with obstacles 
            for (Obstacle* obstacle : obstacles) {
                if (player->collideWith(obstacle)) {
                    collisionResponse(player, obstacle, prevPos);
                    break;
                }
            }

            player->currRow = player->position->z / Floor::TILE_SIZE;
            player->currCol = player->position->x / Floor::TILE_SIZE;
        }
    }

    void movePlayer(Player* player) {
        float translateDistance = 0.0f;
        float speedX = 0.0f;
        float speedZ = 0.0f;
        switch (player->moveState) {
        case Player::MoveState::DOWN:
            translateDistance = checkRotation(player->direction, Direction::DIRECTION_DOWN, false);
            speedZ = 1.0f;
            break;
        case Player::MoveState::LOWER_RIGHT:
            translateDistance = checkRotation(player->direction, Direction::DIRECTION_LOWER_RIGHT, true);
            speedZ = 1.0f;
            speedX = 1.0f;
            break;
        case Player::MoveState::RIGHT:
            translateDistance = checkRotation(player->direction, Direction::DIRECTION_RIGHT, false);
            speedX = 1.0f;
            break;
        case Player::MoveState::UPPER_RIGHT:
            translateDistance = checkRotation(player->direction, Direction::DIRECTION_UPPER_RIGHT, true);
            speedZ = -1.0f;
            speedX = 1.0f;
            break;
        case Player::MoveState::UP:
            translateDistance = checkRotation(player->direction, Direction::DIRECTION_UP, false);
            speedZ = -1.0f;
            break;
        case Player::MoveState::UPPER_LEFT:
            translateDistance = checkRotation(player->direction, Direction::DIRECTION_UPPER_LEFT, true);
            speedZ = -1.0f;
            speedX = -1.0f;
            break;
        case Player::MoveState::LEFT:
            translateDistance = checkRotation(player->direction, Direction::DIRECTION_LEFT, false);
            speedX = -1.0f;
            break;
        case Player::MoveState::LOWER_LEFT:
            translateDistance = checkRotation(player->direction, Direction::DIRECTION_LOWER_LEFT, true);
            speedZ = 1.0f;
            speedX = -1.0f;
            break;
        case Player::MoveState::FREEZE:
            // If no action-related animation is set, overwrite animation with IDLE
            if (player->animation->animationType == Player::PlayerAnimation::MOVE) {
                player->animation->animationType = Player::PlayerAnimation::IDLE;
            }
            break;
        }
        player->position->z += speedZ * translateDistance * deltaTime;
        player->position->x += speedX * translateDistance * deltaTime;
    }

    float checkRotation(Direction* playerDirection, float moveDirection, bool isDiagonal) {
        float translateDistance;
        if (playerDirection->directionEquals(moveDirection)) {
            playerDirection->angle = moveDirection;
            translateDistance = isDiagonal ? config.playerDiagonalMoveSpeed : config.playerMoveSpeed;
        }
        else if (playerDirection->clockwiseCloser(moveDirection)) {
            playerDirection->angle += config.playerRotationSpeed * deltaTime;
            playerDirection->constrainAngle();
            translateDistance = isDiagonal ? config.playerInRotationDiagonalMoveSpeed : config.playerInRotationMoveSpeed;
        }
        else {
            playerDirection->angle -= config.playerRotationSpeed * deltaTime;
            playerDirection->constrainAngle();
            translateDistance = isDiagonal ? config.playerInRotationDiagonalMoveSpeed : config.playerInRotationMoveSpeed;
        }
        return translateDistance;
    }

    void updateZombies() {
        zombieWaveManager->handleZombieWaves();
        waveNum = zombieWaveManager->waveNum;

        for (auto i = std::begin(zombies); i != std::end(zombies);) {
            Zombie* zombie = (*i);
            float paddingZ = 0, paddingX = 0;

            // Move current zombie
            Direction* currDir = zombie->direction;
            if (currDir->directionEquals(Direction::DIRECTION_DOWN)) {
                zombie->position->z += zombie->moveSpeed * deltaTime;
                paddingZ -= Tile::TILE_PAD_Z;
            } 
            else if (currDir->directionEquals(Direction::DIRECTION_RIGHT)) {
                zombie->position->x += zombie->moveSpeed * deltaTime;
                paddingX -= Tile::TILE_PAD_X;
            } 
            else if (currDir->directionEquals(Direction::DIRECTION_UP)) {
                zombie->position->z -= zombie->moveSpeed * deltaTime;
                paddingZ += Tile::TILE_PAD_Z;
            } 
            else if (currDir->directionEquals(Direction::DIRECTION_LEFT)) {
                zombie->position->x -= zombie->moveSpeed * deltaTime;
                paddingX += Tile::TILE_PAD_X;
            }

            // Calculate the current effecting tile's row&column
            int row = zombie->position->z + paddingZ;
            int col = zombie->position->x + paddingX;
            Tile* currTile = floor->tiles[row][col];

            // Check if remove current zombie (final tile / hp = 0, ...)
            bool reachedFinalTile = row == floor->zombieFinalTileRow
                && col == floor->zombieFinalTileCol;

            if (reachedFinalTile) {
                homeBase->health--;
                std::cout << "Health of base is " << homeBase->health << "/" << homeBase->maxHealth << std::endl;
                i = zombies.erase(i);
                continue;
            }

            if (zombie->health <= 0) {
                numZombiesKilled++;
                std::cout << "Number of zombies killed: " << numZombiesKilled << std::endl;
                i = zombies.erase(i);
                continue;
            }

            i++;

            // Rotate current zombie
            if (currTile->tileType != Tile::TYPE_NORMAL)
                zombie->direction->angle = currTile->direction->angle;
        }
    }

    void plantAttack(Plant* plant) {
        if (plant->growStage != Plant::GrowStage::GROWN) {
            return;
        }

        bool zombieInRange = false;
        for (Zombie* zombie : zombies) {
            zombie->animation->animationType = Zombie::MOVE;
            if (zombie->distanceTo(plant) < plant->range->rangeDistance) {
                zombieInRange = true;
            }
        }

        if (!zombieInRange) {
            return;
        }

        if (plant->currAttackTime < plant->attackInterval) {
            plant->currAttackTime += deltaTime;
            return;
        }

        switch (plant->plantType) {
        case Plant::PlantType::CORN:
            //std::cout << "Distance to zombie is " << zombie->distanceTo(plant) << std::endl;
            //std::cout << "Corn perform attack to zombies" << std::endl;
            for (Zombie* zombie : zombies) {
                if (zombie->distanceTo(plant) < plant->range->rangeDistance) {
                    zombie->health -= plant->attackPower;
                    zombie->animation->animationType = Zombie::DAMAGED;
                }
            }
            break;
        case Plant::PlantType::CACTUS:
            //std::cout << "Cactus perform attack to zombies" << std::endl;
            // spawn a bullet
            CactusBullet* bullet = new CactusBullet(
                new Position(plant->position),
                new Direction(plant->direction),
                new Animation(0, 0),
                objectCount++,
                config.cactusBulletRadius
            );
            bullet->attackPower = plant->attackPower;
            gameObjectMap[bullet->objectId] = bullet;
            bullets.push_back(bullet);
            break;
        }
        plant->currAttackTime = 0.0f;
    }

    void updateBullets() {
        for (CactusBullet* bullet : bullets) {
            // Move first
            float dz = std::cos(bullet->direction->angle);
            float dx = std::sin(bullet->direction->angle);

            bullet->position->z += config.cactusBulletSpeed * dz * deltaTime;
            bullet->position->x += config.cactusBulletSpeed * dx * deltaTime;
        }

        for (auto i = std::begin(bullets); i != std::end(bullets);) {
            // Check if bullet collides with zombie
            CactusBullet* bullet = *i;
            bool collided = false;
            for (Zombie* zombie : zombies) {
                collided = bullet->collideWith(zombie);
                if (collided) {
                    zombie->health -= bullet->attackPower;
                    zombie->animation->animationType = Zombie::DAMAGED;
                    i = bullets.erase(i);
                    break;
                }
            }


            if (!collided) {
                // delete if out of bounds
                float minX = 0;
                float minZ = 0;
                float maxX = floor->tiles[0].size() * Floor::TILE_SIZE;
                float maxZ = floor->tiles.size() * Floor::TILE_SIZE;

                Position* bulletPos = bullet->position;
                if (bulletPos->x < 0 || bulletPos->x >= maxX || bulletPos->z < 0 || bulletPos->z >= maxZ) {
                    i = bullets.erase(i);
                }
                else {
                    i++;
                }
            }

        }
    }

    void updatePlayersHighlight() {
        for (Player* player: players) {
            if (player->holding) {
                // highlight interactable objects
                Tool* currTool = (Tool*)gameObjectMap[player->heldObject];
                switch (currTool->toolType) {
                case Tool::ToolType::WATER_CAN: {
                    // highlight plants or water tap
                    float minDistance = std::numeric_limits<float>::max();
                    Plant* highlightPlant = nullptr;
                    for (Plant* plant : plants) {
                        float dist = player->distanceTo(plant);

                        Position playerPlantVec = Position(
                            plant->position->x - player->position->x,
                            plant->position->y - player->position->y,
                            plant->position->z - player->position->z
                        );
                        float angle = player->direction->getAngleBetween(playerPlantVec);

                        if (dist < minDistance && angle <= config.highlightFOVAngle) {
                            highlightPlant = plant;
                            minDistance = dist;
                        }
                    }

                    // Check distance to waterTap as well
                    bool waterTapClosest = false;
                    float dist = player->distanceTo(waterTap);
                    Position playerWaterTapVec = Position(
                        waterTap->position->x - player->position->x,
                        waterTap->position->y - player->position->y,
                        waterTap->position->z - player->position->z
                    );
                    float angle = player->direction->getAngleBetween(playerWaterTapVec);

                    if (dist < minDistance && angle <= config.highlightFOVAngle) {
                        minDistance = dist;
                        waterTapClosest = true;
                    }

                    player->highlightTileRow = -1;
                    player->highlightTileCol = -1;
                    if (waterTapClosest && player->highlightCollideWith(waterTap) && currTool->remainingWater < currTool->capacity) {
                        player->highlightObjectId = waterTap->objectId;
                        player->highlightTileRow = waterTap->position->z / Floor::TILE_SIZE;
                        player->highlightTileCol = waterTap->position->x / Floor::TILE_SIZE;
                        //std::cout << "Highlighting WaterTap" << std::endl;
                    }
                    else {
                        // interacting with tools
                        // Make sure tool is within collision range and is not held by others 
                        if (highlightPlant && highlightPlant->growStage != Plant::GrowStage::GROWN && player->highlightCollideWith(highlightPlant)) {
                            player->highlightObjectId = highlightPlant->objectId;
                            if (currTool->remainingWater > 0) {
                                player->highlightTileRow = highlightPlant->position->z / Floor::TILE_SIZE;
                                player->highlightTileCol = highlightPlant->position->x / Floor::TILE_SIZE;
                            }
                            //std::cout << "Highlighting plant at (" << highlightPlant->position->x << ", " << highlightPlant->position->z << ")" << std::endl;
                        }
                        else {
                            player->highlightObjectId = 0;
                            //std::cout << "Nothing is highlighted" << std::endl;
                        }
                    }
                    break;
                }

                case Tool::ToolType::PLOW: {
                    checkTileHighlight(player, Tile::TYPE_NORMAL);
                    break;
                }

                case Tool::ToolType::SEED: {
                    // highlight tilled tiles
                    checkTileHighlight(player, Tile::TYPE_TILLED);
                    break;
                }

                case Tool::ToolType::PESTICIDE: {
                    // highlight grown plants
                    float minDistance = std::numeric_limits<float>::max();
                    Plant* highlightPlant = nullptr;
                    for (Plant* plant : plants) {
                        float dist = player->distanceTo(plant);

                        Position playerPlantVec = Position(
                            plant->position->x - player->position->x,
                            plant->position->y - player->position->y,
                            plant->position->z - player->position->z
                        );
                        float angle = player->direction->getAngleBetween(playerPlantVec);

                        if (dist < minDistance && angle <= config.highlightFOVAngle) {
                            highlightPlant = plant;
                            minDistance = dist;
                        }
                    }
                    player->highlightTileRow = -1;
                    player->highlightTileCol = -1;

                    if (highlightPlant
                        && highlightPlant->growStage == Plant::GrowStage::GROWN
                        && highlightPlant->aliveTime > highlightPlant->activeTime
                        && highlightPlant->aliveTime < highlightPlant->deathTime
                        && player->highlightCollideWith(highlightPlant)) {
                        player->highlightObjectId = highlightPlant->objectId;
                        player->highlightTileRow = highlightPlant->position->z / Floor::TILE_SIZE;
                        player->highlightTileCol = highlightPlant->position->x / Floor::TILE_SIZE;
                        //std::cout << "Highlighting plant at (" << highlightPlant->position->x << ", " << highlightPlant->position->z << ")" << std::endl;
                    }
                    else {
                        player->highlightObjectId = 0;
                        //std::cout << "Nothing is highlighted" << std::endl;
                    }
                    break;
                }
                case Tool::ToolType::FERTILIZER: {
                    // highlight grown plants
                    float minDistance = std::numeric_limits<float>::max();
                    Plant* highlightPlant = nullptr;
                    for (Plant* plant : plants) {
                        float dist = player->distanceTo(plant);

                        Position playerPlantVec = Position(
                            plant->position->x - player->position->x,
                            plant->position->y - player->position->y,
                            plant->position->z - player->position->z
                        );
                        float angle = player->direction->getAngleBetween(playerPlantVec);

                        if (dist < minDistance && angle <= config.highlightFOVAngle) {
                            highlightPlant = plant;
                            minDistance = dist;
                        }
                    }
                    player->highlightTileRow = -1;
                    player->highlightTileCol = -1;

                    if (highlightPlant
                        && highlightPlant->growStage == Plant::GrowStage::GROWN
                        && currTool->fertilizerCurrTime >= currTool->fertilizerCooldownTime
                        && player->highlightCollideWith(highlightPlant)) {
                        player->highlightObjectId = highlightPlant->objectId;
                        player->highlightTileRow = highlightPlant->position->z / Floor::TILE_SIZE;
                        player->highlightTileCol = highlightPlant->position->x / Floor::TILE_SIZE;
                        //std::cout << "Highlighting plant at (" << highlightPlant->position->x << ", " << highlightPlant->position->z << ")" << std::endl;
                    }
                    else {
                        player->highlightObjectId = 0;
                        //std::cout << "Nothing is highlighted" << std::endl;
                    }
                    break;
                }
                }
            }
            else {
                // highlight tools when not holding anything
                // Loop through tools and check if player collides with them
                // Highlight the closest tool that is in front direction of the player
                Tool* currTool = nullptr;
                float minDistance = std::numeric_limits<float>::max();
                for (Tool* tool : tools) {
                    float dist = player->distanceTo(tool);

                   Position playerToolVec = Position(
                        tool->position->x - player->position->x,
                        tool->position->y - player->position->y,
                        tool->position->z - player->position->z
                    );
                    float angle = player->direction->getAngleBetween(playerToolVec);

                    if (dist < minDistance && angle <= config.highlightFOVAngle) {
                        currTool = tool;
                        minDistance = dist;
                    }
                }

                // Check distance to seedShack as well
                SeedShack* highlightedShack = nullptr;
                for (SeedShack* seedShack : seedShacks) {
                    float dist = player->distanceTo(seedShack);
                    Position playerSeedShackVec = Position(
                        seedShack->position->x - player->position->x,
                        seedShack->position->y - player->position->y,
                        seedShack->position->z - player->position->z
                    );
                    float angle = player->direction->getAngleBetween(playerSeedShackVec);

                    if (dist < minDistance && angle <= config.highlightFOVAngle) {
                        minDistance = dist;
                        highlightedShack = seedShack;
                    }
                }

                if (highlightedShack && player->highlightCollideWith(highlightedShack)) {
                    player->highlightObjectId = highlightedShack->objectId;
                    //std::cout << "Highlighting seedShack" << std::endl;
                }
                else {
                    // interacting with tools
                    // Make sure tool is within collision range and is not held by others 
                    if (currTool && player->highlightCollideWith(currTool) && currTool->heldBy == 0) {
                        player->highlightObjectId = currTool->objectId;
                        //std::cout << "Highlighting tool at (" << currTool->position->x << ", " << currTool->position->z << ")" << std::endl;
                    } else {
                        player->highlightObjectId = 0;
                        //std::cout << "Nothing is highlighted" << std::endl;
                    }
                }

                // Turn off tile highlighting
                player->highlightTileRow = -1;
                player->highlightTileCol = -1;
            }   
            
		}
    }

    void checkTileHighlight(Player* player, const int TILE_TYPE) {
        // highlight normal tiles
        int highlightRow = -1;
        int highlightCol = -1;
        float minDistance = std::numeric_limits<float>::max();

        for (int row = player->currRow - 1; row <= player->currRow + 1; row++) {
            for (int col = player->currCol - 1; col <= player->currCol + 1; col++) {
                // Check out of bounds
                if (row < 0 || row >= floor->tiles.size() || col < 0 || col >= floor->tiles[0].size()) continue;

                Tile* currTile = floor->tiles[row][col];
                if (TILE_TYPE == Tile::TYPE_NORMAL && !currTile->canPlow) {
                    continue;
                }
                else if (TILE_TYPE == Tile::TYPE_TILLED && currTile->plantId != 0) {
                    continue;
                }
                Position centerPosition = currTile->getCenterPosition();
                float dist = player->distanceTo(centerPosition);

                Position playerTileVec = Position(
                    centerPosition.x - player->position->x,
                    centerPosition.y - player->position->y,
                    centerPosition.z - player->position->z
                );
                float angle = player->direction->getAngleBetween(playerTileVec);
                if (dist < minDistance && angle <= config.highlightFOVAngle) {
                    highlightRow = row;
                    highlightCol = col;
                    minDistance = dist;
                }
            }
        }

        bool validIndices = highlightRow != -1 && highlightCol != -1;
        bool tileTypeMatch = validIndices && floor->tiles[highlightRow][highlightCol]->tileType == TILE_TYPE;
        bool plantNotOnTile = validIndices && floor->tiles[highlightRow][highlightCol]->plantId == 0;

        // Highlight tile if types match and no plant is on it (latter condition affects planting seeds)
        if (tileTypeMatch && plantNotOnTile) {
            //std::cout << "Player at (" << player->currRow << ", " << player->currCol << ")" << std::endl;
            //std::cout << "Highlighting tile at (" << highlightRow << ", " << highlightCol << ")" << std::endl;
            player->highlightTileRow = highlightRow;
            player->highlightTileCol = highlightCol;
        }
        else {
            //std::cout << "No Tile highlighting" << std::endl;
            player->highlightTileRow = -1;
            player->highlightTileCol = -1;
        }
	}
    
    Tile* getCurrentTile(GameObject* object) {
        int row = object->position->z / Floor::TILE_SIZE;
        int col = object->position->x / Floor::TILE_SIZE;
        Tile* currTile = floor->tiles[row][col];
        return floor->tiles[row][col];
    }

    void initUnPlowableTiles() {
        for (SeedShack* seedShack : seedShacks) {
            Tile* tile = getCurrentTile(seedShack);
            tile->canPlow = false;
        }

        Tile* tile = getCurrentTile(waterTap);
        tile->canPlow = false;
    }

    void dropTool(Player* player) {
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

    void collisionResponse(Player* player, GameObject* object, Position& prevPos) {
        Position playerObjectVec = Position(
            //object->position->x - player->position->x,
            //object->position->y - player->position->y,
            //object->position->z - player->position->z
            object->position->x - prevPos.x,
            object->position->y - prevPos.y,
            object->position->z - prevPos.z
        );
        if (std::abs(playerObjectVec.x) <= std::abs(playerObjectVec.z)) {
            player->position->z = prevPos.z;
        }
        else {
            player->position->x = prevPos.x;
        }
    }

    bool checkIsGameOver() {
        bool areAllPlayersDead = true;
        for (Player* player : players) {
            if (!player->isDead) {
                areAllPlayersDead = false;
            }
        }

        bool isHomeBaseDead = false;
        if (homeBase->health <= 0) {
            isHomeBaseDead = true;
        }

        return areAllPlayersDead || isHomeBaseDead;
    }

    // We could use other data structures, for now use a list
    std::vector<Player*> players; // Up to 4 players?
    std::vector<Plant*> plants;
    std::vector<Zombie*> zombies;
    std::vector<Tool*> tools;
    std::vector<SeedShack*> seedShacks;
    std::vector<CactusBullet*> bullets;
    std::vector<Obstacle*> obstacles;

    std::vector<Position*> playerInitPositions;
    std::vector<Player*> tempPlayers;

    Floor* floor;
    WaterTap* waterTap;
    HomeBase* homeBase;

    // Zombie Spawn Manager
    ZombieWaveManager* zombieWaveManager;
    int waveNum = 0;
    int totalWaveNum;
    int numZombiesKilled = 0;

    // Game status losing/not losing
    bool isGameOver;

    // GameObject Map
    std::unordered_map<unsigned int, GameObject*> gameObjectMap;

    // start of at 100 so we can portnetially reserve special constants
    unsigned int objectCount = 100;

    // tick count
    long long tick;
    int tickRate;
    float deltaTime;
    float currentTime;

    // Configs
    ServerParams config;
};
#endif
