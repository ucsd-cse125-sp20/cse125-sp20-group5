#ifndef _FLOOR_H_
#define _FLOOR_H_

#include "GameObject.hpp"
#include "Tile.hpp"
#include <boost/serialization/vector.hpp>

class Floor : public GameObject {
public:
    Floor(): GameObject(), zombieBaseTile(nullptr) {}

    ~Floor() {
        for (std::vector<Tile*>& row : tiles) {
            for (Tile* tile : row) {
                delete tile;
            }
        }
        zombieBaseTile = nullptr;
    }

    void init() {
        int numRows = 15;
        int numCols = 20;
        // Init tiles
        // TODO: for now, do a 5x5, but change it later
        for (int i = 0; i < numRows; i++) {
            std::vector<Tile*> row;
            /*
            if (i < numRows - 1) {
                Position* tilePosition = new Position(0, 0, i);
                Direction* tileDirection = new Direction(Direction::DIRECTION_DOWN);
                row.push_back(new Tile(tilePosition, Tile::TYPE_ZOMBIE, tileDirection));

                for (int j = 1; j < numCols - 1; j++) {
                    tilePosition = new Position(j, 0, i);
                    tileDirection = new Direction(0);

                    row.push_back(new Tile(tilePosition, Tile::TYPE_NORMAL, tileDirection));
                }

                tilePosition = new Position(numCols - 1, 0, i);
                tileDirection = new Direction(Direction::DIRECTION_UP);
                row.push_back(new Tile(tilePosition, Tile::TYPE_ZOMBIE, tileDirection));
            }
            else {
                for (int j = 0; j < numCols - 1; j++) {
                    Position* tilePosition = new Position(j, 0, i);
                    Direction* tileDirection = new Direction(Direction::DIRECTION_RIGHT);

                    row.push_back(new Tile(tilePosition, Tile::TYPE_ZOMBIE, tileDirection));
                }
                Position* tilePosition = new Position(numCols - 1, 0, i);
                Direction* tileDirection = new Direction(Direction::DIRECTION_UP);
                row.push_back(new Tile(tilePosition, Tile::TYPE_ZOMBIE, tileDirection));
            }
            */

            for (int j = 0; j < numCols; j++) {
                Position* tilePosition = new Position(j, 0, i); 
                Direction* tileDirection;
                int tileType;
                
                // U shaped
                // if (i <= 1 || i >= width-2  || j >= height -2) {
                //     row.push_back(new Tile(tilePosition, 1, tileDirection));
                // }

                // 2 shaped
                // -----|
                //      |
                //      |------ shaped
                if (i <= 0 && j < 10) { // upper left ---
                    tileDirection = new Direction(Direction::DIRECTION_RIGHT);
                    tileType = Tile::TYPE_ZOMBIE;
                }
                else if (i >= 14 && j >= 10) { // lower right ----
                    tileDirection = new Direction(Direction::DIRECTION_RIGHT);
                    tileType = Tile::TYPE_ZOMBIE;
                }
                else if (j == 10) { // vertical |
                    tileDirection = new Direction(Direction::DIRECTION_DOWN);
                    tileType = Tile::TYPE_ZOMBIE;
                }
                else { // The basic ground
                    tileDirection = new Direction(0);
                    tileType = Tile::TYPE_NORMAL;
                }
                row.push_back(new Tile(tilePosition, tileType, tileDirection));
            }
            tiles.push_back(row);
        }

        zombieBaseTile = tiles[0][0];
        zombieFinalTileRow = numRows - 1;
        zombieFinalTileCol = numCols - 1;
    }

    void setZombieBaseTile(int row, int col) {
        zombieBaseTile = tiles[row][col];
	}

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & tiles;
    }

    Tile* zombieBaseTile;
    int zombieFinalTileRow;
    int zombieFinalTileCol;
    std::vector<std::vector<Tile*>> tiles;
};

#endif