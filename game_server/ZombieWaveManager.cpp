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
    waveNum = 0;
}

void ZombieWaveManager::spawn(Zombie::ZombieType type) {
    // Spawn zombie every second
    int tick = gameState->tick;
    int tickRate = gameState->tickRate;
    if (tick % tickRate == 0) {
        Zombie* zombie = Zombie::buildZombie(gameState->config, type, gameState->floor->zombieBaseTile);
        zombie->objectId = gameState->objectCount++;
        zombie->health = zombieHealth;
        zombie->maxHealth = zombieHealth;
        gameState->gameObjectMap[zombie->objectId] = zombie;
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

            if (waveNum % 2 == 0) {
                zombiesInWave += gameState->config.waveDeltaNumZombies;
                zombieHealth += gameState->config.zombieDeltaHealth;
            }
        }
    }

    if(inWave) {
        if (waveNum % 2 == 0) {
            spawn(Zombie::ZombieType::PIG);
        }
        else if (waveNum % 2 == 1) {
            spawn(Zombie::ZombieType::RABBIT);
        }

        if (zombiesSpawned == zombiesInWave) {
            // End wave when enough zombies are spawned
            inWave = false;
            waveNum++;
        }
    }
}
