#ifndef _GAME_STATE_H
#define _GAME_STATE_H

#include "Player.hpp"
#include "Plant.hpp"
#include "Zombie.hpp"
#include "Tool.hpp"
#include "Tile.hpp"
#include "SeedShack.hpp"
#include "WaterTap.hpp"

#include <vector>

class GameState {
public:
    GameState() {
        // Init the default state here
        // TODO: change them later
        int NUM_OF_PLAYER = 3;
        int NUM_OF_ZOMBIE = 3;

        // Init players
        for (int i = 0; i < NUM_OF_PLAYER; i++) {
            Position* playerPosition = new Position(i, 0, 0);
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
        for (int i = 0; i < NUM_OF_ZOMBIE; i++) {
            Position* zombiePosition = new Position(i*3, 0, 5.0);
            Direction* zombieDirection = new Direction(0.0);
            Animation* zombieAnimation = new Animation(0, 0);
            zombies.push_back(
                new Zombie(zombiePosition, zombieDirection, zombieAnimation, objectCount)
            );
            objectCount++;
        }

        // Init tools
        for (int i = 0; i < 2; i++) {
            Position* toolPosition = new Position(i, 1, 0);
            Direction* toolDirection = new Direction(0.0);
            Animation* toolAnimation = new Animation(0, 0);
            tools.push_back(new Tool(toolPosition, toolDirection, toolAnimation, objectCount, i, 0));
            objectCount++;
        }

        // Init tiles
        // TODO: for now, do a 5x5, but change it later
        for (int i = 0; i < 5; i++) {
            std::vector<Tile*> row;
            for (int j = 0; j < 5; j++) {
                Position* tilePosition = new Position(i, j, 0);
                Direction* tileDirection = new Direction(0);

                row.push_back(new Tile(tilePosition, 0, tileDirection));
            }
            tiles.push_back(row);
        }


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
        for (Player* player: players) {
            player->serialize(ar, version);
        }

        for (Plant* plant: plants) {
            plant->serialize(ar, version);
        }

        for (Zombie* zombie: zombies) {
            zombie->serialize(ar, version);
        }

        for (Tool* tool: tools) {
            tool->serialize(ar, version);
        }

        for (std::vector<Tile*> row: tiles) {
            for (Tile* tile: row) {
                tile->serialize(ar, version);
            }
        }

        seedShack->serialize(ar, version);
        waterTap->serialize(ar, version);
    }

    ~GameState() {
        for (int i = 0; i < players.size(); i++) {
            delete players[i];
        }

        for (int i = 0; i < plants.size(); i++) {
            delete plants[i];
        }

        for (int i = 0; i < zombies.size(); i++) {
            delete zombies[i];
        }

        for (int i = 0; i < tools.size(); i++) {
            delete tools[i];
        }

        for (int i = 0; i < tiles.size(); i++) {
            std::vector<Tile*> row = tiles[i];
            for (int j = 0; j < row.size(); j++) {
                delete row[j];
            }
        }

        delete seedShack;
        delete waterTap;
    }


    void updatePlayerPosition() {
        for (Player* player : players) {
            player->updatePosition();
        }
    }

    // We could use other data structures, for now use a list

    std::vector<Player*> players; // Up to 4 players?
    std::vector<Plant*> plants;
    std::vector<Zombie*> zombies;
    std::vector<Tool*> tools;

    // 2D array for tiles??
    std::vector<std::vector<Tile*>> tiles;

    SeedShack* seedShack; // Assuming there's 1 place to get seeds
    WaterTap* waterTap;

    // start of at 100 so we can portnetially reserve special constants
    unsigned int objectCount = 100;
};

#endif
