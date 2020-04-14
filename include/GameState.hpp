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
private:
    // We could use other data structures, for now use a list

    std::vector<Player*> players; // Up to 4 players?
    std::vector<Plant*> plants;
    std::vector<Zombie*> zombies;
    std::vector<Tool*> tools;

    // 2D array for tiles??
    std::vector<std::vector<Tile*>> tiles;

    SeedShack* seedShack; // Assuming there's 1 place to get seeds
    WaterTap* waterTap;
};