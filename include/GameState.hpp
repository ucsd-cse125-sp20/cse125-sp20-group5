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

#include <cmath>
#include <vector>
#include <iostream>
#include <boost/serialization/vector.hpp>

const float SQRT_2 = 1.41421356237309504880f;
const float STEP_SIZE = 0.15f;
const float STEP_SIZE_DIAGONAL = STEP_SIZE / SQRT_2;
const float IN_ROTATION_STEP_SIZE = 0.05f;
const float IN_ROTATION_STEP_SIZE_DIAGONAL = IN_ROTATION_STEP_SIZE / SQRT_2;

class GameState {
public:
    GameState() : seedShack(nullptr), waterTap(nullptr) {}

    void init(int tick_rate) {
        tickRate = tick_rate;
        init();
	}

    void init() {
        // Init the default state here
        // TODO: change them later
        int NUM_OF_PLAYER = 3;
        int NUM_OF_ZOMBIE = 3;

        // Init players
        for (int i = 0; i < NUM_OF_PLAYER; i++) {
            Position* playerPosition = new Position(i*3, 0, 0);
            Direction* playerDirection = new Direction(0.0);
            Animation* playerAnimation = new Animation(0, 0);
            Color* playerColor = new Color(100, 100, 100);
            players.push_back(
                new Player(
                    playerPosition, playerDirection,
                    playerAnimation, objectCount, playerColor, i
                )
            );
            objectCount++;
        }

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
            Position* toolPosition = new Position(i, 1, 0);
            Direction* toolDirection = new Direction(0.0);
            Animation* toolAnimation = new Animation(0, 0);
            tools.push_back(new Tool(toolPosition, toolDirection, toolAnimation, objectCount, i, 0));
            players[0]->holding = true;
            players[0]->heldObject = tools[0]->objectId;
            objectCount++;
        }

        floor = new Floor();
        floor->init();

        // Init seed shack and water tap
        Position* seedShackPosition = new Position(3, 0, 3);
        Direction* seedShackDirection = new Direction(0);
        Animation* seedShackAnimation = new Animation(0, 0);
        seedShack = new SeedShack(seedShackPosition, seedShackDirection, seedShackAnimation, objectCount);
        objectCount++;

