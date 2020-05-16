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
    zombieRabbitMoveSpeed = config.GetFloat("PhysicsParams", "ZombieRabbitMoveSpeed", 1.6f);
    highlightFOVAngle = PI / config.GetInteger("PhysicsParams", "HighlightFOVAngle", 3);
}