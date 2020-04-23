#pragma once

#define NO_TEXTURE "default_texture.png"
#define GRASS_TEXTURE "grass_texture.jpg"
#define WATER_TEXTURE "water_texture.jpg"

#define ZOMBIE_MODEL "model/rabbit_simple_animation.fbx"
#define PLAYER_MODEL "model/rabbit_simple_animation.fbx"
#define CORN_MODEL "model/corn.fbx"
#define WATER_TAP_MODEL "model/water_tap.fbx"

#define WATER_TAP_SCALER glm::mat4(.5, 0, 0, 0,		0, .5, 0, 0,	0, 0, .5, 0,		0, 0, 0, 1.0);
#define RABBIT_SCALER glm::mat4(.5, 0, 0, 0,		0, .5, 0, 0,	0, 0, .5, 0,		0, 0, 0, 1.0);
#define PLAYER_SCALER glm::mat4(.5, 0, 0, 0,		0, .5, 0, 0,	0, 0, .5, 0,		0, 0, 0, 1.0);


#define AUDIO_FILE_BGM "audio/animal\ dizhuing.wav"