        Position* waterTapPosition = new Position(2, 0, 2);
        Direction* waterTapDirection = new Direction(0);
        Animation* waterTapAnimation = new Animation(0, 0);
        waterTap = new WaterTap(waterTapPosition, waterTapDirection, waterTapAnimation, objectCount);
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
        float translateDistance = 0.0f;
        //std::cout << "Before update angle = " << player->direction->angle << std::endl;
        switch (opCode) {
            case OPCODE_PLAYER_MOVE_DOWN:
                if (player->direction->directionEquals(Direction::DIRECTION_DOWN)) {
                    player->direction->angle = Direction::DIRECTION_DOWN;
                    translateDistance = STEP_SIZE;
                } else if (player->direction->clockwiseCloser(Direction::DIRECTION_DOWN)) {
                    player->direction->angle += Direction::ROTATION_SPEED;
                    player->direction->constrainAngle();
                    translateDistance = IN_ROTATION_STEP_SIZE;
                } else {
                    player->direction->angle -= Direction::ROTATION_SPEED;
                    player->direction->constrainAngle();
                    translateDistance = IN_ROTATION_STEP_SIZE;
                }
                player->position->z += translateDistance;
                break;
            case OPCODE_PLAYER_MOVE_LOWER_RIGHT:
                if (player->direction->directionEquals(Direction::DIRECTION_LOWER_RIGHT)) {
                    player->direction->angle = Direction::DIRECTION_LOWER_RIGHT;
                    translateDistance = STEP_SIZE_DIAGONAL;
                } else if (player->direction->clockwiseCloser(Direction::DIRECTION_LOWER_RIGHT)) {
                    player->direction->angle += Direction::ROTATION_SPEED;
                    player->direction->constrainAngle();
                    translateDistance = IN_ROTATION_STEP_SIZE_DIAGONAL;
                } else {
                    player->direction->angle -= Direction::ROTATION_SPEED;
                    player->direction->constrainAngle();
                    translateDistance = IN_ROTATION_STEP_SIZE_DIAGONAL;
                }
                player->position->z += translateDistance;
                player->position->x += translateDistance;
                break;
            case OPCODE_PLAYER_MOVE_RIGHT:
                if (player->direction->directionEquals(Direction::DIRECTION_RIGHT)) {
                    player->direction->angle = Direction::DIRECTION_RIGHT;
                    translateDistance = STEP_SIZE;
                } else if (player->direction->clockwiseCloser(Direction::DIRECTION_RIGHT)) {
                    player->direction->angle += Direction::ROTATION_SPEED;
                    player->direction->constrainAngle();
                    translateDistance = IN_ROTATION_STEP_SIZE;
                } else {
                    player->direction->angle -= Direction::ROTATION_SPEED;
                    player->direction->constrainAngle();
                    translateDistance = IN_ROTATION_STEP_SIZE;
                }
                player->position->x += translateDistance;
                break;
            case OPCODE_PLAYER_MOVE_UPPER_RIGHT:
                if (player->direction->directionEquals(Direction::DIRECTION_UPPER_RIGHT)) {
                    player->direction->angle = Direction::DIRECTION_UPPER_RIGHT;
                    translateDistance = STEP_SIZE_DIAGONAL;
                } else if (player->direction->clockwiseCloser(Direction::DIRECTION_UPPER_RIGHT)) {
                    player->direction->angle += Direction::ROTATION_SPEED;
                    player->direction->constrainAngle();
                    translateDistance = IN_ROTATION_STEP_SIZE_DIAGONAL;
                } else {
                    player->direction->angle -= Direction::ROTATION_SPEED;
                    player->direction->constrainAngle();
                    translateDistance = IN_ROTATION_STEP_SIZE;
                }
                player->position->z -= translateDistance;
                player->position->x += translateDistance;
                break;
            case OPCODE_PLAYER_MOVE_UP:
                if (player->direction->directionEquals(Direction::DIRECTION_UP)) {
                    player->direction->angle = Direction::DIRECTION_UP;
                    translateDistance = STEP_SIZE;
                }
                else if (player->direction->clockwiseCloser(Direction::DIRECTION_UP)) {
                    player->direction->angle += Direction::ROTATION_SPEED;
                    player->direction->constrainAngle();
                    translateDistance = IN_ROTATION_STEP_SIZE;
                }
                else {
                    player->direction->angle -= Direction::ROTATION_SPEED;
                    player->direction->constrainAngle();
                    translateDistance = IN_ROTATION_STEP_SIZE;
                }
                player->position->z -= translateDistance;
                break;
            case OPCODE_PLAYER_MOVE_UPPER_LEFT:
                if (player->direction->directionEquals(Direction::DIRECTION_UPPER_LEFT)) {
                    player->direction->angle = Direction::DIRECTION_UPPER_LEFT;
                    translateDistance = STEP_SIZE_DIAGONAL;
                }
                else if (player->direction->clockwiseCloser(Direction::DIRECTION_UPPER_LEFT)) {
                    player->direction->angle += Direction::ROTATION_SPEED;
                    player->direction->constrainAngle();
                    translateDistance = IN_ROTATION_STEP_SIZE_DIAGONAL;
                }
                else {
                    player->direction->angle -= Direction::ROTATION_SPEED;
                    player->direction->constrainAngle();
                    translateDistance = IN_ROTATION_STEP_SIZE;
                }
                player->position->z -= translateDistance;
                player->position->x -= translateDistance;
                break;
            case OPCODE_PLAYER_MOVE_LEFT:
                if (player->direction->directionEquals(Direction::DIRECTION_LEFT)) {
                    player->direction->angle = Direction::DIRECTION_LEFT;
                    translateDistance = STEP_SIZE;
                }
                else if (player->direction->clockwiseCloser(Direction::DIRECTION_LEFT)) {
                    player->direction->angle += Direction::ROTATION_SPEED;
                    player->direction->constrainAngle();
                    translateDistance = IN_ROTATION_STEP_SIZE;
                }
                else {
                    player->direction->angle -= Direction::ROTATION_SPEED;
                    player->direction->constrainAngle();
                    translateDistance = IN_ROTATION_STEP_SIZE;
                }
                player->position->x -= translateDistance;
                break;
            case OPCODE_PLAYER_MOVE_LOWER_LEFT:
                if (player->direction->directionEquals(Direction::DIRECTION_LOWER_LEFT)) {
                    player->direction->angle = Direction::DIRECTION_LOWER_LEFT;
                    translateDistance = STEP_SIZE_DIAGONAL;
                }
                else if (player->direction->clockwiseCloser(Direction::DIRECTION_LOWER_LEFT)) {
                    player->direction->angle += Direction::ROTATION_SPEED;
                    player->direction->constrainAngle();
                    translateDistance = IN_ROTATION_STEP_SIZE_DIAGONAL;
                }
                else {
                    player->direction->angle -= Direction::ROTATION_SPEED;
                    player->direction->constrainAngle();
                    translateDistance = IN_ROTATION_STEP_SIZE;
                }
                player->position->z += translateDistance;
                player->position->x -= translateDistance;
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
    }

    void update() {
        tick++;
        updateZombie();
    }

    void updateZombie() {
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
            Zombie* zombie = new Zombie(
                zombiePosition, zombieDirection, zombieAnimation, objectCount++
            );
            zombies.push_back(zombie);
		}

        // Move zombies
        for (Zombie* zombie : zombies) {
            int row = zombie->position->z - Tile::TILE_PAD_Z;
            int col = zombie->position->x - Tile::TILE_PAD_X;

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

            // Rotate zombie
            zombie->direction->angle = currTile->direction->angle;
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

    // start of at 100 so we can portnetially reserve special constants
    unsigned int objectCount = 100;

    // tick count
    long long tick = 0;
    int tickRate;

};

#endif
