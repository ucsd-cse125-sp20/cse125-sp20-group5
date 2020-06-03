#include "ZombieWaveManager.h"
#include "GameState.hpp"
#include "Zombie.hpp"
#include "Floor.hpp"

ZombieWaveManager::ZombieWaveManager() : gameState(nullptr) {}

ZombieWaveManager::ZombieWaveManager(GameState* state)
  : zombiesInWave(state->config.waveDefaultNumZombies),
    zombieRabbitHealthMultiplier(1.0f),
    zombiePigHealthMultiplier(1.0f),
    zombiesSpawned(0) 
{
    gameState = state;
    currTime = gameState->config.waveStartTime;
    waveNum = 0;
}

void ZombieWaveManager::spawn(Zombie::ZombieType type) {
    // Spawn zombie every second
    float modTime = 0.0;
    if (type == Zombie::ZombieType::RABBIT) {
        modTime = fmod(gameState->currentTime, gameState->config.zombieRabbitSpawnFreq);
    }
    else if (type == Zombie::ZombieType::PIG) {
        modTime = fmod(gameState->currentTime, gameState->config.zombiePigSpawnFreq);
    }
    if (modTime < 0.01) {
        Zombie* zombie = Zombie::buildZombie(gameState->config, type, gameState->floor->zombieBaseTile);
        zombie->objectId = gameState->objectCount++;

        
        if (type == Zombie::ZombieType::RABBIT) {
            zombie->maxHealth = gameState->config.zombieRabbitDefaultHealth * zombieRabbitHealthMultiplier;
            zombie->health = zombie->maxHealth;
        }
        else if (type == Zombie::ZombieType::PIG) {
            zombie->maxHealth = gameState->config.zombiePigDefaultHealth * zombiePigHealthMultiplier;
            zombie->health = zombie->maxHealth;
        }
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
                zombieRabbitHealthMultiplier += gameState->config.zombieRabbitDeltaHealthMultiplier;
                zombiePigHealthMultiplier += gameState->config.zombiePigDeltaHealthMultiplier;
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
