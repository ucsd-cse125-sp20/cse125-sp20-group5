#pragma once

#define NO_TEXTURE "texture/default_texture.png"
#define GRASS_TEXTURE "texture/grass_texture.jpg"
#define WATER_TEXTURE "texture/water_texture.jpg"

#define ZOMBIE_MODEL "model/rabbit_simple_animation.fbx"
#define PLAYER_MODEL "model/cat.fbx"
#define CORN_MODEL "model/corn.fbx"
#define WATER_TAP_MODEL "model/water_tap.fbx"
#define WATERING_CAN_MODEL "model/watering_can.fbx"

#define WATER_TAP_SCALER_MAT glm::mat4(.5, 0, 0, 0,		0, .5, 0, 0,	0, 0, .5, 0,		0, 0, 0, 1.0)
#define RABBIT_SCALER_MAT glm::mat4(.5, 0, 0, 0,		0, .5, 0, 0,	0, 0, .5, 0,		0, 0, 0, 1.0)
#define PLAYER_SCALER_MAT glm::mat4(.5, 0, 0, 0,		0, .5, 0, 0,	0, 0, .5, 0,		0, 0, 0, 1.0)

#define WATER_TAP_SCALER 0.5
#define RABBIT_SCALER 0.5
#define PLAYER_SCALER 0.5
#define TOOL_SCALER 0.5

#define AUDIO_FILE_BGM "audio/animal\ dizhuing.wav"