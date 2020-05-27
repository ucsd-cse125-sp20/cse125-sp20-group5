#include "ZombieWaveManager.h"
#include "GameState.hpp"
#include "Zombie.hpp"
#include "Floor.hpp"

ZombieWaveManager::ZombieWaveManager() : gameState(nullptr) {}

ZombieWaveManager::ZombieWaveManager(GameState* state)
  : zombiesInWave(state->config.waveDefaultNumZombies),
    zombieHealth(state->config.zombieDefaultHealth),
    zombiesSpawned(0) 
{
    gameState = state;
    waveNum = 1;
}

void ZombieWaveManager::spawn() {
    // Spawn zombie every second
    int tick = gameState->tick;
    int tickRate = gameState->tickRate;
    if (tick % tickRate == 0) {
        // TODO: Need to find out the position of the tile
        // Position right now uses indices
        Position* zombieBasePos = gameState->floor->zombieBaseTile->position;
        Direction* zombieBaseDir = gameState->floor->zombieBaseTile->direction;
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
            zombiePosition, zombieDirection,
            zombieAnimation, gameState->objectCount++, gameState->config.zombieRabbitRadius
        );
        zombie->health = zombieHealth;
        zombie->maxHealth = zombieHealth;
        gameState->zombies.push_back(zombie);

        zombiesSpawned++;
    }
}

void ZombieWaveManager::handleZombieWaves() {
    if (!inWave) {
        if (currTime > 0) {
            currTime -= gameState->deltaTime;
        }
        else {
            inWave = true;
            currTime = gameState->config.waveStartTime;
            // handle increasing health and number of zombies
            zombiesSpawned = 0;
            zombiesInWave += gameState->config.waveDeltaNumZombies;
            zombieHealth += gameState->config.zombieDeltaHealth;
        }
    }

    if(inWave) {
        spawn();

        if (zombiesSpawned == zombiesInWave) {
            // End wave when enough zombies are spawned
            inWave = false;
            waveNum++;
        }
    }
}
