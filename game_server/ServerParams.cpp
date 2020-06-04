#include <inih/INIReader.h>
#include "ServerParams.h"

ServerParams::ServerParams() {
    INIReader config("ServerConfig.ini");

    if (config.ParseError() != 0) {
        throw std::exception("Cannot Load file ServerConfig.ini ");
    }

    port = config.GetInteger("GameServer", "port", 10032);
    tickrate = config.GetInteger("GameServer", "tickrate", 32);
    maxNumPlayers = config.GetInteger("GameServer", "MaxNumPlayers", 4);

    playerMaxHealth = config.GetInteger("PhysicsParams", "PlayerMaxHealth", 3);
    playerMoveSpeed = config.GetFloat("PhysicsParams", "PlayerMoveSpeed", 4.8f);
    playerDiagonalMoveSpeed = playerMoveSpeed / SQRT_2;
    playerInRotationMoveSpeed = config.GetFloat("PhysicsParams", "PlayerInRotationMoveSpeed", 1.6f);
    playerInRotationDiagonalMoveSpeed = playerInRotationMoveSpeed / SQRT_2;
    playerRotationSpeed = config.GetInteger("PhysicsParams", "PlayerRotationSpeed", 4) * PI;
    playerCatRadius = config.GetFloat("PhysicsParams", "PlayerCatRadius", 0.5f);
    zombieRabbitMoveSpeed = config.GetFloat("PhysicsParams", "ZombieRabbitMoveSpeed", 3.6f);
    zombieRabbitRadius = config.GetFloat("PhysicsParams", "ZombieRabbitRadius", 0.5f);
    zombiePigMoveSpeed = config.GetFloat("PhysicsParams", "ZombiePigMoveSpeed", 1.6f);
    zombiePigRadius = config.GetFloat("PhysicsParams", "ZombiePigRadius", 0.5f);
    highlightFOVAngle = PI / config.GetFloat("PhysicsParams", "HighlightFOVAngle", 3);
    playerKnockBackMaxDistance = config.GetFloat("PhysicsParams", "PlayerKnockBackMaxDistance", 5.0f);
    playerRespawnInvincibleTime = config.GetFloat("PhysicsParams", "PlayerRespawnInvincibleTime", 2.0f);
    seedBagRadius = config.GetFloat("PhysicsParams", "SeedBagRadius", 1.0f);

    // Zombie wave parameters
    waveDefaultNumZombies = config.GetInteger("ZombieWaves", "WaveDefaultNumZombies", 1);
    waveDeltaNumZombiesMultiplier = config.GetFloat("ZombieWaves", "WaveDeltaNumZombiesMultiplier", 1.2f);
    zombieRabbitDefaultHealth = config.GetInteger("ZombieWaves", "ZombieRabbitDefaultHealth", 100);
    zombieRabbitDeltaHealthMultiplier = config.GetFloat("ZombieWaves", "ZombieRabbitDeltaHealthMultiplier", 1.2f);
    zombieRabbitDeltaMoveSpeedMultiplier = config.GetFloat("ZombieWaves", "ZombieRabbitDeltaMoveSpeedMultiplier", 1.05f);
    zombiePigDefaultHealth = config.GetInteger("ZombieWaves", "ZombiePigDefaultHealth", 100);
    zombiePigDeltaHealthMultiplier = config.GetFloat("ZombieWaves", "ZombiePigDeltaHealthMultiplier", 1.2f);
    zombiePigDeltaMoveSpeedMultiplier = config.GetFloat("ZombieWaves", "ZombiePigDeltaMoveSpeedMultiplier", 1.02f);
    waveStartTime = config.GetFloat("ZombieWaves", "WaveStartTime", 5.0f);
    zombieRabbitSpawnFreq = config.GetFloat("ZombieWaves", "ZombieRabbitSpawnFreq", 1.0f);
    zombiePigSpawnFreq = config.GetFloat("ZombieWaves", "ZombiePigSpawnFreq", 1.0f);
    totalWaveNum = config.GetInteger("ZombieWaves", "TotalWaveNum", 10);

    // Plant Corn
    cornAttackPower = config.GetInteger("Corn", "AttackPower", 50);
    cornBoundingBoxRadius = config.GetFloat("Corn", "BoundingBoxRadius", 1.0f);
    cornAttackInterval = config.GetFloat("Corn", "AttackInterval", 1.0f);
    cornAttackRange = config.GetFloat("Corn", "AttackRange", 3.0f);
    cornGrowExpireTime = config.GetFloat("Corn", "GrowExpireTime", 2.0f);
    cornCooldownExpireTime = config.GetFloat("Corn", "CooldownExpireTime", 2.0f);
    cornActiveTime = config.GetFloat("Corn", "ActiveTime", 5.0f);
    cornDeathTime = config.GetFloat("Corn", "DeathTime", 5.0f);
    cornPesticideSprayTime = config.GetFloat("Corn", "PesticideSprayTime", 2.0f);
    cornFertilizerCompleteTime = config.GetFloat("Corn", "FertilizerCompleteTime", 2.0f);
    cornFertilizerDeltaAttack = config.GetInteger("Corn", "FertilizerDeltaAttack", 50);

    // Plant Cactus
    cactusAttackPower = config.GetInteger("Cactus", "AttackPower", 50);
    cactusBoundingBoxRadius = config.GetFloat("Cactus", "BoundingBoxRadius", 1.0f);
    cactusAttackInterval = config.GetFloat("Cactus", "AttackInterval", 1.0f);
    cactusAttackRange = config.GetFloat("Cactus", "AttackRange", 10.0f);
    cactusGrowExpireTime = config.GetFloat("Cactus", "GrowExpireTime", 2.0f);
    cactusCooldownExpireTime = config.GetFloat("Cactus", "CooldownExpireTime", 2.0f);
    cactusBulletSpeed = config.GetFloat("Cactus", "BulletSpeed", 9.0f);
    cactusBulletRadius = config.GetFloat("Cactus", "BulletRadius", 0.5f);
    cactusActiveTime = config.GetFloat("Cactus", "ActiveTime", 5.0f);
    cactusDeathTime = config.GetFloat("Cactus", "DeathTime", 15.0f);
    cactusPesticideSprayTime = config.GetFloat("Cactus", "PesticideSprayTime", 2.0f);
    cactusFertilizerCompleteTime = config.GetFloat("Cactus", "FertilizerCompleteTime", 2.0f);
    cactusFertilizerDeltaAttack = config.GetInteger("Cactus", "FertilizerDeltaAttack", 50);

    // Obstacle
    treeBoxRadius = config.GetFloat("Obstacle", "TreeBoxRadius", 0.5f);
    stoneBoxRadius = config.GetFloat("Obstacle", "StoneBoxRadius", 0.5f);

    // Levels
    int numLevels = config.GetInteger("Levels", "NumLevels", 4);
    for (unsigned int i = 0; i < numLevels; i++) {
        std::string currLevel = "Level" + std::to_string(i);
        levels.push_back(config.Get("Levels", currLevel, "InitGameState.ini"));
    }
}