#include <inih/INIReader.h>
#include "ServerParams.h"

ServerParams::ServerParams() {
    INIReader config("ServerConfig.ini");

    if (config.ParseError() != 0) {
        throw std::exception("Cannot Load file ServerConfig.ini ");
    }

    port = config.GetInteger("GameServer", "port", 10032);
    tickrate = config.GetInteger("GameServer", "tickrate", 32);

    playerMoveSpeed = config.GetFloat("PhysicsParams", "PlayerMoveSpeed", 4.8f);
    playerDiagonalMoveSpeed = playerMoveSpeed / SQRT_2;
    playerInRotationMoveSpeed = config.GetFloat("PhysicsParams", "PlayerInRotationMoveSpeed", 1.6f);
    playerInRotationDiagonalMoveSpeed = playerInRotationMoveSpeed / SQRT_2;
    playerRotationSpeed = config.GetInteger("PhysicsParams", "PlayerRotationSpeed", 4) * PI;
    playerCatRadius = config.GetFloat("PhysicsParams", "PlayerCatRadius", 0.5f);
    zombieRabbitMoveSpeed = config.GetFloat("PhysicsParams", "ZombieRabbitMoveSpeed", 1.6f);
    zombieRabbitRadius = config.GetFloat("PhysicsParams", "ZombieRabbitRadius", 0.5f);
    highlightFOVAngle = PI / config.GetFloat("PhysicsParams", "HighlightFOVAngle", 3);
    playerKnockBackMaxDistance = config.GetFloat("PhysicsParams", "PlayerKnockBackMaxDistance", 5.0f);
    playerRespawnInvincibleTime = config.GetFloat("PhysicsParams", "PlayerRespawnInvincibleTime", 2.0f);

    // Zombie wave parameters
    waveDefaultNumZombies = config.GetInteger("ZombieWaves", "WaveDefaultNumZombies", 1);
    waveDeltaNumZombies = config.GetInteger("ZombieWaves", "WaveDeltaNumZombies", 2);
    zombieDefaultHealth = config.GetInteger("ZombieWaves", "ZombieDefaultHealth", 100);
    zombieDeltaHealth = config.GetInteger("ZombieWaves", "ZombieDeltaHealth", 20);
    waveStartTime = config.GetFloat("ZombieWaves", "WaveStartTime", 5.0f);
}