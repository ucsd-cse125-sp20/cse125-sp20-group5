#ifndef _SERVER_PARAMS_H
#define _SERVER_PARAMS_H

#include <vector>
#include <string>

class ServerParams {
public:

    // Network settings
    int port;
    int tickrate;
    int maxNumPlayers;

    // Physics settings
    int playerMaxHealth;
    float playerMoveSpeed;
    float playerDiagonalMoveSpeed;
    float playerInRotationMoveSpeed;
    float playerInRotationDiagonalMoveSpeed;
    float playerRotationSpeed;
    float playerCatRadius;
    float zombieRabbitMoveSpeed;
    float zombieRabbitRadius;
    float zombiePigMoveSpeed;
    float zombiePigRadius;
    float highlightFOVAngle;
    float playerKnockBackMaxDistance;
    float playerRespawnInvincibleTime;
    float seedBagRadius;


    // Zombie Waves related
    int waveDefaultNumZombies;
    int waveDeltaNumZombies;
    int zombieDefaultHealth;
    int zombieDeltaHealth;
    float waveStartTime;

    // Plant Corn
    int cornAttackPower;
    float cornBoundingBoxRadius;
    float cornAttackInterval;
    float cornAttackRange;
    float cornGrowExpireTime;
    float cornCooldownExpireTime;
    float cornActiveTime;
    float cornDeathTime;
    float cornPesticideSprayTime;
    float cornFertilizerCompleteTime;
    int cornFertilizerDeltaAttack;

    // Plant Cactus
    int cactusAttackPower;
    float cactusBoundingBoxRadius;
    float cactusAttackInterval;
    float cactusAttackRange;
    float cactusGrowExpireTime;
    float cactusCooldownExpireTime;
    float cactusBulletSpeed;
    float cactusBulletRadius;
    float cactusActiveTime;
    float cactusDeathTime;
    float cactusPesticideSprayTime;
    float cactusFertilizerCompleteTime;
    int cactusFertilizerDeltaAttack;

    // Obstacle
    float treeBoxRadius;
    float stoneBoxRadius;

    // Level names
    std::vector<std::string> levels;

    ServerParams();

    // Math constants
    static constexpr const float PI = 3.14159265358979323846f;
    static constexpr const float TWO_PI = 2 * PI;
    static constexpr const float FOUR_PI = 4 * PI;
    static constexpr const float PI_2 = 1.57079632679489661923f;
    static constexpr const float PI_4 = 0.785398163397448309616f;
    static constexpr const float SQRT_2 = 1.41421356237309504880f;

    static constexpr const float SPEED = 4.8f;
    static constexpr const float SPEED_DIAGONAL = SPEED / SQRT_2;
    static constexpr const float IN_ROTATION_SPEED = 1.6f;
    static constexpr const float IN_ROTATION_SPEED_DIAGONAL = IN_ROTATION_SPEED / SQRT_2;
};
#endif
